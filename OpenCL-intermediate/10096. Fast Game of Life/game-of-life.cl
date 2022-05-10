#define MAXN 2022
#define BLOCK 16
 
int notInBlk(int x, int y, int dx, int dy) {
  return ((x + dx) < 0 || (x + dx) >= BLOCK || (y + dy) < 0 || (y + dy) >= BLOCK);
}
 
__kernel void game(int N, __global char A[MAXN][MAXN],
                  __global char B[MAXN][MAXN]) {
    int nln = 0;
    int global_row = get_global_id(0)+1;
    int global_col = get_global_id(1)+1;
    int local_row = get_local_id(0);
    int local_col = get_local_id(1);
 
    int d[8][2] = {{-1, -1}, {-1, 0}, {0, -1}, {-1, 1},
                    {1, -1},  {0, 1},  {1, 0},  {1, 1}};
 
    __local int local_board[BLOCK][BLOCK];
 
    local_board[local_row][local_col] = A[global_row][global_col];
    barrier(CLK_LOCAL_MEM_FENCE);
    if(global_row >= (N+1) || global_col >= (N+1)) return;
 
    for (int k = 0; k < 8; k++) {
        if(notInBlk(local_row, local_col, d[k][0], d[k][1]))
            nln += A[global_row + d[k][0]][global_col + d[k][1]];
        else
            nln += local_board[local_row + d[k][0]][local_col + d[k][1]];
    }
    B[global_row][global_col] = ((local_board[local_row][local_col] == 0 && nln == 3) || (local_board[local_row][local_col] == 1 && (nln == 2 || nln == 3)));
 
}