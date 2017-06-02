#include <iostream>
#include <cstdlib>
#include <vector>
#include <random>

#include <sys/mman.h>

#include "../../2014/ztimer.h"

using namespace std;

#include <immintrin.h>
#include <smmintrin.h>

#if defined(__GNUC__)
#define PORTABLE_ALIGN16 __attribute__((aligned(16)))
#else
#define PORTABLE_ALIGN16 __declspec(align(16))
#endif

#define NUM_RAND_GAP_JUMPS   100

const size_t TOTAL_QTY  = 1024*1024;

// This function computes the squared L2, but only for vectors where the number of elements is a multiple of 16 
float dist(const float* pVect1, const float* pVect2, size_t qty) {
  size_t qty16 = qty >> 4;

  const float* pEnd1 = pVect1 + (qty16 << 4);

  __m128  diff, v1, v2;
  __m128  sum = _mm_set1_ps(0);

  while (pVect1 < pEnd1) {
    // PREFETCH IS BAD HERE!!!
    //_mm_prefetch((char*)(pVect2 + 16), _MM_HINT_T0);
    v1 = _mm_loadu_ps(pVect1); pVect1 += 4;
    v2 = _mm_loadu_ps(pVect2); pVect2 += 4;
    diff = _mm_sub_ps(v1, v2);
    sum = _mm_add_ps(sum, _mm_mul_ps(diff, diff));

    v1 = _mm_loadu_ps(pVect1); pVect1 += 4;
    v2 = _mm_loadu_ps(pVect2); pVect2 += 4;
    diff = _mm_sub_ps(v1, v2);
    sum = _mm_add_ps(sum, _mm_mul_ps(diff, diff));

    v1 = _mm_loadu_ps(pVect1); pVect1 += 4;
    v2 = _mm_loadu_ps(pVect2); pVect2 += 4;
    diff = _mm_sub_ps(v1, v2);
    sum = _mm_add_ps(sum, _mm_mul_ps(diff, diff));

    v1 = _mm_loadu_ps(pVect1); pVect1 += 4;
    v2 = _mm_loadu_ps(pVect2); pVect2 += 4;
    diff = _mm_sub_ps(v1, v2);
    sum = _mm_add_ps(sum, _mm_mul_ps(diff, diff));
  }
  
  float PORTABLE_ALIGN16 TmpRes[4];

  _mm_store_ps(TmpRes, sum);
  float res = TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3];

  return res;
};

void gen_data(size_t N, size_t vec_size, vector<vector<float>>& data) {
  if (vec_size & 15) {
    cerr << "The size of the vector " << vec_size << " isn't a multiple of 16! (as it should be)" << endl;
    abort();
  }
  data.clear();
  data.resize(N);

  mt19937 gen;
  gen.seed(0);
  uniform_real_distribution<> dis(0, 1);

  for (size_t i = 0; i < N; ++i) {
    data[i].resize(vec_size);
    for (size_t k = 0; k < vec_size; ++k) {
      data[i][k] = dis(gen); 
    }
  }
  cout << "Generated " << N << " vectors of the size " << vec_size << endl;
}

enum eWalkMode {
  kSequential,
  kRandom,
  kRandomGap,
  kMaxWalkMode
};

const string get_walk_mode_name(eWalkMode walk_mode) {
  switch (walk_mode) {
    case kSequential: return "SEQ";  break;
    case kRandom:     return "RAND"; break;
    case kRandomGap:  return "RAND GAP"; break;
    default: 
      cerr << "Unknown walk mode code: " << walk_mode << endl; 
      abort();
  }
};

void get_walk_data(vector<unsigned> &walk_data, eWalkMode walk_mode) {
  if (walk_mode == kSequential) {
    for (unsigned i = 0; i < walk_data.size(); ++i)
      walk_data[i] = i;
  } else if (walk_mode == kRandom) {
    size_t N = walk_data.size();
    static linear_congruential_engine<unsigned, 48271, 0, 2147483647> gen;
    uniform_int_distribution<> dis(0, N - 1);
    
    for (size_t i = 0; i < N; ++i) {
      walk_data[i] = dis(gen);
    }
  } else if (walk_mode == kRandomGap) {
    size_t N = walk_data.size();
    static linear_congruential_engine<unsigned, 48271, 0, 2147483647> gen;
    if (2*NUM_RAND_GAP_JUMPS > N) {
      cerr << "The number of elements in the array is too small" << endl;
      abort();
    }
    uniform_int_distribution<> dis(0, N/NUM_RAND_GAP_JUMPS - 1);
    
    walk_data[0] = dis(gen);
    for (size_t i = 1; i < N; ++i) {
      walk_data[i] = (dis(gen) + walk_data[i-1]) % N;
    }
  } else {
    cerr << "Unknown walk mode code: " << walk_mode << endl; 
    abort();
  }
}

