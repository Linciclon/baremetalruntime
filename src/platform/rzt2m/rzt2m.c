#include <rzt2m_uart.h>
#include <plat.h>

#if defined(USE_UART0)
#define RZT2M_UART_BASE 0x80001000U
#elif defined(USE_UART1)
#define RZT2M_UART_BASE 0x80001400U
#elif defined(USE_UART2)
#define RZT2M_UART_BASE 0x80001800U
#elif defined(USE_UART3)
#define RZT2M_UART_BASE 0x80001C00U
#elif defined(USE_UART4)
#define RZT2M_UART_BASE 0x80002000U
#elif defined(USE_UART5)
#define RZT2M_UART_BASE 0x81001000U
#else
#error "No supported RZ/T2M UART was selected"
#endif

#define RZT2M_GICD_ICFGR_BASE (PLAT_GICD_BASE_ADDR + 0x0C00U)

/* Reset values required by the RZ/T2M interrupt controller.  The generic GIC
 * code deliberately leaves ICFGR untouched because it is platform-specific. */
static const uint32_t rzt2m_gicd_icfgr[30] = {
    0x00000000U, 0x00000000U, 0xAAAAAAAAU, 0xAAAAA8AAU,
    0xAAAAAAAAU, 0xAAAAAAAAU, 0xAAAAAAAAU, 0xAAAAAAAAU,
    0xAAAAAAAAU, 0xAAAAAA00U, 0xAAAAAAAAU, 0xAAAAAAAAU,
    0xAAAAAAAAU, 0xAAAAAAAAU, 0xAAAAAAAAU, 0xAAAAAAAAU,
    0xAAAAAAAAU, 0x0000000AU, 0x00000000U, 0xA02A0000U,
    0x28282828U, 0x00282828U, 0xA82A0A00U, 0xA0AA8020U,
    0x00000002U, 0x00000000U, 0xAAAA0AAAU, 0xAAAAAAAAU,
    0x00AAAAAAU, 0x828A0A2AU,
};

static void rzt2m_gicd_icfgr_init(void)
{
    volatile uint32_t *icfgr =
        (volatile uint32_t *)RZT2M_GICD_ICFGR_BASE;

    for (unsigned int i = 2; i < 30; i++) {
        icfgr[i] = rzt2m_gicd_icfgr[i];
    }
}

/* Keep the MMIO pointer observable in debug builds while preserving volatile
 * accesses to the peripheral registers. */
static volatile R_SCI0_Type * volatile uart __attribute__((used)) = (volatile R_SCI0_Type *)RZT2M_UART_BASE;

void uart_init(void)
{
    rzt2m_gicd_icfgr_init();
    rzt2m_uart_init(uart);
    rzt2m_uart_enable(uart);

    return;
}

void uart_putc(char c)
{
    rzt2m_uart_putc(uart, c);
}

char uart_getchar(void)
{
    return rzt2m_uart_getc(uart);
}

void uart_enable_rxirq(void){
    rzt2m_uart_enable_rxirq(uart);
}

void uart_clear_platform(void){
    renesas_clear_uart(uart);
}
void uart_clear_rxirq(void){
    /*while(!(uart->flag & UART_FR_RXFE)) {
        volatile char c = uart->data;
    }
    uart->isr_clear = 0xffff;*/
    (void)rzt2m_uart_getc(uart);
}


