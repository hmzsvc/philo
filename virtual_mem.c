#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    int *ptr = malloc(sizeof(int));
    *ptr = 42;

    printf("PID: %d\n", getpid());
    printf("Virtual address of ptr: %p\n", (void *)ptr);
    printf("Value at ptr: %d\n", *ptr);

    getchar(); // Bellek haritasını izlemek için beklet
    free(ptr);
    return 0;
}