#include "PacketSerial.h"

PacketSerial::PacketSerial(int32_t *registar)
{
  _registar = registar;
}

void PacketSerial::writeData(int32_t data, uint8_t reg)
{
    uint8_t dataBytes[4] =
    {
        (data >> 24) & 0xFF,
        (data >> 16) & 0xFF,
        (data >>  8) & 0xFF,
        (data >>  0) & 0xFF
    };

    Serial.write(HEAD_BYTE);
    Serial.write(reg);
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < 4; ++i)
    {
        if ((dataBytes[i] == ESCAPE_BYTE) || (dataBytes[i] == HEAD_BYTE))
        {
            Serial.write(ESCAPE_BYTE);
            checksum += ESCAPE_BYTE;
            Serial.write(dataBytes[i] ^ ESCAPE_MASK);
            checksum += dataBytes[i] ^ ESCAPE_MASK;
        }
        else
        {
            Serial.write(dataBytes[i]);
            checksum += dataBytes[i];
        }
    }

    // 末尾にチェックサムを追加で送信する
    Serial.write(checksum);
}
void PacketSerial::readData()
{
    uint8_t bytes[4] = {0,0,0,0};
    int8_t checksum = 0;
    delayMicroseconds(500);
    uint8_t data = Serial.read();
    delayMicroseconds(500);

    if (data == HEAD_BYTE)
    {
        uint8_t reg = Serial.read();
        for (int i = 0; i < 4; ++i)
        {
            uint8_t d = Serial.read();
            delayMicroseconds(500);
            if (d == ESCAPE_BYTE)
            {
                uint8_t nextByte = Serial.read();
                bytes[i] = nextByte ^ ESCAPE_MASK;
                checksum += (d + nextByte);
            }
            else
            {
                bytes[i] = d;
                checksum += d;
            }
        }
        int8_t checksum_recv = Serial.read();
        delayMicroseconds(500);
        int32_t DATA = 0x00;
        for(int i = 0; i < 4; i++)
        {
            DATA |= (((int32_t)bytes[i]) << (24 - (i*8)));
        }

        if (checksum == checksum_recv)
        {
            *(_registar + reg) =  DATA;
        }
        else
        {
            // data error
        }
    }
}

void PacketSerial::writeFloatData(float data, uint8_t reg)
{
  int integer = (int)data;
  uint32_t decimal = (uint32_t)((data-integer)*1000000);
  writeData(integer, reg);
  writeData(decimal, reg+1);
}

float PacketSerial::getFloatData(uint8_t reg)
{
  return *(_registar + reg) + ((float)*(_registar + reg+1))/1000000;
}
