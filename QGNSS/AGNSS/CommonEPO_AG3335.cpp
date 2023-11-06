#include "CommonEPO_AG3335.h"
uint8_t gnss_app_binary_calculate_binary_checksum(const gnss_app_binary_binary_payload_t *const payload)
{
    uint8_t checksum = 0;
    uint8_t *pheader = NULL;
    uint8_t *pdata = NULL;
    uint16_t i;
    if (NULL == payload)
    {
        return 0;
    }
    /* The checksum is the 8-bit exclusive OR of all bytes in the payload. */
    pheader = (uint8_t *)payload;
    for (i = 0; i < GNSS_APP_BINARY_BINARY_PAYLOAD_HEADER_SIZE; i++)
    {
        checksum ^= *pheader;
        pheader++;
    }
    pdata = (uint8_t *)payload->data;
    for (i = 0; i < payload->data_size; i++)
    {
        checksum ^= *pdata;
        pdata++;
    }
    return checksum;
}

int16_t gnss_app_binary_encode_binary_packet(uint8_t *const buffer, uint16_t max_buffer_size, const gnss_app_binary_binary_payload_t* const payload)
{
    uint8_t *pbyte;
    uint16_t required_length;
    if (NULL == buffer || payload == NULL)
    {
        return -1;
    }
    required_length = payload->data_size + GNSS_APP_BINARY_BINARY_CONTROL_SIZE +
                      GNSS_APP_BINARY_BINARY_PAYLOAD_HEADER_SIZE;
    if (max_buffer_size < required_length)
    {
        return -1;
    }
    memset((void *)buffer, 0, max_buffer_size);
    buffer[0] = GNSS_APP_BINARY_BINARY_PREAMBLE1;
    buffer[1] = GNSS_APP_BINARY_BINARY_PREAMBLE2;
    pbyte = &buffer[2];
    memcpy(pbyte, payload, GNSS_APP_BINARY_BINARY_PAYLOAD_HEADER_SIZE);
    pbyte += GNSS_APP_BINARY_BINARY_PAYLOAD_HEADER_SIZE;
    memcpy(pbyte, payload->data, payload->data_size);
    pbyte += payload->data_size;
    *pbyte++ = gnss_app_binary_calculate_binary_checksum(payload);
    *pbyte++ = GNSS_APP_BINARY_BINARY_ENDWORD1;
    *pbyte = GNSS_APP_BINARY_BINARY_ENDWORD2;
    return required_length;
}


int16_t gnss_epo_encode_binary(uint16_t msg_id, char *buffer, uint16_t buffer_size,
                               char *data_input, int32_t data_length)
{
    gnss_app_binary_binary_payload_t payload;
    int16_t binary_message_length;
    memset((void *)&payload, 0, sizeof(gnss_app_binary_binary_payload_t));
    payload.message_id = msg_id;
    payload.data_size = (uint16_t)data_length;
    memcpy(payload.data, data_input, sizeof(uint8_t) * data_length);
    binary_message_length = gnss_app_binary_encode_binary_packet((uint8_t*)buffer, buffer_size, &payload);
    return binary_message_length;
}

int32_t epo_demo_get_sv_prn(int32_t type, uint8_t *data)
{
    int32_t sv_id, sv_prn = 0;

    sv_id = data[3];

    switch (type)
    {
    case EPO_DEMO_MODE_GPS:
        sv_prn = sv_id;
        break;
    case EPO_DEMO_MODE_GLONASS:
        sv_prn = sv_id - GNSS_GLONASS_EPO_BASE_ID;
        break;
    case EPO_DEMO_MODE_GALILEO:
        if (sv_id == 255)
        {
            sv_prn = 255;
        }
        else
        {
            sv_prn = sv_id - GNSS_GALILEO_EPO_BASE_ID;
        }
        break;
    case EPO_DEMO_MODE_BEIDOU:
        if(sv_id == 255) {
            sv_prn = 255;
        } else if(sv_id < 200){
            sv_prn = sv_id - 135;
        }
        else{
            sv_prn = sv_id - GNSS_BEIDOU_EPO_BASE_ID;
        }
        break;
    default:
        sv_prn = 0;
    }

    return sv_prn;
}
