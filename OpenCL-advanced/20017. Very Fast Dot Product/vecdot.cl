#define uint32_t unsigned int
#define GROUP_SIZE 1024
 
inline uint32_t rotate_left(uint32_t x, uint32_t n) {
    return  (x << n) | (x >> (32-n));
}
inline uint32_t encrypt(uint32_t m, uint32_t key) {
    return (rotate_left(m, key&31) + key)^key;
}
 
__kernel void vecdot(uint32_t key1, uint32_t key2, __global int*C, int N) 
{
    uint32_t sum = 0;
    int start = get_global_id(0) * GROUP_SIZE;
    for (int i = start; i < N && i < start + GROUP_SIZE; i++) {
        sum += encrypt(i, key1) * encrypt(i, key2);
    }
    C[get_global_id(0)] = sum;
}