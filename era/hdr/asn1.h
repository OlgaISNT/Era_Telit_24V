/**
 * @file asn1.h
 * @version 1.0.0
 * @date 17.04.2022
 * @author: DKozenkov from RMerdeev
 * @brief app description
 */

#ifndef HDR_ASN1_H_
#define HDR_ASN1_H_

#include "m2mb_types.h"
#include "msd.h"

UINT8 asn1_encode_msd ( T_msd * msd, UINT8 * buffer );
void asn1_init_variables ( void );

#endif /* HDR_ASN1_H_ */
