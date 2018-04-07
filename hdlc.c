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

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "hdlc.h"
#include "crc.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
// Significant octet values
#define HDLC_FLAG_SEQUENCE  0x7e   // Flag Sequence
#define HDLC_CONTROL_ESCAPE 0x7d   // Asynchronous Control Escape
#define HDLC_ESCAPE_BIT     0x20   // Asynchronous transparency modifier

/* _____LOCAL VARIABLES______________________________________________________ */
static u8_t   hdlc_rx_frame[HDLC_MRU];
static u8_t   hdlc_rx_frame_index;
static crc_t  hdlc_rx_frame_fcs;
static bool_t hdlc_rx_esc_flag;

/// Pointer to the function that will be called to send a character
static hdlc_tx_u8_fn_t hdlc_tx_u8_fn;

/// Pointer to the function that will be called to process a received HDLC frame
static hdlc_on_rx_frame_fn_t hdlc_on_rx_frame_fn;

/* _____PRIVATE FUNCTION PROTOTYPES__________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____PRIVATE FUNCTIONS____________________________________________________ */
/// Function to send a byte
static void hdlc_tx_u8(u8_t data)
{
    // Call provided function
    (*hdlc_tx_u8_fn)(data);
}

/// Function to send an escaped byte
static void hdlc_esc_tx_u8(u8_t data)
{
    // See if data should be escaped
    if((data == HDLC_CONTROL_ESCAPE) || (data == HDLC_FLAG_SEQUENCE))
    {
        // Send control escape byte
        hdlc_tx_u8(HDLC_CONTROL_ESCAPE);
        // Toggle escape bit
        data ^= HDLC_ESCAPE_BIT;
    }
    
    // Send data
    hdlc_tx_u8(data);
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void hdlc_init(hdlc_tx_u8_fn_t       tx_u8_fn,
               hdlc_on_rx_frame_fn_t on_rx_frame_fn)
{
    hdlc_rx_frame_index = 0;
    hdlc_rx_frame_fcs   = CRC_INIT_VAL;
    hdlc_rx_esc_flag    = FALSE;
    hdlc_tx_u8_fn       = tx_u8_fn;
    hdlc_on_rx_frame_fn = on_rx_frame_fn;
}

void hdlc_on_rx_u8(u8_t data)
{
    // Start/End sequence?
    if(data == HDLC_FLAG_SEQUENCE)
    {
        /* 
           If Escape sequence + End sequence is received then this packet must 
           be silently discarded.
         */
        if(hdlc_rx_esc_flag == TRUE)
        {
            // Reset flag
            hdlc_rx_esc_flag = FALSE;
        }
        // Minimum requirement for a valid frame is reception of good FCS
        else if(  (hdlc_rx_frame_index >= sizeof(hdlc_rx_frame_fcs)) 
                &&(hdlc_rx_frame_fcs   == CRC_MAGIC_VAL            )  )
        {
            // Pass on frame with FCS field removed
            (*hdlc_on_rx_frame_fn)(hdlc_rx_frame,
                                   hdlc_rx_frame_index - sizeof(hdlc_rx_frame_fcs));
        }
        // Reset for next packet
        hdlc_rx_frame_index = 0;
        hdlc_rx_frame_fcs   = CRC_INIT_VAL;
        return;
    }

    // Escape sequence processing?
    if(hdlc_rx_esc_flag)
    {
        // Reset flag
        hdlc_rx_esc_flag  = FALSE;
        // Toggle escape bit to restore data to correct state
        data             ^= HDLC_ESCAPE_BIT;
    }
    else if(data == HDLC_CONTROL_ESCAPE)
    {
        // Set flag to indicate that the next byte's escape bit must be toggled
        hdlc_rx_esc_flag = TRUE;
        // Discard control escape byte (do not buffer it)
        return;
    }

    // Store received data
    hdlc_rx_frame[hdlc_rx_frame_index] = data;

    // Update checksum
    hdlc_rx_frame_fcs = crc_update_u8(hdlc_rx_frame_fcs, data);

    // Advance to next position in buffer
    hdlc_rx_frame_index++;

    // Check for buffer overflow
    if(hdlc_rx_frame_index >= HDLC_MRU)
    {
        // Wrap index
        hdlc_rx_frame_index  = 0;

        // Invalidate FCS so that packet will be rejected
        hdlc_rx_frame_fcs  ^= 0xFFFF;
    }
}

void hdlc_tx_frame(const u8_t * data, size_t nr_of_bytes)
{
    u8_t  data_u8;
    crc_t fcs = CRC_INIT_VAL;    

    // Send start marker
    hdlc_tx_u8(HDLC_FLAG_SEQUENCE);

    // Send escaped data
    while(nr_of_bytes)
    {
        // Get next byte
        data_u8 = *data++;
        
        // Update checksum
        fcs = crc_update_u8(fcs, data_u8);
        
        // ESC send data
        hdlc_esc_tx_u8(data_u8);
                
        // decrement counter
        nr_of_bytes--;
    }

    // Invert checksum
    fcs ^= 0xffff;

    // Low byte of inverted FCS
    data_u8 = U16_LO8(fcs);
    // ESC send data
    hdlc_esc_tx_u8(data_u8);

    // High byte of inverted FCS
    data_u8 = U16_HI8(fcs);
    // ESC send data
    hdlc_esc_tx_u8(data_u8);

    // Send end marker
    hdlc_tx_u8(HDLC_FLAG_SEQUENCE);    
}

