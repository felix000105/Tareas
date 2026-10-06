#include <time.h>
#include <memory>
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <immintrin.h>

using namespace std;

// COUNTER TYPE //
typedef unsigned long long bench_t;

static bench_t before;
static bench_t after;

// ASKS FOR THE TIME STAMP COUNTER (= WHAT TIME IS IT PLEASE ?)//
static inline bench_t cycles(void) {
    unsigned int hi, lo;
    __asm__ __volatile__ ("rdtsc\n\t":"=a" (lo), "=d"(hi));
    return ((bench_t) lo) | (((bench_t) hi) << 32);
}

float horner(float X, const float *coef, long size){
    float ACC=0.0f;
    for(long i=0;i<size;i++){
        ACC=(ACC+coef[i])*X;
    }
    return ACC;
}

float horner_intrinsic(float X, const float *coef, long size){
    if(size < 8 || size % 8 != 0)
        return horner(X,coef,size);

    float X2=X*X;
    float X4=X2*X2;
    float X8=X4*X4;

    __m256 X256=_mm256_set1_ps(X8);
    __m256 Y=_mm256_setzero_ps();

    long bloques=size/8;

    for(long i=0;i<bloques-1;i++){
        __m256 C=_mm256_load_ps(coef+8*i);
        Y=_mm256_add_ps(Y,C);
        Y=_mm256_mul_ps(Y,X256);
    }

    __m256 C=_mm256_load_ps(coef+8*(bloques-1));
    Y=_mm256_add_ps(Y,C);

    alignas(32) float R[8];
    _mm256_store_ps(R,Y);

    float P=0.0f;
    float potencia=X;

    for(int i=7;i>=0;i--){
        P+=R[i]*potencia;
        potencia*=X;
    }

    return P;
}

int main(){
    const long size=10000;
    float X=1.1f;
    float R;
    int num_trials=100000;

    srand(time(NULL));

    float *coeficientes;
    coeficientes=(float *)_mm_malloc(size*sizeof(float),32);

    if(coeficientes==nullptr){
        cerr<<"Error reservando memoria."<<endl;
        return 1;
    }

    for(long i=0;i<size;i++){
        coeficientes[i]=(float)(rand()%1000)/1000.0f;
        if(i<10)
            cout<<coeficientes[i]<<endl;
    }

    R=horner(X,coeficientes,size);
    cout<<"Resultado Horner escalar: "<<R<<endl;

    R=horner_intrinsic(X,coeficientes,size);
    cout<<"Resultado Horner AVX: "<<R<<endl;

    using HornerFn=float (*)(float,const float *,long);
    HornerFn volatile fn_escalar=horner;
    HornerFn volatile fn_avx=horner_intrinsic;
    volatile float sink=0.0f;

    sink=fn_escalar(X,coeficientes,size);
    sink=fn_avx(X,coeficientes,size);

    auto inicio=chrono::steady_clock::now();

    for(int j=0;j<num_trials;j++)
        sink=fn_escalar(X,coeficientes,size);

    auto fin=chrono::steady_clock::now();

    double tiempo_escalar=chrono::duration<double>(fin-inicio).count();

    inicio=chrono::steady_clock::now();

    for(int j=0;j<num_trials;j++)
        sink=fn_avx(X,coeficientes,size);

    fin=chrono::steady_clock::now();

    double tiempo_avx=chrono::duration<double>(fin-inicio).count();

    cout<<"Time taken: "<<tiempo_escalar<<endl;
    cout<<"Time taken: "<<tiempo_avx<<endl;
    cout<<"Speedup AVX: "<<tiempo_escalar/tiempo_avx<<"x"<<endl;

    (void)sink;

    _mm_free(coeficientes);

    return 0;
}
