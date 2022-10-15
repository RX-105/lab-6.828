#define uint32_t unsigned int
typedef uint32_t pde_t;
typedef uint32_t pte_t;

#include <stdio.h>

__attribute__ ((constructor)) void
before(void){
    printf("run before main()\n");
}

__attribute__ ((destructor)) void
after(void){
    printf("run after main()\n");
}

void
main (void)
{
    pde_t entry_pgdir[10] = {
        [0] = 1,
        [9] = 114514};
    pte_t entry_pgtable[10] = {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    uint32_t i = 0;
    for (; i < 10; ++i)
    {
        printf("%d: %d\n", i, entry_pgdir[i]);
    }
    uint32_t a = 0xF0000000 >> 22;
    printf("%d\n", a);
    printf("%d\n", 0x002000 | 0x001 | 0x002);
    register int b = 0xFFFFFFFF;
    printf("%d\n", b);
    printf("%d\n", -1*10+1);
}