void test_onechunk(const vector<vector<float>>& data, bool huge_page, eWalkMode walk_mode) {
  const size_t N = data.size();
  const size_t vec_size = data[0].size();
  const size_t elem_size = 8 + 4 * vec_size;

  size_t MemSize = N * elem_size;
  char* const pChunkStart = huge_page ? reinterpret_cast<char *>(mmap(NULL, MemSize, PROT_READ | PROT_WRITE,
                                        MAP_PRIVATE| MAP_ANONYMOUS , -1, 0))
                                      : new char [ MemSize   ] ;
  if (huge_page) madvise(pChunkStart, MemSize, MADV_HUGEPAGE );

  char* pChunk = pChunkStart;
  for (size_t i = 0; i < N; ++i) {
    uint32_t* pi = (uint32_t*)pChunk; 
    pi[0] = i;
    pi[1] = vec_size; 
    float*    pf = reinterpret_cast<float*>(pChunk + 8);
  
    for (size_t k = 0; k < vec_size; ++k) {
      pf[k] = data[i][k];
    }
    pChunk += elem_size;
  }

  {
    cout << "\t\t====================================" << endl;
    cout << "\t\tTEST ONE CHUNK DIRECT\tmode=" << get_walk_mode_name(walk_mode) << "\thuge_page=" << huge_page << endl; 
    cout << "\t\tN=" << N << "\t vec_size=" << vec_size << endl;
    vector<unsigned> walk_data(N);
    get_walk_data(walk_data, walk_mode);

    for (int use_prefetch = 0; use_prefetch < 2; ++ use_prefetch) {
      WallClockTimer z;
      uint64_t totalElapsed = 0;
      z.reset();
      float *pQuery = reinterpret_cast<float*>(pChunkStart + 8);
      float sum = 0;
      for (size_t i = 0; i < N; ++i) {
        if (use_prefetch && i < N-1) _mm_prefetch(pChunkStart + walk_data[i+1]*elem_size, _MM_HINT_T0);
        char *pst = pChunkStart + walk_data[i] * elem_size; 
        const uint32_t* pi = (uint32_t*) pst;
        uint32_t id = pi[0];
        size_t   qty = pi[1];
        const float* pData = reinterpret_cast<float*>(pst + 8);

        float d = dist(pQuery, pData, qty);

        sum += d + qty + id;
      }
      totalElapsed += z.split();

      cout << "\t\tuse_prefetch=" << use_prefetch << endl; 
      cout << "\t\tElapsed: " << float(totalElapsed)/1000.0 << " ms" << "\tIgnore: " << sum << endl;
    }
    cout << "\t\t====================================" << endl;
  }

  if (huge_page) {
    munmap(pChunkStart, MemSize);
  } else delete [] pChunkStart;
};

struct __attribute__((packed)) Elem1 {
  uint32_t id;
  uint32_t qty; 
  float    pData[0];
};

struct __attribute__((packed)) Elem2 {
  uint32_t id;
  uint32_t qty; 
  const float*   pData;
  
  Elem2(uint32_t id = 0, uint32_t qty = 0, const float* pData=NULL)  {
    this->id = id;
    this->qty = qty;
    this->pData = pData;
  }
};

