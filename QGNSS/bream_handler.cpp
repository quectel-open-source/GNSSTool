/*******************************************************************************
* Copyright 2019 Broadcom Limited -- http://www.broadcom.com
* This program is the proprietary software of Broadcom Limited and/or
* its licensors, and may only be used, duplicated, modified or distributed
* pursuant to the terms and conditions of a separate, written license
* agreement executed between you and Broadcom (an "Authorized License").
* Except as set forth in an Authorized License, Broadcom grants no license
* (express or implied), right to use, or waiver of any kind with respect to
* the Software, and Broadcom expressly reserves all rights in and to the
* Software and all intellectual property rights therein. IF YOU HAVE NO
* AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
* WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF
* THE SOFTWARE.
* ---------------------------------------------------------------------------*/
/** @file bream_handler.cpp
*============================================================================*/

#include <stdio.h>
#include <string.h>
#include "bream.h"
#include "bream_handler.h"
#include <QDebug>

#define SYNC_CHAR1  0xB5
#define SYNC_CHAR2  0x62

#define LD2BRM_PREAMBLE_SIZE    2
#define LD2BRM_HEADER_SIZE      4
#define LD2BRM_CHKSUM_SIZE      2

enum
{
    WAIT_FOR_SYNC_CHAR1 = 0,
    WAIT_FOR_SYNC_CHAR2,
    WAIT_FOR_HEADER,
    WAIT_FOR_MESSAGE_COMPLETE,
};


void ResetState()
{
}

void DumpRawPacket(uint8_t *buf, uint16_t size, bool bwrite)
{
}

void AsicData(unsigned char *pucData, unsigned short usLen)
{
}

void HandlePayload(uint8_t cls, uint8_t id, uint16_t payloadLen, unsigned char *pucPayload)
{
    uint16_t clsid = cls << 8 | id;
    //LD2_LOG("RX %s\n", BreamHelper::GetInstance().GetNameOfBreamPacket(cls, id));
    switch (clsid)
    {
    case BRM_ACK_ACK:
        break;
    default:
        break;
  }
}

void Send(uint16_t payloadLen, unsigned char *pucPayload)
{
    DumpRawPacket(pucPayload, payloadLen, true);
    /*TODO: send to */
}

void BuildBreamFrame(uint16_t clsid, uint16_t payloadLen, unsigned char *pucPayload, char *buf)
{

    buf[0] = SYNC_CHAR1;
    buf[1] = SYNC_CHAR2;
    buf[2] = (clsid>>8) & 0xFF;
    buf[3] = clsid & 0xFF;
    buf[4] = payloadLen & 0xFF;
    buf[5] = (payloadLen >> 8) & 0xFF;
    memcpy(&buf[6], pucPayload, payloadLen);

    // check crc
    unsigned char chkA = 0, chkB = 0;
    for (int i = 2; i < (LD2BRM_PREAMBLE_SIZE+LD2BRM_HEADER_SIZE+ payloadLen); i++)
    {
        chkA = chkA + buf[i];
        chkB = chkB + chkA;
        //LD2_LOG("ch=%02X, A=%02X B=%02X\n", buf[i], chkA, chkB);
    }

    buf[LD2BRM_PREAMBLE_SIZE + LD2BRM_HEADER_SIZE + payloadLen] = chkA;
    buf[LD2BRM_PREAMBLE_SIZE + LD2BRM_HEADER_SIZE + payloadLen+1] = chkB;

    return;
    //Send(LD2BRM_PREAMBLE_SIZE + LD2BRM_HEADER_SIZE + payloadLen + LD2BRM_CHKSUM_SIZE, buf);
}



