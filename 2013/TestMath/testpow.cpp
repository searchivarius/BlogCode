/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Leonid Boytsov, http://boytsov.info
 */

#include "cmn.h"

using namespace std;

const unsigned MaxPow = 32;

template <class T>
T PowOptimPosExp0(T Base, unsigned Exp) {
    if (!Exp) return 1;
    T res = Base;

    --Exp;

    while (Exp) {
        if (Exp & 1) res *= Base;

        Base *= Base;
        Exp >>= 1;

    };

    return res;
};


template <class T>
T PowOptimPosExp1(T Base, unsigned Exp) {
    if (Exp == 0) return 1;
    if (Exp == 1) return Base; 
    if (Exp == 2) return Base * Base; 
    if (Exp == 3) return Base * Base * Base; 
    if (Exp == 4) {
        Base *= Base;
        return Base * Base;
    }

    T res = Base;

    if (Exp == 5) {
        Base *= Base;
        return res * Base * Base;
    }

    if (Exp == 6) {
        Base *= Base;
        res = Base;
        Base *= Base;
        return res * Base;
    }

    if (Exp == 7) {
        Base *= Base;
        res *= Base;
        Base *= Base;
        return res * Base;
    }

    if (Exp == 8) {
        Base *= Base;
        Base *= Base;
        Base *= Base;
        return Base;
    }

    if (Exp == 9) {
        Base *= Base;
        Base *= Base;
        Base *= Base;
        return res * Base;
    }

    if (Exp == 10) {
        Base *= Base;
        res = Base;
        Base *= Base;
        Base *= Base;
        return res * Base;
    }

    if (Exp == 11) {
        Base *= Base;
        res  *= Base;
        Base *= Base;
        Base *= Base;
        return res * Base;
    }

    if (Exp == 12) {
        Base *= Base;
        Base *= Base;
        res = Base;
        Base *= Base;
        return res * Base;
    }

    if (Exp == 13) {
        Base *= Base;
        Base *= Base;
        res *= Base;
        Base *= Base;
        return res * Base;
    }

    if (Exp == 14) {
        Base *= Base;
        res = Base;
        Base *= Base;
        res *= Base;
        Base *= Base;
        return res * Base;
    }

    if (Exp == 15) {
        Base *= Base;
        res *= Base;
        Base *= Base;
        res *= Base;
        Base *= Base;
        return res * Base;
    }

    res *= res;
    res *= res;
    res *= res;
    res *= res;

    if (Exp == 16) {
        return res;
    }
#if 1
    Exp -= 16;

    while (true) {
        if (Exp & 1) res *= Base;
        Exp >>= 1;
        if (Exp) {
            Base *= Base;
        } else {
            return res;
        }
    }
#else
    if (Exp % 2) {
        Exp -= 9;
        res *= Base;
    } else {
        Exp -= 8;
    }

    while (Exp) {
        if (Exp & 1) res *= Base;
        //res *= (1 + (Base - 1)*(Exp & 1));
        Base *= Base;
        Exp >>= 1;

        if (Exp & 1) res *= Base;
        //res *= (1 + (Base - 1)*(Exp & 1));
        Base *= Base;
        Exp >>= 1;

    }
#endif

    return res;
};

template <class T>
T PowOptimPosExp2(T Base, unsigned Exp) {
    T Base2 = Base * Base;
    T Base4 = Base2 * Base2;
    T Base8 = Base4 * Base4;
    T res   = 1;

label:
        if (Exp == 0) return res;
        if (Exp == 1) return res * Base; 
        if (Exp == 2) return res * Base2;
        if (Exp == 3) return res * Base2 * Base;
        if (Exp == 4) return res * Base4;
        if (Exp == 5) return res * Base4 * Base;
        if (Exp == 6) return res * Base4 * Base2;
        if (Exp == 7) return res * Base4 * Base2 * Base;

        res *= Base8;
        Exp -= 8;
goto label;

    return 0.0;
};

using namespace std;

