/* ============================================================
 * Computes C = A * B
 *   A : M x K  (row-major)
 *   B : K x N  (row-major)
 *   C : M x N  (row-major)
 * ============================================================ */
#include <riscv_vector.h>
#include <stddef.h>
#define VL(n) __riscv_vsetvl_e32m4(n)
#define ZERO(n) __riscv_vfmv_v_f_f32m4(0.0f,n)
#define LOAD(n,vl) __riscv_vle32_v_f32m4(n,vl)
#define STORE(p,n,vl) __riscv_vse32_v_f32m4(p,n,vl)
#define FAM(p,a,b,vl) __riscv_vfmacc_vf_f32m4(p,a,b,vl)
typedef vfloat32m4_t v_t;

void matmul(float *A, float *B, float *C, int M, int K, int N) {
    int x=0;
    for(;x+3<M;x+=4){
        float *a0=A+x*K,*a1=a0+K,*a2 = a1 + K, *a3 = a2 + K;
        float *c0=C+x*N,*c1 = c0 + N, *c2 = c1 + N, *c3 = c2 + N;
        for(int y=0;y<N;){
            size_t vl=VL(N-y);
            v_t b,v0=ZERO(vl), v1 = ZERO(vl), v2 = ZERO(vl), v3 = ZERO(vl);
            for(int k=0;k<K;k++){
                b=LOAD(B+k*N+y,vl);
                v0=FAM(v0,a0[k],b,vl), v1 = FAM(v1, a1[k], b, vl);
                v2 = FAM(v2, a2[k], b, vl), v3 = FAM(v3, a3[k], b, vl);
            }
            STORE(c0+y,v0,vl), STORE(c1 + y, v1, vl);
            STORE(c2 + y, v2, vl), STORE(c3 + y, v3, vl);
            y+=vl;
        }
    }
    for(;x<M;x++){
        float *a0=A+x*K;
        float *c0=C+x*N;
        for(int y=0;y<N;){
            size_t vl=VL(N-y);
            v_t b,v0=ZERO(vl);
            for(int k=0;k<K;k++){
                b=LOAD(B+k*N+y,vl);
                v0=FAM(v0,a0[k],b,vl);
            }
            STORE(c0+y,v0,vl);
            y+=vl;
        }
        
    }
}