void test_onechunk_indirect1(const vector<vector<float>>& data, bool huge_page, eWalkMode walk_mode) {
  const size_t N = data.size();
  const size_t vec_size = data[0].size();
  const size_t elem_size = 8 + 4 * vec_size;

  size_t MemSize = N * elem_size;
  char* const pChunkStart = huge_page ? reinterpret_cast<char *>(mmap(NULL, MemSize, PROT_READ | PROT_WRITE,
                                        MAP_PRIVATE| MAP_ANONYMOUS , -1, 0))
                                      : new char [ MemSize   ] ;
  if (huge_page) madvise(pChunkStart, MemSize, MADV_HUGEPAGE );

  char* pChunk = pChunkStart;
  vector<Elem1*>  vpData(N);
  for (size_t i = 0; i < N; ++i) {
    uint32_t* pi = (uint32_t*)pChunk; 
    vpData[i] = (Elem1*)pChunk;
    pi[0] = i;
    pi[1] = vec_size; 
    float*    pf = reinterpret_cast<float*>(pChunk + 8);
  
    for (size_t k = 0; k < vec_size; ++k) {
      pf[k] = data[i][k];
    }
    pChunk += elem_size;
  }
  {
    cout << "\t\t====================================" << endl;
    cout << "\t\tTEST ONE CHUNK INDIRECT ONE-LEVEL\tmode=" << get_walk_mode_name(walk_mode) << "\thuge_page=" << huge_page << endl;
    cout << "\t\tN=" << N << "\t vec_size=" << vec_size << endl;
    vector<unsigned> walk_data(N);
    get_walk_data(walk_data, walk_mode);

    for (int use_prefetch = 0; use_prefetch < 2; ++ use_prefetch) {
      WallClockTimer z;
      uint64_t totalElapsed = 0;
      z.reset();
      float *pQuery = reinterpret_cast<float*>(pChunkStart + 8);
      float sum = 0;
      for (size_t i = 0; i < N; ++i) {
        if (use_prefetch && i < N-1) _mm_prefetch(vpData[walk_data[i+1]], _MM_HINT_T0);
        const Elem1* pElem = vpData[walk_data[i]];
        float d = dist(pQuery, pElem->pData, pElem->qty);
    
        sum += d + pElem->qty + pElem->id;
      }
      totalElapsed += z.split();

      cout << "\t\tuse_prefetch=" << use_prefetch << endl; 
      cout << "\t\tElapsed: " << float(totalElapsed)/1000.0 << " ms" << "\tIgnore: " << sum << endl;
    }
    cout << "\t\t====================================" << endl;
  }

  if (huge_page) {
    munmap(pChunkStart, MemSize);
  } else delete [] pChunkStart;
};

void test_mulchunk_indirect1(const vector<vector<float>>& data, bool huge_page, eWalkMode walk_mode, size_t chunk_size) {
  const size_t N = data.size();
  const size_t vec_size = data[0].size();
  const size_t elem_size = 8 + 4 * vec_size;

  if (N % chunk_size || N < chunk_size) {
    cerr << "The number of vector elements must be a multiple of " << chunk_size << endl;
    abort();
  }

  size_t MemChunkSize = chunk_size * elem_size;
  size_t chunk_qty = (N / chunk_size);

  vector<char *> vpChunks(chunk_qty);
  for (size_t i = 0; i < chunk_qty; ++i) {
    vpChunks[i] = huge_page ? reinterpret_cast<char *>(mmap(NULL, MemChunkSize, PROT_READ | PROT_WRITE,
                                        MAP_PRIVATE| MAP_ANONYMOUS , -1, 0))
                                      : new char [ MemChunkSize ] ;
    if (huge_page) madvise(vpChunks[i], MemChunkSize, MADV_HUGEPAGE );
  }

  vector<Elem1*>  vpData(N);
  for (size_t i = 0; i < N; ++i) {
    size_t chunkId = i / chunk_size;
    size_t inChunkId = i % chunk_size;
    char* p = vpChunks[chunkId] + inChunkId * elem_size;
    uint32_t* pi = (uint32_t*)p; 
    vpData[i] = (Elem1*)p;
    pi[0] = i;
    pi[1] = vec_size; 
    float*    pf = reinterpret_cast<float*>(p + 8);
  
    for (size_t k = 0; k < vec_size; ++k) {
      pf[k] = data[i][k];
    }
  }
  {
    cout << "\t\t====================================" << endl;
    cout << "\t\tTEST MULTIPLE CHUNKS INDIRECT ONE-LEVEL\tmode=" << get_walk_mode_name(walk_mode) << "\thuge_page=" << huge_page << endl;
    cout << "\t\tN=" << N << "\t# of chunks=" << chunk_qty << "\t vec_size=" << vec_size << endl;
    vector<unsigned> walk_data(N);
    get_walk_data(walk_data, walk_mode);

    for (int use_prefetch = 0; use_prefetch < 2; ++ use_prefetch) {
      WallClockTimer z;
      uint64_t totalElapsed = 0;
      z.reset();
      float *pQuery = reinterpret_cast<float*>(vpChunks[0] + 8);
      float sum = 0;
      for (size_t i = 0; i < N; ++i) {
        if (use_prefetch && i < N-1) _mm_prefetch(vpData[walk_data[i+1]], _MM_HINT_T0);
        const Elem1* pElem = vpData[walk_data[i]];
        float d = dist(pQuery, pElem->pData, pElem->qty);
    
        sum += d + pElem->qty + pElem->id;
      }
      totalElapsed += z.split();

      cout << "\t\tuse_prefetch=" << use_prefetch << endl; 
      cout << "\t\tElapsed: " << float(totalElapsed)/1000.0 << " ms" << "\tIgnore: " << sum << endl;
    }
    cout << "\t\t====================================" << endl;
  }

  for (size_t i = 0; i < chunk_qty; ++i) {
    char *pChunkStart = vpChunks[i];
    if (huge_page) {
      munmap(pChunkStart, MemChunkSize);
    } else delete [] pChunkStart;
  }
};

