#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <atomic>
#include <thread>
#include <mutex>
#include <cassert>

// This is only for _mm_prefetch
#include <x86intrin.h>

#include "../ztimer.h"

using namespace std;

#define MANUAL_VECTORIZE

const float mulConst = 1e-5;
const size_t numThreads = std::thread::hardware_concurrency();


int defaultRandomSeed = 0;

typedef mt19937 RandomGeneratorType;

inline RandomGeneratorType& getThreadLocalRandomGenerator() {
  static thread_local RandomGeneratorType  randomGen(defaultRandomSeed);

  return randomGen;
}

template <class Function>
inline void parallelFor(size_t start, size_t end, Function fn) {

  if (numThreads == 1) {
    for (size_t id = start; id < end; id++) {
      fn(id, 0);
    }
  } else {
    std::vector<std::thread>  threads;
    std::atomic<size_t>       current(start);

    // keep track of exceptions in threads
    // https://stackoverflow.com/a/32428427/1713196
    std::exception_ptr lastException = nullptr;
    std::mutex         lastExceptMutex;

    for (size_t threadId = 0; threadId < numThreads; ++threadId) {
      threads.push_back(std::thread([&, threadId] {
        while (true) {
          size_t id = current.fetch_add(1);

          if ((id >= end)) {
            break;
          }

          try {
            fn(id, threadId);
          } catch (...) {
            std::unique_lock<std::mutex> lastExcepLock(lastExceptMutex);
            lastException = std::current_exception();
            /* 
             * This will work even when current is the largest value that
             * size_t can fit, because fetch_add returns the previous value
             * before the increment (what will result in overflow 
             * and produce 0 instead of current + 1).
             */
            current = end;
            break;
          }
        }
      }));
    }
    for (auto & thread : threads) {
      thread.join();
    }
    if (lastException) {
      std::rethrow_exception(lastException);
    }
  }


}


// random 32-bit integer number
inline int32_t randomInt() {
    /*
     * Random number generation is thread safe when respective
     * objects are not shared among threads. So, we will keep one
     * random number generator per thread.
    */ 
    // thread_local is static by default, but let's keep it static for clarity
    static thread_local std::uniform_int_distribution<int32_t> distr(0, std::numeric_limits<int32_t>::max());
   
    return distr(getThreadLocalRandomGenerator()); 
}

template <class T>
// random real number from 0 (inclusive) to 1 (exclusive)
inline T randomReal() {
    /*
     * Random number generation is thread safe when respective
     * objects are not shared among threads. So, we will keep one
     * random number generator per thread.
    */ 
    // thread_local is static by default, but let's keep it static for clarity
    static thread_local std::uniform_real_distribution<T> distr(0, 1);

    return distr(getThreadLocalRandomGenerator()); 
}

#ifdef MANUAL_VECTORIZE
float dotProd(const float *pVect1, const float *pVect2, const size_t qty) {
  size_t qty16 = qty >> 4;

  const float *pEnd1 = pVect1 + (qty16 << 4);
  float tmpRes[8];

  __m256 v1, v2;
  __m256 sum = _mm256_set1_ps(0);

  while (pVect1 < pEnd1) {
    //_mm_prefetch((char*)(pVect2 + 16), _MM_HINT_T0);
    v1 = _mm256_loadu_ps(pVect1);
    pVect1 += 8;
    v2 = _mm256_loadu_ps(pVect2);
    pVect2 += 8;
    sum = _mm256_add_ps(sum, _mm256_mul_ps(v1, v2));

    v1 = _mm256_loadu_ps(pVect1);
    pVect1 += 8;
    v2 = _mm256_loadu_ps(pVect2);
    pVect2 += 8;
    sum = _mm256_add_ps(sum, _mm256_mul_ps(v1, v2));
  }

  _mm256_store_ps(tmpRes, sum);
  float res = tmpRes[0] + tmpRes[1] + tmpRes[2] + tmpRes[3] + tmpRes[4] + tmpRes[5] + tmpRes[6] + tmpRes[7];

  
  while (pVect1 < pEnd1) {
    res += (*pVect1) * (*pVect2);
    pVect1++;
    pVect2++;
  }

  return (res);
}
#else
float dotProd(const float *v1, const float *v2, const size_t qty) {
  float res = 0;
  for (size_t i = 0; i < qty; ++i) {
    res += v1[i] * v2[i];
  }
  return res;
}
#endif

void genRandData(size_t vecQty, size_t vecSize, vector<float>& res) {
  res.resize(vecSize * vecQty);
  for (size_t i = 0; i < res.size(); ++i) {
    res[i] = randomReal<float>();
    assert(res[i] >= 0);
  }
  cout << "Generated " << vecQty << " vectors of the size " << vecSize << endl;
}

void genRandLoc(size_t qty /* # of locations to generate */, size_t dataQty, vector<size_t>& res) {
  res.resize(qty);
  for (size_t i = 0; i < res.size(); ++i) {
    res[i] = randomInt() % dataQty;
  }
  cout << "Generated " << res.size() << " random locations with IDs <" << dataQty << endl;
}

