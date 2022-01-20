/**
 * Copyright (C) 2013, 2014 Johannes Taelman
 *
 * This file is part of Axoloti.
 *
 * Axoloti is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Axoloti is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Axoloti. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __AXOLOTI_DEFINES_H
#define __AXOLOTI_DEFINES_H

#include <stdint.h>

#define PI_F 3.1415927f
#define SAMPLERATE 48000
#define BUFSIZE 16
#define BUFSIZE_POW 4
typedef int32_t int32buffer[BUFSIZE];

//#define BOARD_STM32F4DISCOVERY 1
//#define BOARD_AXOLOTI_V03 1
#define BOARD_AXOLOTI_V05 1

/* BOARD_AXOLOTI_V05 + one of these below for vongon board */
//#define BOARD_STM32F429DISC 1
//#define BOARD_SELECTOR 1
#define BOARD_AXO 1 //ultrasheer

// firmware version 1.0.0.1
#define FWVERSION1 1
#define FWVERSION2 0
#define FWVERSION3 0
#define FWVERSION4 1

#if (BOARD_STM32F4DISCOVERY)
#elif (BOARD_AXOLOTI_V03)
#elif (BOARD_AXOLOTI_V05)
#elif (BOARD_STM32F429DISC && !BOARD_AXOLOTI_V05)
#error BOARD_STM32F429DISC needs BOARD_AXOLOTI_V05 enabled as well!
#elif (BOARD_SELECTOR && !BOARD_AXOLOTI_V05)
#error BOARD_SELECTOR needs BOARD_AXOLOTI_V05 enabled as well!
#elif (BOARD_AXO && !BOARD_AXOLOTI_V05)
#error BOARD_AXO needs BOARD_AXOLOTI_V05 enabled as well!
#else
#error Must define board!
#endif

#endif
