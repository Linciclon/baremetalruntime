/**
 * SPDX-License-Identifier: GPL-2.0
 * Copyright (c) Bao Project and Contributors. All rights reserved.
 */

#ifndef UART_RENESAS_H
#define UART_RENESAS_H

#include <stdint.h>
#include <stdbool.h>
#include "uart_api.h"
#include "renesas.h"

#include "bsp_clk.h"
#include "bsp_ioport_api.h"


typedef R_SCI0_Type bao_uart_t;


#define serial_puts(str_buffer) rzt2m_uart_puts(1,str_buffer) 
#define serial_getc(str_buffer) rzt2m_uart_getc(1) 


void renesas_uart_init (volatile R_SCI0_Type * const p_api_ctrl, uart_cfg_t const * const p_cfg);

void rzt2m_uart_init(volatile R_SCI0_Type * const p_api_ctrl);

void uart_start(volatile  R_SCI0_Type * const p_api_ctrl, uart_cfg_t const * const p_cfg);

void uart_read(volatile R_SCI0_Type * const p_api_ctrl, uint32_t * const p_dest, uint32_t const bytes);
void uart_write(volatile R_SCI0_Type * const p_api_ctrl, uint8_t const * const p_src);

void rzt2m_uart_enable_rxirq(volatile R_SCI0_Type * const p_api_ctrl);

void rzt2m_uart_putc(volatile R_SCI0_Type * ptr_uart,uint8_t c);
void rzt2m_uart_puts(volatile R_SCI0_Type * ptr_uart,const char *s);

uint32_t rzt2m_uart_getc(volatile R_SCI0_Type * p_uart);




void rzt2m_uart_disable(volatile R_SCI0_Type * const p_api_ctrl);
void rzt2m_uart_enable(volatile R_SCI0_Type * const p_api_ctrl);


void uart_set_baud_rate(R_SCI0_Type * const p_api_ctrl, uint32_t baud_rate);
void renesas_set_baud_rate(volatile  R_SCI0_Type * const p_api_ctrl, void const * const p_baud_setting);

void renesas_clear_uart(volatile  R_SCI0_Type * const p_api_ctrl);


#endif