template <class T>
void testPow(int N, int rep) {
    cout << "================================ " << endl;
    vector<T>   data1(N*4);
    vector<T>   data2(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data1[i] = 1 + (rand() % 10000) / 10.0;
        data2[i] = 1 + (rand() % 10000) / 10.0;
    }

    WallClockTimer timer;
    T sum = 0;
    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
            sum += pow(data1[i],   data2[i]); 
            sum += pow(data1[i+1], data2[i+1]); 
            sum += pow(data1[i+2], data2[i+2]); 
            sum += pow(data1[i+3], data2[i+3]); 
        }
        sum /= N*4;
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Ignore: " << sum << endl;
    cout << "Pows computed: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of Pows per sec: " << (float(TotalQty) / t) << endl;
    
}

template <class T>
void testIntPowOptim0(int IntExp, int N, int rep) {
    cout << "================================ " << endl;
    vector<T>   data(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data[i] = 1 + (rand() % 10000) / 1000.0;
    }

    WallClockTimer timer;
    T sum = 0;
    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
            sum += PowOptimPosExp0(data[i],   IntExp); 
            sum += PowOptimPosExp0(data[i+1], IntExp); 
            sum += PowOptimPosExp0(data[i+2], IntExp); 
            sum += PowOptimPosExp0(data[i+3], IntExp); 
        }
        sum /= N*4;
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Ignore: " << sum << endl;
    cout << "Pows (optimized0) computed, degree: " << IntExp << " TotalQty: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of integer (optimized0) Pows per sec: " << (float(TotalQty) / t) << endl;
    
}

template <class T>
void testIntPowOptim1(int IntExp, int N, int rep) {
    cout << "================================ " << endl;
    vector<T>   data(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data[i] = 1 + (rand() % 10000) / 1000.0;
    }

    WallClockTimer timer;
    T sum = 0;
    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
            sum += PowOptimPosExp1(data[i],   IntExp); 
            sum += PowOptimPosExp1(data[i+1], IntExp); 
            sum += PowOptimPosExp1(data[i+2], IntExp); 
            sum += PowOptimPosExp1(data[i+3], IntExp); 
        }
        sum /= N*4;
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Ignore: " << sum << endl;
    cout << "Pows (optimized1) computed, degree: " << IntExp << " TotalQty: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of integer (optimized1) Pows per sec: " << (float(TotalQty) / t) << endl;
    
}

template <class T>
void testIntPowOptim2(int IntExp, int N, int rep) {
    cout << "================================ " << endl;
    vector<T>   data(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data[i] = 1 + (rand() % 10000) / 1000.0;
    }

    WallClockTimer timer;
    T sum = 0;
    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
            sum += PowOptimPosExp2(data[i],   IntExp); 
            sum += PowOptimPosExp2(data[i+1], IntExp); 
            sum += PowOptimPosExp2(data[i+2], IntExp); 
            sum += PowOptimPosExp2(data[i+3], IntExp); 
        }
        sum /= N*4;
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Ignore: " << sum << endl;
    cout << "Pows (optimized2) computed, degree: " << IntExp << " TotalQty: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of integer (optimized2) Pows per sec: " << (float(TotalQty) / t) << endl;
    
}

template <class T>
void testIntPow(int IntExp, int N, int rep) {
    cout << "================================ " << endl;
    vector<T>   data(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data[i] = 1 + (rand() % 10000) / 1000.0;
    }

    WallClockTimer timer;
    T sum = 0;
    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
            sum += pow(data[i],   IntExp); 
            sum += pow(data[i+1], IntExp); 
            sum += pow(data[i+2], IntExp); 
            sum += pow(data[i+3], IntExp); 
        }
        sum /= N*4;
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Ignore: " << sum << endl;
    cout << "Pows computed, degree: " << IntExp << " TotalQty: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of integer Pows per sec: " << (float(TotalQty) / t) << endl;
    
}

template <class T>
void testIntPowExplicitTemplate(int IntExp, int N, int rep) {
    cout << "================================ " << endl;
    vector<T>   data(N*4);

    for (int i = 0; i < 4*N; ++i) {
        data[i] = 1 + (rand() % 10000) / 1000.0;
    }

    WallClockTimer timer;
    T sum = 0;
    for (int j = 0; j < rep; ++j) {
        for (int i = 0; i < N*4; i+=4) {
            sum += pow(data[i],   (unsigned)IntExp); 
            sum += pow(data[i+1], (unsigned)IntExp); 
            sum += pow(data[i+2], (unsigned)IntExp); 
            sum += pow(data[i+3], (unsigned)IntExp); 
        }
        sum /= N*4;
    }
    timer.split();
    uint64_t t = timer.elapsed();
    uint64_t TotalQty = rep * N * 4;
    cout << "Ignore: " << sum << endl;
    cout << "Pows (expl arguments) computed, degree: " << IntExp << " TotalQty: " << TotalQty << ", time " <<  t / 1e3 << " ms, type: " << typeid(T).name() << endl;
    cout << "Milllions of integer Pows (expl arguments) per sec: " << (float(TotalQty) / t) << endl;
    
}

