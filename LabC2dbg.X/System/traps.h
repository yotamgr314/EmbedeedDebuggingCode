
#ifndef _TRAPS_H
#define _TRAPS_H

#include <stdint.h>


typedef enum 
{
    /* ----- Traps ----- */
    TRAPS_OSC_FAIL = 0, /** Oscillator Fail Trap vector */
    TRAPS_STACK_ERR = 1, /** Stack Error Trap Vector */
    TRAPS_ADDRESS_ERR = 2, /** Address Error Trap Vector */
    TRAPS_MATH_ERR = 3, /** Math Error Trap Vector */
    TRAPS_NVM_ERR = 7, /** NVM Error Trap Vector */
} TRAPS_ERROR_CODE;


void TRAPS_halt_on_error(uint16_t code);

#endif