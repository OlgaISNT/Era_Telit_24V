/**
 * @file auxiliary.h
 * @version 1.0.0
 * @date 05.05.2022
 * @author: DKozenkov     
 * @brief app description
 */

#ifndef HDR_AUXILIARY_H_
#define HDR_AUXILIARY_H_

#include "m2mb_types.h"

UINT16 auxiliary_hex_to_char ( UINT8 *buffer_in, UINT8 num, char *buffer_out, UINT32 b_out_size );
UINT8 auxiliary_char_to_hex ( char *buffer_in, UINT16 num, UINT8 *buffer_out );
UINT8 auxiliary_write_u8 ( UINT8 *buff, UINT8 ind, UINT8 value );
UINT8 auxiliary_write_u16 ( UINT8 *buff, UINT8 ind, UINT16 value );
UINT8 auxiliary_write_u32 ( UINT8 *buff, UINT8 ind, UINT32 value );
UINT8 auxiliary_read_u8 ( UINT8 *buff, UINT8 *ind );
UINT16 auxiliary_read_u16 ( UINT8 *buff, UINT8 *ind );
UINT32 auxiliary_read_u32 ( UINT8 *buff, UINT8 *ind );
UINT8 auxiliary_crc8 ( UINT8 *pcBlock, UINT8 len );
UINT16 auxiliary_crc16 ( UINT8 *pcBlock, UINT8 len );
UINT8 auxiliary_swap ( UINT8 val );

#endif /* HDR_AUXILIARY_H_ */
