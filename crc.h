#ifndef __CRC_H__
#define __CRC_H__
/* =============================================================================

    Copyright (c) 2008 Pieter Conradie <http://piconomix.com>
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
    
    Title:          crc.h : CRC calculator
    Author(s):      Pieter Conradie
    Creation Date:  2008-11-06

============================================================================= */

/** 
    @ingroup UTILS
    @defgroup CRC crc.h : CRC calculator

    CRC (Cyclic Redundancy Check) calculator for checksums.
    
    File(s):
    - utils/crc.h
    - utils/crc.c
    
    A CRC is an error-detecting code that is used for data integrity checks.
    
    @see http://en.wikipedia.org/wiki/Cyclic_redundancy_check
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "defines.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS _________________________________________________________ */
/// Size of CRC
typedef u16_t crc_t;
/// The generator polynomial CRC16-CCITT: x^16 + x^12 + x^5 + x^0
#define CRC_POLYNOMIAL  0x8408
/// Initial CRC value
#define CRC_INIT_VAL    0xffff
/// Magic CRC value
#define CRC_MAGIC_VAL   0xf0b8

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/** 
   Calculate the CRC over one byte.

   @param crc       The initial CRC to start the calculation with
   @param data      The value to calculate the CRC over

   @return u16_t    The resultant CRC
 */
crc_t crc_update_u8(crc_t crc, u8_t data);

/** 
   Calculate the CRC over a number of bytes.

   @param crc           The initial CRC to start the calculation with
   @param data          Pointer to the data to calculate the CRC over
   @param nr_of_bytes   The amount of bytes to calculate the CRC over

   @return crc_t        The resultant CRC over the group of bytes
 */
crc_t crc_update_data(crc_t crc, const void* data, size_t nr_of_bytes);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif // #ifndef __CRC_H__