int main() {
    SetHighAccuracy();

    for (float a = 1.1 ; a <= 2; a+= 0.1) {
        for (int i = 0; i < MaxPow; ++i) {
            float v1 = pow(a, i);
            float v2 = PowOptimPosExp0(a, i);
            float v3 = PowOptimPosExp1(a, i);
            float v4 = PowOptimPosExp2(a, i);
    
            
            if (v2 < 1 || fabs(v1/v2 - 1) > 1e-5) {
                cerr << "Bug0 in the PowOptimPosExp0! Exponent " << i  << endl;
                cerr <<  v1 << " -> " <<  v2 << " " << fabs(v1/v2 - 1) << endl;
                return 1;
            }
            if (v3 < 1 || fabs(v1/v3 - 1) > 1e-5) {
                cerr << "Bug1 in the PowOptimPosExp1! Exponent " << i  << endl;
                cerr <<  v1 << " -> " <<  v3 << " " << fabs(v1/v3 - 1) << endl;
                return 1;
            }
            if (v4 < 1 || fabs(v1/v4 - 1) > 1e-5) {
                cerr << "Bug2 in the PowOptimPosExp2! Exponent " << i  << endl;
                cerr <<  v1 << " -> " <<  v4 << " " << fabs(v1/v4 - 1) << endl;
                return 1;
            }
   
        }
    }
    cout << "=== Testing generic (not necessarily integer exponents) ===" << endl;
    testPow<float>(10000, 200);
    testPow<double>(10000, 200);
    testPow<long double>(10000, 200);

    cout << "=== float base, integer exponent ====" << endl;

    for (int i = 0; i < MaxPow; ++i) {
        testIntPow<float>(i, 10000, 200);
    }
    for (int i = 0; i < MaxPow; ++i) {
        testIntPowExplicitTemplate<float>(i, 10000, 200);
    }
    for (int i = 0; i < MaxPow; ++i) {
        testIntPowOptim0<float>(i, 10000, 200);
    }
    for (int i = 0; i < MaxPow; ++i) {
        testIntPowOptim1<float>(i, 10000, 200);
    }
    for (int i = 0; i < MaxPow; ++i) {
        testIntPowOptim2<float>(i, 10000, 200);
    }
#if 0
    cout << "=== double base, integer exponent ====" << endl;

    for (int i = 0; i < MaxPow; ++i) {
        testIntPow<double>(i, 10000, 200);
    }
    for (int i = 0; i < MaxPow; ++i) {
        testIntPowExplicitTemplate<double>(i, 10000, 200);
    }
    for (int i = 0; i < MaxPow; ++i) {
        testIntPowOptim0<double>(i, 10000, 200);
    }
    for (int i = 0; i < MaxPow; ++i) {
        testIntPowOptim1<double>(i, 10000, 200);
    }
    for (int i = 0; i < MaxPow; ++i) {
        testIntPowOptim2<double>(i, 10000, 200);
    }


    cout << "=== long double base, integer exponent ====" << endl;

    for (int i = 0; i < MaxPow; ++i) {
        testIntPow<long double>(i, 10000, 200);
    }
    for (int i = 0; i < MaxPow; ++i) {
        testIntPowExplicitTemplate<long double>(i, 10000, 200);
    }
    for (int i = 0; i < MaxPow; ++i) {
        testIntPowOptim0<long double>(i, 10000, 200);
    }
    for (int i = 0; i < MaxPow; ++i) {
        testIntPowOptim1<long double>(i, 10000, 200);
    }
    for (int i = 0; i < MaxPow; ++i) {
        testIntPowOptim2<long double>(i, 10000, 200);
    }
#endif
}
