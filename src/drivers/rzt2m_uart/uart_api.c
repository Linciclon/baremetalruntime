#include "uart_api.h"


uint32_t get_pmr(void)
{
     uint32_t result=0;
     //asm volatile("MRC p15, 0, %0, c4, c6, 0" : : "r"(result) :  "memory");
     return result;
}

void set_pmr(uint32_t value)
{
     //asm volatile("MCR p15, 0, %0, c4, c6, 1" : : "r"(value) : "memory");
}