void scan(const size_t vecSize,
          const vector<float>& queries,
          const vector<float>& data,
          const vector<size_t>& dataLoc,
          bool usePrefetch = false) {
  float sum = 0;

  size_t queryQty = queries.size() / vecSize;
  const float *pDataBeg = &data[0];

  const float normConst = mulConst / vecSize;

  // Query is accessed in the outer loop so it could be cached well
  for (size_t iq = 0; iq < queryQty; ++iq) {
    const float *pQuery = &queries[iq * vecSize];
    assert(pQuery + vecSize <= &queries[0] + queries.size());

    if (dataLoc.size() < 1) 
      continue;

    for (size_t id = 0; id < dataLoc.size() - 1; ++id) {
      const float *pData = pDataBeg + dataLoc[id] * vecSize;
      assert(pData + vecSize <= &data[0] + data.size());
      if (usePrefetch) {
        const float *pDataNext = pDataBeg + dataLoc[id+1] * vecSize;
        _mm_prefetch((char *)pDataNext, _MM_HINT_T0);
      }
      float val = normConst * (float)dotProd(pQuery, pData, vecSize);
      sum += val; 
    }
  }

  cout << "Ignore sum: " << sum << endl;
}

void scanMultiThreadMix(const size_t vecSize,
                     const vector<float>& queries,
                     const vector<float>& data,
                     const vector<size_t>& dataLoc) {
  size_t queryQty = queries.size() / vecSize;
  const float *pDataBeg = &data[0];

  vector<float> allSum(numThreads);
  const float normConst = mulConst / vecSize;

  // Query is accessed in the outer loop so it could be cached well
  for (size_t iq = 0; iq < queryQty; ++iq) {
    const float *pQuery = &queries[iq * vecSize];
    assert(pQuery + vecSize <= &queries[0] + queries.size());

    parallelFor(0, dataLoc.size(), 
                [&](size_t k, size_t threadId) {
      assert(dataLoc[k]*vecSize < data.size());
      assert(threadId < numThreads);
      const float *pData = pDataBeg + dataLoc[k] * vecSize;

      float val = normConst * (float)dotProd(pQuery, pData, vecSize);
      if (val < 0) {
        cout << "k=" << k << " tid=" << threadId << " data.size()=" << data.size() << " iq=" << iq << endl;
      }
      assert(val >= 0);
      allSum[threadId] +=  val;
    });
  }

  float sum = 0;
  for (float s : allSum) {
    sum += s;
  }

  cout << "Ignore sum: " << sum << " # of threads: " << numThreads << endl;
}

void scanMultiThreadSplitRobin(const size_t vecSize,
                     const vector<float>& queries,
                     const vector<float>& data,
                     const vector<size_t>& dataLoc, 
                     bool usePrefetch) {

  size_t queryQty = queries.size() / vecSize;
  const float *pDataBeg = &data[0];
  const float normConst = mulConst / vecSize;

  vector<float> allSum(numThreads);

  // Query is accessed in the outer loop so it could be cached well
  for (size_t iq = 0; iq < queryQty; ++iq) {
    const float *pQuery = &queries[iq * vecSize];
    assert(pQuery + vecSize <= &queries[0] + queries.size());

    if (dataLoc.size() < 1) 
      continue;

    parallelFor(0, numThreads, 
                [&](size_t k, size_t threadId) {
      assert(threadId < numThreads);

      const size_t dataQty = dataLoc.size();
      for (size_t k = 0; k < dataQty; ++k) {
        if (k % numThreads != threadId) continue;
        const float *pData = pDataBeg + dataLoc[k] * vecSize;
        assert(pData + vecSize <= &data[0] + data.size());
        if (usePrefetch) {
          const float *pDataNext = pDataBeg + dataLoc[k+1] * vecSize;
          _mm_prefetch((char *)pDataNext, _MM_HINT_T0);
        }
        float val = normConst * (float)dotProd(pQuery, pData, vecSize);
        allSum[threadId] += val;
        
      }
      
    });
  }

  float sum = 0;
  for (float s : allSum) {
    sum += s;
  }

  cout << "Ignore sum: " << sum << " # of threads: " << numThreads << endl;
}

void scanMultiThreadSplitChunk(const size_t vecSize,
                     const vector<float>& queries,
                     const vector<float>& data,
                     const vector<size_t>& dataLoc, 
                     bool usePrefetch) {

  size_t queryQty = queries.size() / vecSize;
  const float *pDataBeg = &data[0];
  const float normConst = mulConst / vecSize;

  vector<float> allSum(numThreads);

  // Query is accessed in the outer loop so it could be cached well
  for (size_t iq = 0; iq < queryQty; ++iq) {
    const float *pQuery = &queries[iq * vecSize];
    assert(pQuery + vecSize <= &queries[0] + queries.size());

    if (dataLoc.size() < 1) 
      continue;

    size_t chunkSize = dataLoc.size() / numThreads;

    parallelFor(0, numThreads, 
                [&](size_t k, size_t threadId) {
      assert(threadId < numThreads);

      size_t start = threadId * chunkSize; 
      size_t end = min(chunkSize + threadId * chunkSize, dataLoc.size()); 
      if (threadId + 1 == numThreads) {
        end = dataLoc.size();
      }

      if (start == end) {
        return;
      }

      for (size_t k = start; k < end - 1; ++k) {
        const float *pData = pDataBeg + dataLoc[k] * vecSize;
        assert(pData + vecSize <= &data[0] + data.size());
        if (usePrefetch) {
          const float *pDataNext = pDataBeg + dataLoc[k+1] * vecSize;
          _mm_prefetch((char *)pDataNext, _MM_HINT_T0);
        }
        float val = normConst * (float)dotProd(pQuery, pData, vecSize);
        allSum[threadId] += val;
        
      }
      
    });
  }

  float sum = 0;
  for (float s : allSum) {
    sum += s;
  }

  cout << "Ignore sum: " << sum << " # of threads: " << numThreads << endl;
}