void test_onechunk_indirect2(const vector<vector<float>>& data, bool huge_page, eWalkMode walk_mode) {
  const size_t N = data.size();
  const size_t vec_size = data[0].size();
  const size_t elem_size = 4 * vec_size;

  size_t MemSize = N * elem_size;
  char* const pChunkStart = huge_page ? reinterpret_cast<char *>(mmap(NULL, MemSize, PROT_READ | PROT_WRITE,
                                        MAP_PRIVATE| MAP_ANONYMOUS , -1, 0))
                                      : new char [ MemSize   ] ;
  if (huge_page) madvise(pChunkStart, MemSize, MADV_HUGEPAGE );

  char* pChunk = pChunkStart;
  vector<Elem2*>  vpData(N);

  for (size_t i = 0; i < N; ++i) {
    float*    pf = reinterpret_cast<float*>(pChunk);
    vpData[i] = new Elem2(i, vec_size, pf);
  
    for (size_t k = 0; k < vec_size; ++k) {
      pf[k] = data[i][k];
    }
    pChunk += elem_size;
  }

  {
    cout << "\t\t====================================" << endl;
    cout << "\t\tTEST ONE CHUNK INDIRECT LEVEL-TWO\tmode=" << get_walk_mode_name(walk_mode) << "\thuge_page=" << huge_page << endl;
    cout << "\t\tN=" << N << "\t vec_size=" << vec_size << endl;
    vector<unsigned> walk_data(N);
    get_walk_data(walk_data, walk_mode);

    for (int use_prefetch = 0; use_prefetch < 2; ++ use_prefetch) {
      WallClockTimer z;
      uint64_t totalElapsed = 0;
      z.reset();
      float *pQuery = reinterpret_cast<float*>(pChunkStart);
      float sum = 0;
      for (size_t i = 0; i < N; ++i) {
        if (i < N-1) {
          const Elem2* pElemNext = vpData[walk_data[i+1]];
          _mm_prefetch(pElemNext, _MM_HINT_T0);
          _mm_prefetch(pElemNext->pData, _MM_HINT_T0);
        }
        const Elem2* pElem = vpData[walk_data[i]];
        float d = dist(pQuery, pElem->pData, pElem->qty);

        sum += d + pElem->qty + pElem->id;
      }
      totalElapsed += z.split();
      cout << "\t\tprefetch=" << use_prefetch << endl; 
      cout << "\t\tElapsed: " << float(totalElapsed)/1000.0 << " ms" << "\tIgnore: " << sum << endl;
    }
    cout << "\t\t====================================" << endl;
  }

  for (size_t i = 0; i < N; ++i) {
    const Elem2* pElem = vpData[i];
    delete pElem;
  }

  if (huge_page) {
    munmap(pChunkStart, MemSize);
  } else delete [] pChunkStart;
};

void test_sepalloc_indirect1(const vector<vector<float>>& data, eWalkMode walk_mode) {
  const size_t N = data.size();
  const size_t vec_size = data[0].size();
  const size_t elem_size = 8 + 4 * vec_size;

  vector<char*>   vExtData(N);
  vector<Elem1*>  vpData(N);

  for (size_t i = 0; i < N; ++i) {
    vExtData[i] = new char[elem_size];
    uint32_t* pi = (uint32_t*)vExtData[i]; 
    vpData[i] = (Elem1*)vExtData[i];
    pi[0] = i;
    pi[1] = vec_size; 
    float*    pf = reinterpret_cast<float*>(vExtData[i] + 8);
  
    for (size_t k = 0; k < vec_size; ++k) {
      pf[k] = data[i][k];
    }
  }
  {
    cout << "\t\t====================================" << endl;
    cout << "\t\tTEST INDIV. ALLOC DATA INDIRECT LEVEL-ONE\tmode=" << get_walk_mode_name(walk_mode) << endl;
    cout << "\t\tN=" << N << "\t vec_size=" << vec_size << endl;
    vector<unsigned> walk_data(N);
    get_walk_data(walk_data, walk_mode);

    for (int use_prefetch = 0; use_prefetch < 2; ++ use_prefetch) {
      WallClockTimer z;
      uint64_t totalElapsed = 0;
      z.reset();
      float *pQuery = vpData[0]->pData;
      float sum = 0;
      for (size_t i = 0; i < N; ++i) {
        const Elem1* pElem = vpData[walk_data[i]];
        if (use_prefetch && i < N-1) _mm_prefetch(vpData[walk_data[i+1]], _MM_HINT_T0);
        float d = dist(pQuery, pElem->pData, pElem->qty);

        sum += d + pElem->qty + pElem->id;
      }
      totalElapsed += z.split();
      cout << "\t\tprefetch=" << use_prefetch << endl; 
      cout << "\t\tElapsed: " << float(totalElapsed)/1000.0 << " ms" << "\tIgnore: " << sum << endl;
    }
    cout << "\t\t====================================" << endl;
  }
  for (size_t i = 0; i < N; ++i) {
    delete [] vExtData[i];
  }
};

