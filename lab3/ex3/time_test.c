#include <stdio.h>
#include <stdlib.h>

int fib(int n) {
    if(n<=1) return 1;
    return fib(n-1)+fib(n-2);
}

int main(int argc, char *argv[])
{
    fib(100000);
    
    return 0;
}