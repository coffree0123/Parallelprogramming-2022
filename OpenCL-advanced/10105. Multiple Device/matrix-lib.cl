/* constant */
#define MAXN 1024
#define Blk 32
/* mul */
__kernel void mul(__global int A[MAXN][MAXN], 
          __global int B[MAXN][MAXN], 
          __global int C[MAXN][MAXN], int N) 
{   
    int BSIDE = N / Blk;
    int globalRow = get_global_id(0);
    int globalCol = get_global_id(1);
    int localRow = get_local_id(0);
    int localCol = get_local_id(1);
 
    __local int ALocal[MAXN / Blk][MAXN / Blk];
    __local int BLocal[MAXN / Blk][MAXN / Blk];
    /* loop */
    int sum = 0;  
    for (int block = 0; block < Blk; block++) {
        ALocal[localRow][localCol] = 
        A[globalRow][block * BSIDE + localCol];
        BLocal[localRow][localCol] = 
        B[block * BSIDE + localRow][globalCol];
 
        barrier(CLK_LOCAL_MEM_FENCE);
 
        /* inner */
        for (int k = 0; k < BSIDE; k++)
            sum += ALocal[localRow][k] * BLocal[k][localCol];
    }
    C[globalRow][globalCol] = sum;
}
/* end */