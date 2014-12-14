#include "syscall.h"

int main() {
  
  int i, j, k;
  int testing[64][64][64];

  for (i = 0; i < 64; i++) {
    for (j = 0; j < 64; j++) {
      for (k = 0; k < 64; k++) {
        testing[i][j][k] = 1;
      }
    }
  }

  int sum = 0;
        
  for (i = 0; i < 64; i++) {
    for (j = 0; j < 64; j++) {
      for (k = 0; k < 64; k++) {
        sum += testing[i][j][k];
      }
    }
  }

  Exit(sum); //262144
}