void test_sepalloc_indirect2(const vector<vector<float>>& data, eWalkMode walk_mode) {
  const size_t N = data.size();
  const size_t vec_size = data[0].size();

  vector<Elem2*>  vpData(N);

  for (size_t i = 0; i < N; ++i) {
    vpData[i] = new Elem2(i, vec_size, &data[i][0]);
  }

  {
    cout << "\t\t====================================" << endl;
    cout << "\t\tTEST INDIV. ALLOC INDIRECT LEVEL-TWO\tmode=" << get_walk_mode_name(walk_mode) << endl;
    cout << "\t\tN=" << N << "\t vec_size=" << vec_size << endl;
    vector<unsigned> walk_data(N);
    get_walk_data(walk_data, walk_mode);

    for (int use_prefetch = 0; use_prefetch < 2; ++ use_prefetch) {
      WallClockTimer z;
      uint64_t totalElapsed = 0;
      z.reset();
      const float *pQuery = reinterpret_cast<const float*>(&data[0][0]);
      float sum = 0;
      for (size_t i = 0; i < N; ++i) {
        if (i < N-1) {
          const Elem2* pElemNext = vpData[walk_data[i+1]];
          _mm_prefetch(pElemNext, _MM_HINT_T0);
          _mm_prefetch(pElemNext->pData, _MM_HINT_T0);
        }
        const Elem2* pElem = vpData[walk_data[i]];
        float d = dist(pQuery, pElem->pData, pElem->qty);
    
        sum += d + pElem->qty + pElem->id;
      }
      totalElapsed += z.split();
      cout << "\t\tprefetch=" << use_prefetch << endl; 
      cout << "\t\tElapsed: " << float(totalElapsed)/1000.0 << " ms" << "\tIgnore: " << sum << endl;
    }
    cout << "\t\t====================================" << endl;
  }

  for (size_t i = 0; i < N; ++i) {
    const Elem2* pElem = vpData[i];
    delete pElem;
  }

};

int main(int argc, char*argv[]) {
    for (size_t  vec_size=16; vec_size <= 128; vec_size *=2) {
      cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
      cout << "@@@ START vector size=" << vec_size << endl;
      cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
      vector<vector<float>> data;
      gen_data(TOTAL_QTY, vec_size, data);

      for (int iwalk_mode = 0; iwalk_mode != kMaxWalkMode; ++iwalk_mode) {
        eWalkMode walk_mode = static_cast<eWalkMode>(iwalk_mode);
        cout << "\t*******************************************************" << endl;
        cout << "\t*** START " << get_walk_mode_name(walk_mode) << endl;
        cout << "\t*******************************************************" << endl;

        // We will test huge pages only in the case of the gapped or random gap access
        for (int huge_page = 0; huge_page < (walk_mode == kRandomGap || walk_mode == kRandom ? 2 : 1); ++huge_page) {
          cout << "\t\t----------------------------------------------" << endl;
          test_onechunk(data, huge_page, walk_mode);
          test_onechunk_indirect1(data, huge_page, walk_mode);
          test_onechunk_indirect2(data, huge_page,  walk_mode);
          cout << "\t\t----------------------------------------------" << endl;
          for (size_t chunk_size = 1024; chunk_size <= 1024*512; chunk_size *= 8)
            test_mulchunk_indirect1(data, huge_page, walk_mode, chunk_size);
          cout << "\t\t----------------------------------------------" << endl;
        }

        test_sepalloc_indirect1(data, walk_mode);
        test_sepalloc_indirect2(data, walk_mode);

        cout << "\t*******************************************************" << endl;
        cout << "\t*** END " << get_walk_mode_name(walk_mode) << endl;
        cout << "\t*******************************************************" << endl;
        cout << endl;
      }
      cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
      cout << "@@@ END vector size=" << vec_size << endl;
      cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
      cout << endl;
    }
  return 0;
}
