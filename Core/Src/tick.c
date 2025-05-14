/**
 * @file    tick.c
 * @brief   Contains definitions for tick management functions.
 * 
 * @author  Mattia Gregnanin
 * @date    2025-05-14
 */
#include "tick.h"

volatile uint32_t uwTick = 0UL;

uint32_t Tick_GetTick(void) {
    return uwTick;
}

void Tick_IncTick(void) {
    uwTick++;
}