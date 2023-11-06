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
/** @file bream_handler.h  
*============================================================================*/
#include <string.h>
#include <stdint.h>
#include "bream.h"

#ifndef BREAM_HANDLER_H
#define BREAM_HANDLER_H

typedef void(*LD2BRM_Nmea)(U1*, U4);
typedef void(*LD2BRM_RawData)(U1*, U2);

void ResetState();
void Send(uint16_t payloadLen, unsigned char *pucPayload);
void BuildBreamFrame(uint16_t clsid, uint16_t payloadLen, unsigned char *pucPayload,  char *buffer);

void Send(LD2BRM_StpBatchSetPayload &payload);
void Send(LD2BRM_StpConfigCommandPayload &payload);
void Send(LD2BRM_StpConfigCommandPayloadWithOption &payload);
void Send(LD2BRM_StpGeofenceSetPayload &payload);
void Send(LD2BRM_StpSynchPollPayload &payload);
void Send(LD2BRM_StpSynchSetPayload &payload);
void Send(LD2BRM_StpMeSettingsPollPayload &payload);
void Send(LD2BRM_StpMeSettingsSetPayload &payload);
void Send(LD2BRM_StpInfoPollPayload &payload);
void Send(LD2BRM_StpInfoSetPayload &payload);
void Send(LD2BRM_StpLogfilterSetPayload &payload);
void Send(LD2BRM_StpPeSettingsPollPayload &payload);
void Send(LD2BRM_StpPeSettingsSetPayload &payload);
void Send(LD2BRM_StpMessagePollPayload &payload);
void Send(LD2BRM_StpMessageSetsPayload &payload);
void Send(LD2BRM_StpMessageSetPayload &payload);
void Send(LD2BRM_StpNmeaSetDeprecatedPayload &payload);
void Send(LD2BRM_StpNmeaSetV0Payload &payload);
void Send(LD2BRM_StpNmeaSetV1Payload &payload);
void Send(LD2BRM_StpPwrModeSetPayload &payload);
void Send(LD2BRM_StpPortPollPayload &payload);
void Send(LD2BRM_StpPortSetUartPayload &payload);
void Send(LD2BRM_StpPortSetUsbPayload &payload);
void Send(LD2BRM_StpPortSetSpiPayload &payload);
void Send(LD2BRM_StpPortSetDdcPayload &payload);
void Send(LD2BRM_StpRateSetPayload &payload);
void Send(LD2BRM_StpResetCommandPayload &payload);
void Send(LD2BRM_StpRxmSetPayload &payload);
void Send(LD2BRM_StpBiasSetPayload &payload);
void Send(LD2BRM_StpBiasPollPayload &payload);
void Send(LD2BRM_StpFctTestSetPayload &payload);
void Send(LD2BRM_StpFctTestPollPayload &payload);
void Send(LD2BRM_LogCreateCommandPayload &payload);
void Send(LD2BRM_LogEraseCommand &payload);
void Send(LD2BRM_LogFindtimeInputPayload &payload);
void Send(LD2BRM_LogInfoPollPayload &payload);
void Send(LD2BRM_LogRetrievebatchCommandPayload &payload);
void Send(LD2BRM_LogRetrieveCommandPayload &payload);
void Send(LD2BRM_AstLtoInputPayload &payload);
void Send(LD2BRM_AstNvmemPollPayload &payload);
void Send(LD2BRM_AstNvmemInputPayload &payload);
void Send(LD2BRM_AstIniPosXyzInputPayload &payload);
void Send(LD2BRM_AstIniPosLlhInputPayload &payload);
void Send(LD2BRM_AstRefTimeUtcInputPayload &payload);
void Send(LD2BRM_AstIniTimeGnssInputPayload &payload);
void Send(LD2BRM_AstIniClkdInputPayload &payload);
void Send(LD2BRM_StsVerPollPayload &payload);
void Send(LD2BRM_PvtResetAccDistanceCommandPayload &payload);
void Send(LD2BRM_AscPwrCycleCommandPayload *payload);
void Send(LD2BRM_BupPollPayload &payload);
void Send(LD2BRM_BupCommandPayload &payload);
void Send(LD2BRM_StpConfig2PollPayload &payload);
void Send(LD2BRM_StpConfig2CommandPayload *payload);

#endif  // BREAM_HANDLER_H
