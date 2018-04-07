#ifndef _HDLC_CRC16_H
#define _HDLC_CRC16_H

#include <QtCore/QtGlobal>
#include <QByteArray>

/// The generator polynomial CRC16-CCITT: x^16 + x^12 + x^5 + x^0
#define CRC_POLYNOMIAL  0x8408
/// Initial CRC value
#define CRC_INIT_VAL    0xffff
/// Magic CRC value
#define CRC_MAGIC_VAL   0xf0b8


class HDLC_crc16
{

public:

    HDLC_crc16(){}

    static quint16 crc_update_u8(quint16 crc, quint8 data)
    {
        quint8 i;

        crc ^= data;

        for(i=8; i!=0; i--)
        {
            if(crc & 1)
            {
                crc = (crc>>1) ^ CRC_POLYNOMIAL;
            }
            else
            {
                crc = (crc>>1);
            }
        }

        return crc;
    }

    static quint16 crc_update_data(QByteArray data)
    {
        quint16 crc = CRC_INIT_VAL;

        for(int i=0; i<data.size(); i++)
        {
            crc = crc_update_u8(crc, data.at(i));
        }

        return crc;
    }
};
#endif // _HDLC_CRC16_H
