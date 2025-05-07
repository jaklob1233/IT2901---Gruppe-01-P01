/*
 * Company:    THALES Communications & Security
 *
 * Copyright   2023 - Gennevillliers - THALES Communications & Security
 *
 * All rights especially the right for copying and distribution as
 * well as translation reserved.
 * No part of the product shall be reproduced or stored, processed
 * copied or distributed with electronic tools or by paper copy or
 * any other process without written authorization of
 * THALES Communications & Security
 */
#ifndef PARAMS_H
#define PARAMS_H

typedef struct params_s {
    char dil_interface_name[256];
    char fin[256];
    char fout[256];
    int  ptime;
    int  audio_chunk_size;
    char url_central_logger[256];
    char url_speech_to_text_webhook[256];
    char url_text_to_speech_webhook[256];
    char url_mqtt_transport_webhook[256];
    char speechtotext_config_profile[256];
    char texttospeech_config_profile[256];
    char speechtotext_variant[256];
    char texttospeech_variant[256];
    char mqtt_variant[256];
    char mqtt_server_ip[256];
    int  mqtt_server_port;
    char mqtt_source_ip[256];
    char mqtt_destination_ip[256];
    char mqtt_sender_id[256];
    char mqtt_receiver_id[256];
    char mqtt_config_profile[256];
    char mqtt_base_topic[256];
    char mqtt_node_id[256];
} params_t;

int params_init(void);

void params_uninit(void);

void params_lock(void);

void params_unlock(void);

params_t *params(void);

#endif