template <typename F>
void bench(F f, const string& testName, float dataToRead) {
  WallClockTimer t;
  f();
  float timeSec = t.split() / 1000000.0;
  cout << "Test: " << testName << " time: " <<  timeSec << " sec Estimated throughput (GB/sec): " << ((float(dataToRead)/1e9)/timeSec)<< endl;
}

int main(int argc, char* argv[]) {
  vector<float> data;
  vector<float> queries;
  vector<size_t> dataLoc; 
  vector<size_t> dataLocSorted; 

  size_t    vecSize = 100;
  bool      usePrefetch = true;
  float     scanFrac = 0.01;
  size_t    dataQty = 1000000; // 4 * scanFrac * vecSize * dataQty should be >> than the L3 cache size
  size_t    queryQty = 5000;

  for (int i = 1; i < argc; ++i) {
    string curr(argv[i]);
    if (curr.substr(0, 2) != "--") {
      cerr << "Invalid option: " << curr << endl; 
      return 1;
    }
    curr = curr.substr(2);
    if (i + 1 == argc) {
      cerr << "No argument after option: " << curr << endl;
      return 1;
    }
    i++;
    if (curr == "vec_size") {
      vecSize = atoi(argv[i]);
    } else if (curr == "data_qty") {
      dataQty = atoi(argv[i]);
    } else if (curr == "query_qty") {
      queryQty = atoi(argv[i]);
    } else if (curr == "use_prefetch") {
      usePrefetch = atoi(argv[i]) != 0;
    } else if (curr == "scan_frac") {
      scanFrac = atof(argv[i]);
    } else {
      cerr << "Unknown option" << endl;
      return 1;
    }
  }

  assert(scanFrac > 0 && scanFrac <= 1);

  size_t scanQty = (size_t) (scanFrac * dataQty);
  cout << "Vector size: " << vecSize << endl;
  cout << "Use prefetch: " << usePrefetch <<  endl;
  cout << "Fraction scanned: " << scanFrac <<  " # of entries to be scanned: " << scanQty << endl;


  genRandData(dataQty, vecSize, data);
  genRandData(queryQty, vecSize, queries);
  genRandLoc(scanQty, dataQty, dataLoc);
  dataLocSorted = dataLoc; // copy
  sort(dataLocSorted.begin(), dataLocSorted.end());

  cout << "# of data points: " << data.size()/vecSize << endl;
  cout << "# of query points: " << queries.size()/vecSize<< endl;
  cout << "# of data locations for random access: " << dataLoc.size() << endl;
  cout << "# of data locations for sequential access: " << dataLocSorted.size() << endl;

  float dataToRead = sizeof(float) * dataLocSorted.size() * vecSize * queryQty;

  cout << "Data per thread (MBs): " << (dataToRead/numThreads/1e6) << endl;

  bench([&](){
          scanMultiThreadSplitRobin(vecSize, queries, data, dataLocSorted, usePrefetch);
        },
        "sorted round-robin split multi-threaded scan", dataToRead);

  bench([&](){
          scanMultiThreadSplitRobin(vecSize, queries, data, dataLoc, usePrefetch);
        },
        "unsorted round-robin split multi-threaded scan", dataToRead);

  bench([&](){
          scanMultiThreadSplitChunk(vecSize, queries, data, dataLocSorted, usePrefetch);
        },
        "sorted chunk split multi-threaded scan", dataToRead);

  bench([&](){
          scanMultiThreadSplitChunk(vecSize, queries, data, dataLoc, usePrefetch);
        },
        "unsorted chunk split multi-threaded scan", dataToRead);

  bench([&](){
          scanMultiThreadMix(vecSize, queries, data, dataLocSorted);
        },
        "sorted mix multi-threaded scan", dataToRead);

  bench([&](){
          scanMultiThreadMix(vecSize, queries, data, dataLoc);
        },
        "unsorted mix multi-threaded scan", dataToRead);

  bench([&](){
          scan(vecSize, queries, data, dataLocSorted, usePrefetch);
        },
        "sorted scan", dataToRead);

  bench([&](){
          scan(vecSize, queries, data, dataLoc, usePrefetch);
        },
        "unsorted scan", dataToRead);

  return 0;
}
