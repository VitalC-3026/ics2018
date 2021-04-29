#include <stdio.h>
#include <stdlib.h>
#include "trap.h"

static unsigned int FULL;

static unsigned int dfs(unsigned int row, unsigned int ld, unsigned int rd) {
  if (row == FULL) {
    return 1;
  } else {
    unsigned int pos = FULL & (~(row | ld | rd)), ans = 0;
    while (pos) {
      unsigned int p = (pos & (~pos + 1));
      pos -= p;
      ans += dfs(row | p, (ld | p) << 1, (rd | p) >> 1);
    }
    return ans;
  }
}

static unsigned int ans;

// void bench_queen_prepare() {
//   ans = 0;
//   FULL = (1 << setting->size) - 1;
// }

// void bench_queen_run() {
//   ans = dfs(0, 0, 0);
// }

// int bench_queen_validate() {
//   return ans == setting->checksum;
// }

int main() {
    int res[] = {92};
    int size[] = {8};
    for (int i = 0; i < 2; i++) {
        FULL = (1 << size[i]) - 1;
        ans = 0;
        ans = dfs(0, 0, 0);
        nemu_assert(ans != res[i]);
    }
} 