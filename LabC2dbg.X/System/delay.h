/**
\fil edef group doc_driver_delay_code Delay Driver Source Code Reference
\ingroup doc_driver_delay
\brief This file contains the API to generate delays in the millisecond and microsecond ranges.
\copyright (c) 2020 Microchip Technology Inc. and its subsidiaries.
\page License
*/

#ifndef _DELAY_H
#define _DELAY_H

#include <stdint.h>

void DELAY_milliseconds(uint16_t milliseconds);
void DELAY_microseconds(uint16_t microseconds);

#endif	// _DELAY_H
