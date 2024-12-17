#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main(){
    for (int i = 0; i < 10; i++) {
        printf("[");
        for (int j = 0; j < i; j++) printf("#");
        for (int j = i; j < 9; j++) printf("-");
        printf("]\r");
        fflush(stdout);
        sleep(1);
    }
    printf("\n");
    return 0;
}
