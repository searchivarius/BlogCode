// FastMemSet.cpp : Defines the entry point for the console application.
//

/**
* This code is released under the
* Apache License Version 2.0 http://www.apache.org/licenses/.
*
* (c) Leonid Boytsov, http://boytsov.info
*/

#include <cstdint>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>

#if defined(__SSE4_2__ ) || defined(__AVX__)
#include <immintrin.h>
#include <mmintrin.h>
#include <emmintrin.h>
#else
#error "Neither SSE 4.2 nor AVX is supported!"
#endif

#include "hr_time.h"

#include "stdafx.h"

using namespace std;

void customMemSet1(uint32_t* ptr, size_t qty) {
    size_t qty4 = qty / 4;

    __m128i zero = _mm_set1_epi32(0);
    __m128i* ptr128 = reinterpret_cast<__m128i*>(ptr);
    __m128i* ptr128end = ptr128 + qty4;
    while (ptr128 < ptr128end)
        _mm_storeu_si128(ptr128++, zero);
    for (size_t i = 4 * qty4; i < qty; ++i)
        ptr[i] = 0;
}

void customMemSet2(uint32_t* ptr, size_t qty) {
    for (size_t i = 0; i < qty; ++i)
        ptr[i] = 0;
}


void test(int N, int custom, int rep) {
    vector<uint32_t>   data(N);

    for (int i = 0; i < N; ++i) {
        data[i] = rand();
    }

    CStopWatch timer;

    double total = 0;

    uint64_t sum = 0;

    for (int j = 0; j < rep; ++j) {

        timer.startTimer();

        if (0 == custom)
            memset(&data[0], 0, sizeof(data[0])*data.size());
        else if (1 == custom)
            customMemSet1(&data[0], data.size());
        else if (2 == custom)
            customMemSet2(&data[0], data.size());
        else {
            cerr << "Wrong custom flag = " << custom << endl;
            abort();
        }

        timer.stopTimer();
        total += timer.getElapsedTime();

        for (int i = 0; i < N; i++) {
            sum += data[i];
            data[i] = rand();
        }
    }

    cout << "Ignore: " << sum << endl;
    cout << "Custom flag = " << custom;
    cout << " total zeroed: " << rep * N << ", time " << total * 1e3 << " ms" << " Ints per sec: " << (rep * N / total) << endl;
}


int _tmain(int argc, _TCHAR* argv[])
{
   for (int custom = 0; custom < 3; ++custom) {
     cout << "========= Custom: " << custom << "======================" << endl;
     test(1024 * 1024, custom, 8);
     test(256 * 1024, custom, 32);
     test(64 * 1024, custom, 128);
     test(16 * 1024, custom, 512);
     test(4 * 1024, custom, 2048);
     cout << "================================================" << endl;
   }

	return 0;
}

