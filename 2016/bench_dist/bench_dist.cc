#include <iostream>
#include <cstdlib>
#include <vector>
#include <random>

#include "../../2014/ztimer.h"

using namespace std;

#include <immintrin.h>
#include <smmintrin.h>

#if defined(__GNUC__)
#define PORTABLE_ALIGN16 __attribute__((aligned(16)))
#else
#define PORTABLE_ALIGN16 __declspec(align(16))
#endif

// This function computes the squared L2, but only for vectors where the number of elements is a multiple of 16 
float dist(const float* pVect1, const float* pVect2, size_t qty) {
  size_t qty16 = qty >> 4;

  const float* pEnd1 = pVect1 + (qty16 << 4);

  __m128  diff, v1, v2;
  __m128  sum = _mm_set1_ps(0);

  while (pVect1 < pEnd1) {
    _mm_prefetch((char*)(pVect2 + 16), _MM_HINT_T0);
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

void test_chunk(const vector<vector<float>>& data) {
  const size_t N = data.size();
  const size_t vec_size = data[0].size();
  const size_t elem_size = 8 + 4 * vec_size;

  char* const pChunkStart = new char [ N * elem_size  ] ;
  char* pChunk = pChunkStart;
  for (size_t i = 0; i < N; ++i) {
    uint32_t* pi = (uint32_t*)pChunk; 
    pi[0] = i;
    pi[1] = vec_size; 
    float*    pf = (float*)pChunk + 8;
  
    for (size_t k = 0; k < vec_size; ++k) {
      pf[k] = data[i][k];
    }
    pChunk += elem_size;
  }
  WallClockTimer z;
  uint64_t totalElapsed = 0;
  z.reset();
  float *pQuery = (float*)pChunkStart + 8;
  float sum = 0;
  for (size_t i = 1; i < N; ++i) {
    char *pst = pChunkStart + i * elem_size; 
    const uint32_t* pi = (uint32_t*) pst;
    uint32_t id = pi[0];
    size_t   qty = pi[1];
    const float* pData = (float*)pst + 8;

    float d = dist(pQuery, pData, qty);

    sum += d + qty + id;
  }
  totalElapsed += z.split();
  cout << "Ignore: " << sum << endl;
  cout << "Test (chunk) N=" << N << " vec_size=" << vec_size << " elapsed: " << float(totalElapsed)/1000.0 << " ms" << endl;
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

void test_chunk_indirect1(const vector<vector<float>>& data) {
  const size_t N = data.size();
  const size_t vec_size = data[0].size();
  const size_t elem_size = 8 + 4 * vec_size;

  char* const pChunkStart = new char [ N * elem_size  ] ;
  char* pChunk = pChunkStart;
  vector<Elem1*>  vpData(N);
  for (size_t i = 0; i < N; ++i) {
    uint32_t* pi = (uint32_t*)pChunk; 
    vpData[i] = (Elem1*)pChunk;
    pi[0] = i;
    pi[1] = vec_size; 
    float*    pf = (float*)pChunk + 8;
  
    for (size_t k = 0; k < vec_size; ++k) {
      pf[k] = data[i][k];
    }
    pChunk += elem_size;
  }
  WallClockTimer z;
  uint64_t totalElapsed = 0;
  z.reset();
  float *pQuery = (float*)pChunkStart + 8;
  float sum = 0;
  for (size_t i = 1; i < N; ++i) {
    const Elem1* pElem = vpData[i];
    float d = dist(pQuery, pElem->pData, pElem->qty);

    sum += d + pElem->qty + pElem->id;
  }
  totalElapsed += z.split();
  cout << "Ignore: " << sum << endl;
  cout << "Test (chunk indirect1) N=" << N << " vec_size=" << vec_size << " elapsed: " << float(totalElapsed)/1000.0 << " ms" << endl;
};

void test_origdata_indirect1(const vector<vector<float>>& data) {
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
    float*    pf = (float*)vExtData[i] + 8;
  
    for (size_t k = 0; k < vec_size; ++k) {
      pf[k] = data[i][k];
    }
  }
  WallClockTimer z;
  uint64_t totalElapsed = 0;
  z.reset();
  float *pQuery = (float*)&vExtData[0] + 8;
  float sum = 0;
  for (size_t i = 1; i < N; ++i) {
    const Elem1* pElem = vpData[i];
    float d = dist(pQuery, pElem->pData, pElem->qty);

    sum += d + pElem->qty + pElem->id;
  }
  totalElapsed += z.split();
  cout << "Ignore: " << sum << endl;
  cout << "Test (chunk indirect1) N=" << N << " vec_size=" << vec_size << " elapsed: " << float(totalElapsed)/1000.0 << " ms" << endl;
  for (size_t i = 0; i < N; ++i) {
    delete [] vExtData[i];
  }
};

void test_origdata_indirect2(const vector<vector<float>>& data) {
  const size_t N = data.size();
  const size_t vec_size = data[0].size();

  vector<Elem2*>  vpData(N);

  for (size_t i = 0; i < N; ++i) {
    vpData[i] = new Elem2(i, vec_size, &data[i][0]);
  }

  WallClockTimer z;
  uint64_t totalElapsed = 0;
  z.reset();
  const float *pQuery = reinterpret_cast<const float*>(&data[0][0]);
  float sum = 0;
  for (size_t i = 1; i < N; ++i) {
    const Elem2* pElem = vpData[i];
    float d = dist(pQuery, pElem->pData, pElem->qty);

    sum += d + pElem->qty + pElem->id;
  }
  totalElapsed += z.split();
  cout << "Ignore: " << sum << endl;
  cout << "Test (origdata indirect2) N=" << N << " vec_size=" << vec_size << " elapsed: " << float(totalElapsed)/1000.0 << " ms" << endl;

  for (size_t i = 1; i < N; ++i) {
    const Elem2* pElem = vpData[i];
    delete pElem;
  }

};

void test_chunk_indirect2(const vector<vector<float>>& data) {
  const size_t N = data.size();
  const size_t vec_size = data[0].size();
  const size_t elem_size = 8 + 4 * vec_size;

  char* const pChunkStart = new char [ N * elem_size  ] ;
  char* pChunk = pChunkStart;
  vector<Elem2*>  vpData(N);

  for (size_t i = 0; i < N; ++i) {
    uint32_t* pi = (uint32_t*)pChunk; 
    pi[0] = i;
    pi[1] = vec_size; 
    float*    pf = (float*)pChunk + 8;
    vpData[i] = new Elem2(pi[0], pi[1], pf);
  
    for (size_t k = 0; k < vec_size; ++k) {
      pf[k] = data[i][k];
    }
    pChunk += elem_size;
  }
  WallClockTimer z;
  uint64_t totalElapsed = 0;
  z.reset();
  float *pQuery = (float*)pChunkStart + 8;
  float sum = 0;
  for (size_t i = 1; i < N; ++i) {
    const Elem2* pElem = vpData[i];
    float d = dist(pQuery, pElem->pData, pElem->qty);

    sum += d + pElem->qty + pElem->id;
  }
  totalElapsed += z.split();
  cout << "Ignore: " << sum << endl;
  cout << "Test (chunk indirect2) N=" << N << " vec_size=" << vec_size << " elapsed: " << float(totalElapsed)/1000.0 << " ms" << endl;

  for (size_t i = 1; i < N; ++i) {
    const Elem2* pElem = vpData[i];
    delete pElem;
  }
};

int main(int argc, char*argv[]) {
  vector<vector<float>> data;
  gen_data(1024*1024*2, 128, data);
  test_chunk(data);
  test_chunk_indirect1(data);
  test_origdata_indirect1(data);
  test_chunk_indirect2(data);
  test_origdata_indirect2(data);
  return 0;
}
