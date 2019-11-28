#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <atomic>
#include <thread>
#include <mutex>

// This is only for _mm_prefetch
#include <x86intrin.h>

#include "../ztimer.h"

using namespace std;

#define MANUAL_VECTORIZE

const bool usePrefetch = false;
const float  scanFrac = 0.01;
const size_t dataQty = 1000000; // 4 * vecSize * dataQty must be >> than the L3 cache size
const size_t queryQty = 10000;

int defaultRandomSeed = 0;

typedef mt19937 RandomGeneratorType;

inline RandomGeneratorType& getThreadLocalRandomGenerator() {
  static thread_local RandomGeneratorType  randomGen(defaultRandomSeed);

  return randomGen;
}

template <class Function>
inline void parallelFor(size_t start, size_t end, size_t numThreads, Function fn) {
  if (numThreads <= 0) {
    numThreads = std::thread::hardware_concurrency();
  }

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

  for (size_t i = qty16 * 16; i < qty; ++i) {
    res += v1[i] * v2[i];
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
  size_t sum = 0;

  size_t queryQty = queries.size() / vecSize;
  const float *pDataBeg = &data[0];

  const float normConst = 10.0 / (1.0 + dataLoc.size());

  // Query is accessed in the outer loop so it could be cached well
  for (size_t iq = 0; iq < queryQty; ++iq) {
    const float *pQuery = &queries[iq * vecSize];

    if (dataLoc.size() < 1) 
      continue;

    for (size_t k = 0; k < dataLoc.size() - 1; ++k) {
      const float *pData = pDataBeg + dataLoc[k] * vecSize;
      //cout << dataLoc[k] << " " << pData[0] << endl;
      if (usePrefetch) {
        const float *pDataNext = pDataBeg + dataLoc[k+1] * vecSize;
        _mm_prefetch((char *)pDataNext, _MM_HINT_T0);
      }
      sum += k + dotProd(pQuery, pData, vecSize);
    }
    sum *= normConst;
  }

  cout << "Ignore sum: " << sum << endl;
}

#if 0
void scanMultiThread(const size_t vecSize,
                     const vector<float>& queries,
                     const vector<float>& data,
                     const vector<size_t>& dataLoc) {
  std::atomic<size_t>  sum(0);

  size_t queryQty = queries.size() / vecSize;
  const float *pDataBeg = &data[0];

  const float normConst = 10.0 / (1.0 + dataLoc.size());

  // Query is accessed in the outer loop so it could be cached well
  for (size_t iq = 0; iq < queryQty; ++iq) {
    const float *pQuery = &queries[iq * vecSize];

    if (dataLoc.size() < 1) 
      continue;

    parallelFor(0, dataLoc.size() - 1, 0 /* use all threads */, 
                [&](size_t k, size_t ) {
      const float *pData = pDataBeg + dataLoc[k] * vecSize;
      sum.fetch_add(k + dotProd(pQuery, pData, vecSize));
    });
    sum = sum.fetch_add(0) * normConst;
  }

  cout << "Ignore sum: " << sum << endl;
}
#else
void scanMultiThread(const size_t vecSize,
                     const vector<float>& queries,
                     const vector<float>& data,
                     const vector<size_t>& dataLoc) {
  std::atomic<size_t>  sum(0);

  size_t queryQty = queries.size() / vecSize;
  const float *pDataBeg = &data[0];

  const float normConst = 10.0 / (1.0 + dataLoc.size());

  size_t  numThreads = std::thread::hardware_concurrency();

  // Query is accessed in the outer loop so it could be cached well
  for (size_t iq = 0; iq < queryQty; ++iq) {
    const float *pQuery = &queries[iq * vecSize];

    if (dataLoc.size() < 1) 
      continue;

    size_t chunkSize = dataLoc.size() / numThreads;

    parallelFor(0, numThreads, numThreads, 
                [&](size_t , size_t threadId) {
      size_t start = threadId * chunkSize; 
      size_t end = min(chunkSize + threadId * chunkSize, dataLoc.size()); 

      for (size_t k = start; k < end; ++k) {
        const float *pData = pDataBeg + dataLoc[k] * vecSize;
        sum.fetch_add(k + dotProd(pQuery, pData, vecSize));
      }
      
    });
    sum = sum.fetch_add(0) * normConst;
  }

  cout << "Ignore sum: " << sum << endl;
}
#endif

template <typename F>
void bench(F f, const string& testName) {
  WallClockTimer t;
  f();
  cout << "Test: " << testName << " time: " << t.split() / 1000000.0 << " sec" << endl;
}

int main(int argc, char* argv[]) {
  vector<float> data;
  vector<float> queries;
  vector<size_t> dataLoc; 
  vector<size_t> dataLocSorted; 

  if (argc != 2) {
    cout << "Usage: " << argv[0] << " <vector size, e.g., 32> " << endl;
    return 1;
  }
  size_t vecSize = atoi(argv[1]);
  cout << "Vector size: " << vecSize << endl;

  size_t scanQty = (size_t) (scanFrac * dataQty);

  genRandData(dataQty, vecSize, data);
  genRandData(queryQty, vecSize, queries);
  genRandLoc(scanQty, dataQty, dataLoc);
  dataLocSorted = dataLoc; // copy
  sort(dataLocSorted.begin(), dataLocSorted.end());

  cout << "# of data points: " << data.size()/vecSize << endl;
  cout << "# of query points: " << queries.size()/vecSize<< endl;
  cout << "# of data locations for random access: " << dataLoc.size() << endl;
  cout << "# of data locations for sequential access: " << dataLocSorted.size() << endl;

  bench([&](){
          scanMultiThread(vecSize, queries, data, dataLocSorted);
        },
        "sorted multi-threaded scan");

  bench([&](){
          scanMultiThread(vecSize, queries, data, dataLoc);
        },
        "unsorted multi-threaded scan");

  bench([&](){
          scan(vecSize, queries, data, dataLocSorted, usePrefetch);
        },
        "sorted scan");

  bench([&](){
          scan(vecSize, queries, data, dataLoc, usePrefetch);
        },
        "unsorted scan");

  return 0;
}
