/**
 * @file security_access.h
 * @version 1.0.0
 * @date 06.07.2022
 * @author: DKozenkov     
 * @brief app description
 */

#ifndef HDR_SECURITY_ACCESS_H_
#define HDR_SECURITY_ACCESS_H_

#include <m2mb_types.h>

typedef unsigned long DWORD;
typedef unsigned char byte;
typedef byte U08;
typedef unsigned long U32;

// Calculates KEY value from SEED value
DWORD new_SA_CalculateKey(DWORD seed, DWORD SA_mask);
DWORD SA_CalculateKey(byte parameter, DWORD seed, DWORD SA_mask);
DWORD VW_SA_CalculateKey(byte parameter, DWORD seed, DWORD SA_mask);
extern UINT16 CAN_SA_CalculateKey(UINT16 seed);

unsigned int rnd_generate(void);

unsigned int get_mask(void);

#endif /* HDR_SECURITY_ACCESS_H_ */
