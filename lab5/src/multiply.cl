#define BLOCK_SIZE 32


__kernel void block_multiply(
    __global const int *A,
    __global const int *B,
    __global int *C,
    const int M,
    const int K,
    const int N
){
    int global_col = get_global_id(0);
    int global_row = get_global_id(1);
    int local_col = get_local_id(0);
    int local_row = get_local_id(1);
    __local int blockA[BLOCK_SIZE][BLOCK_SIZE];
    __local int blockB[BLOCK_SIZE][BLOCK_SIZE];
    int sum = 0;
    for (int k = 0; k * BLOCK_SIZE < K; k++){
        blockA[local_row][local_col] = A[global_row * K + BLOCK_SIZE * k + local_col];
        blockB[local_row][local_col] = B[(k * BLOCK_SIZE + local_row) * N + global_col];
        barrier(CLK_LOCAL_MEM_FENCE);
        for (int t = 0; t < BLOCK_SIZE; t++){
            sum += blockA[local_row][t] * blockB[t][local_col];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    C[global_row * N + global_col] = sum;
}