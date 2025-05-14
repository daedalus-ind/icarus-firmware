/**
 * @file    tick.h
 * @brief   Contains declarations for tick management functions.
 * 
 * @author  Mattia Gregnanin
 * @date    2025-05-14
 */
#ifndef __TICK_H
#define __TICK_H

#include "main.h"

/**
 * @brief Contains the tick count value.
 */
extern volatile uint32_t uwTick;

/**
 * @brief Get the current tick count, in ms.
 */
uint32_t Tick_GetTick(void);

/**
 * @brief Increment the tick count.
 */
void Tick_IncTick(void);

#endif