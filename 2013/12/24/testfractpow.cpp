/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * The code was used in Leo's blog:
 *
 * http://searchivarius.org/blog/efficient-exponentiation-square-rooting
 *
 * (c) Leonid Boytsov, http://boytsov.info
 */

#include "cmn.h"

using namespace std;

const unsigned MaxNumDig = 8;

using namespace std;

template <class T>
inline T EfficientFractPow(T Base, T FractExp, unsigned NumDig) {
    //CHECK(FractExp <= 1 && NumDig);
    uint64_t MaxK = uint64_t(1) << NumDig;
    uint64_t Exp = static_cast<unsigned>(std::ceil(FractExp * MaxK));


    if (Exp == 0)    return 1;     // pow == 0 
    if (Exp == MaxK) return Base;  // pow == 1

    uint64_t Mask1 = MaxK - 1;
    uint64_t Mask2 = MaxK >>= 1;

    T res = 1.0;

    while (true) {
        Base = sqrt(Base);

        if (Exp & Mask2) res *= Base;

        Exp = (Exp << 1) & Mask1;

        if (!Exp) return res;
    }

    return res;
}

template <class T>
void testPow(int N, int rep, unsigned NumDig) {
    cout << "================================ " << endl;
    vector<T>   data1(N*4);
    vector<T>   data2(N*4);

    uint64_t MaxK = uint64_t(1)<<NumDig;

    for (int i = 0; i < 4*N; ++i) {
        data1[i] = 1 + (rand() % 10000) / 10.0;
        data2[i] = (rand() % MaxK) / T(MaxK);
    }

    WallClockTimer timer;
    T sum = 0;
    T fract = T(1)/N;
    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
            sum += 0.01 * pow(data1[i],   data2[i]); 
            sum += 0.01 * pow(data1[i+1], data2[i+1]); 
            sum += 0.01 * pow(data1[i+2], data2[i+2]); 
            sum += 0.01 * pow(data1[i+3], data2[i+3]); 
        }
        sum *= fract;
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Ignore: " << sum << endl;
    cout << "Pows computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of Pows per sec: " << (float(TotalQty) / t) << endl;
    
}

template <class T>
void testEfficientFractPow(int N, int rep, unsigned NumDig, bool bRootOnly) {
    cout << "================================ " << endl;
    vector<T>   data1(N*4);
    vector<T>   data2(N*4);

    uint64_t MaxK = uint64_t(1)<<NumDig;

    for (int i = 0; i < 4*N; ++i) {
        data1[i] = 1 + (rand() % 10000) / 10.0;
        data2[i] = bRootOnly ? T(1) / T(MaxK):(rand() % MaxK) / T(MaxK);
    }

    WallClockTimer timer;
    T sum = 0;
    T fract = T(1)/N;
    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
            sum += 0.01 * EfficientFractPow(data1[i],   data2[i], NumDig); 
            sum += 0.01 * EfficientFractPow(data1[i+1], data2[i+1], NumDig); 
            sum += 0.01 * EfficientFractPow(data1[i+2], data2[i+2], NumDig); 
            sum += 0.01 * EfficientFractPow(data1[i+3], data2[i+3], NumDig); 
        }
        sum *= fract;
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Ignore: " << sum << endl;
    cout << "Pows computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of efficient fract Pows (bRootOnly = "  << bRootOnly << " per sec: " << (float(TotalQty) / t) << " numDig = " << NumDig << endl;
    
}

int main() {
    SetHighAccuracy();

    for (float a = 1.1 ; a <= 2; a+= 0.1) {
        for (unsigned NumDig = 1; NumDig < MaxNumDig; ++NumDig) {
            uint64_t MaxFract = uint64_t(1) << NumDig;

            for (uint64_t intFract = 0; intFract < MaxFract; ++intFract) {
              float fract = float(intFract) / float(MaxFract);
              float v1 = pow(a, fract);
              float v2 = EfficientFractPow(a, fract, NumDig);
            
              if (v2 < 1 || fabs(v1/v2 - 1) > 1e-5) {
                  cerr << "Bug1 in the EfficientFractPow! Exponent " << NumDig  << endl;
                  cerr <<  v1 << " -> " <<  v2 << " " << fabs(v1/v2 - 1) << endl;
                  return 1;
              }
           }
        }
    }

    for (uint64_t NumDig = 1; NumDig <= MaxNumDig; ++NumDig) {
        testEfficientFractPow<float>(1000, 100, NumDig, true);
        testEfficientFractPow<float>(1000, 100, NumDig, false);
        testPow<float>(10000, 100, NumDig);
    }
}
