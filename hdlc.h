#ifndef __HDLC_H__
#define __HDLC_H__
/* =============================================================================

    Copyright (c) 2006 Pieter Conradie <http://piconomix.com>
 
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

    Title:          hdlc.h : HDLC encapsulation layer
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31

============================================================================= */
/** 
    @ingroup COMMS
    @defgroup HDLC hdlc.h : HDLC encapsulation layer

    This component encapsulates packets in HDLC-like frames.

    File(s):
    - comms/hdlc.h
    - comms/hdlc.c

    The purpose of this module is detect the start and end of a data packet and
    if any errors occured during transmission.

    This is accomplished by appending a 16-bit CRC to the data and by adding a
    unique start and end marker to the packet (byte 0x7E or ASCII character '~').
    This allows a receiver to detect the start of a new packet, without using a
    timeout mechanism (no timers). This is very useful when communicating with
    a non real-time operating system, e.g. Windows.

    To make sure that 0x7E only occurs to mark the valid start and end of a 
    packet, any 0x7E bytes in the data is replaced with a [0x7D, 0x5E] sequence
    (known as "escaping"). 0x5E = 0x7E xor 0x20. Any data byte 0x7D must also 
    be "escaped", meaning it must be replaced with a [0x7D, 0x5D] sequence. 
    0x5D = 0x7D xor 0x20.

    For example, to transmit the following data:
    
        [0x01] [0x02] [0x7e] [0x03]
    
    The following packet will be generated:
    
        [0x7e] [0x01] [0x02] [0x7d] [0x5e] [0x03] [CRC-LO] [CRC-HI] [0x7e]

    In general any data byte can be escaped by replacing it with 0x7D and the 
    original data byte xor'd with 0x20 (toggling bit 5). This is useful if 
    software flow control is used with XON and XOFF characters and these 
    characters must not occur accidently in the data.

    @warn_s
    The overhead with escaping may mean that a packet may be up to 
    double in size if all of the data bytes is 0x7D or 0x7E, but the 
    probability of this sequency occuring is low.
    @warn_e

    Linking dependency to the higher communication layer is avoided by 
    passing a pointer to the function hdlc_init(). The pointer function 
    call overhead can be avoided by replacing a direct call to the 
    function handler if it is known at compile time. This means that 
    hdlc.c must be modifed to avoid a small processing overhead.

    Reference:
    - [RFC 1662 "PPP in HDLC-like Framing"] (http://tools.ietf.org/html/rfc1662)
    
    Example:
 
    @include comms/test/hdlc_test.c
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stddef.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "defines.h"

#ifdef __cplusplus
extern "C" {
#endif
/* _____DEFINITIONS _________________________________________________________ */
#ifndef HDLC_MRU
/// Receive Packet size (Maximum Receive Unit)
#define HDLC_MRU    64
#endif

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Definition for a pointer to a function that will be called to send a byte
typedef void (*hdlc_tx_u8_fn_t)(u8_t data);

/**
    Definition for a pointer to a function that will be called once a frame has
    been received.
 */
typedef void (*hdlc_on_rx_frame_fn_t)(const u8_t * data, size_t nr_of_bytes);

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
    Initialise HDLC encapsulation layer.

   @param[in] tx_u8_fn          Pointer to a function that will be called to 
                                send a byte.
   @param[in] on_rx_frame_fn    Pointer to function that is called when a
                                correct frame is received.
 */
void hdlc_init(hdlc_tx_u8_fn_t       tx_u8_fn,
               hdlc_on_rx_frame_fn_t on_rx_frame_fn);

/**
    Function handler that is fed all raw received data.

    @param[in] data     received 8-bit data

 */
void hdlc_on_rx_u8(u8_t data);

/**
    Encapsulate and send an HDLC frame.

    @param[in] data         Buffer containing data for transmission
    @param[in] nr_of_bytes  Number of bytes in buffer to be transmitted

 */
void hdlc_tx_frame(const u8_t * data, size_t nr_of_bytes);

/* _____MACROS_______________________________________________________________ */

#ifdef __cplusplus
}
#endif
/// @}

#endif // #ifndef __HDLC_H__
