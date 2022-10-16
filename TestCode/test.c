#define uint32_t unsigned int
typedef uint32_t pde_t;
typedef uint32_t pte_t;

#include <stdio.h>
#include <stdlib.h>

__attribute__ ((constructor)) void
before(void){
    printf("run before main()\n");
}

__attribute__ ((destructor)) void
after(void){
    printf("run after main()\n");
}

int
main (void)
{
    char *str = (char *)malloc(sizeof(char));
    *str = 'c';
    printf("%s", str);
}