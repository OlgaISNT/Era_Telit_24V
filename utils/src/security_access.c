/**
 * @file security_access.c
 * @version 1.0.0
 * @date 06.07.2022
 * @author: DKozenkov     
 * @brief

// @Function      SA_CalculateKey()
//--------------------------------------------------------------------------------------------------
// @Description   Calculates KEY value from SEED value.
//--------------------------------------------------------------------------------------------------
// @Notes         This function is processor-independent.
//--------------------------------------------------------------------------------------------------
// @ReturnValue   KEY value.
//--------------------------------------------------------------------------------------------------
// @Parameters    parameter - sent by tester in request;
//                seed - sent by ECU in response.
 * Пример использования:
  byte par = 55; // значение, получаемое извне БЭГа от тестера
  DWORD seed = (DWORD)rnd_generate(); // случайное число, генерируемое в БЭГе и отправляемое в тестер
  LOG_INFO("seed:%lu", seed);
  DWORD key = SA_CalculateKey(par, seed, SA_MASK); // SA_MASK - индивидуальна для каждого MODEL_ID
  LOG_INFO("key:%lu", key); // значение, расчитанное внутри БЭГа и внутри тестера и сравниваемое с полученным от тестера
*/

/* Include files ================================================================================*/
#include "security_access.h"
#include <time.h>
#include <stdlib.h>
#include "log.h"
#include "app_cfg.h"
#include "factory_param.h"
/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static const byte  SA_numAdditionalLoops = 35U;
static const DWORD SA_highestBit = 0x80000000U;

static const UINT16 TOPBIT = 0x8000;
static const UINT16 POLYNOM_1 = 0x8408;
static const UINT16 POLYNOM_2 = 0x8025;
static const UINT16 BITMASK = 0x0080;
static const UINT16 INITIAL_REMINDER = 0xFFFE;
static const UINT16 MSG_LEN = 2; /* seed length in bytes */
/* Local function prototypes ====================================================================*/
/* Static functions =============================================================================*/
/* Global functions =============================================================================*/

DWORD SA_CalculateKey(byte parameter, DWORD seed, DWORD SA_mask) {
    byte i = 0U;
    DWORD key = 0U;

    // The number of loops is (parameter + SA_numAdditionalLoops) with a maximum of 255
    parameter += SA_numAdditionalLoops;

    // Test if there's an overrun
    if (parameter < SA_numAdditionalLoops) {
        // Yes -> maximum value
        parameter = 255U;
    } else {
        // No
        //DoNothing();
    }

    // Calculate the key
    for (i = 0U; i < parameter; i++) {
        // Is the highest bit set ?
        if (seed & SA_highestBit) {
            // Yes
            // Shift the temporary value from 1 to the left and do an XOR with the selected mask
            seed = (seed << 1U) ^ SA_mask;
        } else {
            // No
            // Only shifts the temporary value from 1 to the left
            seed <<= 1U;
        }
    }

    key = seed;

    return key;
}

unsigned int rnd_generate(void) {
    struct timespec ns_time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ns_time);
    srand(ns_time.tv_nsec);
    int r = rand();
    return (unsigned int) r;
}

unsigned int get_mask(void) {
    switch (get_model_id()) {
        case 0:
        default: return SA_MASK_0;
        case 1: return SA_MASK_1;
        case 2: return SA_MASK_2;
        case 3:
        case 9:	return SA_MASK_3; // Для УАЗа маска прежняя, как для MODEL_ID по предложению А.Удельнова ("они подготовили свое ПО EOL и изменения крайне нежелательны")
        case 4: return SA_MASK_4;
        case 5: return SA_MASK_5;
        case 6: return SA_MASK_6;
        case 7: return SA_MASK_7;
        case 8: return SA_MASK_8;
        case 10:return SA_MASK_10;
    }
}

extern UINT16 CAN_SA_CalculateKey(UINT16 seed) {
    UINT8 bSeed[2];
    UINT16 remainder;
    UINT8 n;
    UINT8 i;
    bSeed[0] = (UINT8)(seed >> 8); /* MSB */
    bSeed[1] = (UINT8) seed; /* LSB */
    remainder = INITIAL_REMINDER;

    for (n = 0; n < MSG_LEN; n++) {
        /* Bring the next byte into the remainder. */
        remainder ^= ((bSeed[n]) << 8);
        /* Perform modulo-2 division, a bit at a time. */
        for (i = 0; i < 8; i++) {
            /* Try to divide the current data bit. */
            if (remainder & TOPBIT) {
                if (remainder & BITMASK) {
                    remainder = (remainder << 1) ^ POLYNOM_1;
                } else {
                    remainder = (remainder << 1) ^ POLYNOM_2;
                }
            } else {
                remainder = (remainder << 1);
            }
        }
    }
    /* The final remainder is the key */
    return remainder;
}


//DWORD new_SA_CalculateKey (DWORD seed, DWORD SA_mask) {
//	DWORD key = 0;
//	if (seed !=0 ) {
//		for (unsigned char i=0; i<35; i++) {
//			if (seed & 0x80000000) {
//				seed = seed << 1;
//				seed = seed ^ SA_mask;
//			} else {
//				seed = seed << 1;
//			}
//		}
//	}
//
//	key = seed;
//
//	return key;
//}

//DWORD VW_SA_CalculateKey(byte rnd, DWORD seed, DWORD SA_mask) {
//	U08 i;
//	U32 key=seed;
//	U32 Mask01=0x2DBA91A9;
//	if  (rnd<(255-35) ) rnd+=35; else rnd=255;
//	for (i=1; i<=rnd; i++)
//	{
//		if ((key&0x80000000)!=0) key=(key<<1)^Mask01; else key<<=1;
//	}
//	return key;
//}

