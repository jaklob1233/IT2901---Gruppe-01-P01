#include <stdlib.h>
#include <stdio.h>
#include "../include/apiClient.h"
#include "../include/list.h"
#include "../external/cJSON.h"
#include "../include/keyValuePair.h"
#include "../include/binary.h"
#include "../model/error.h"
#include "../model/message.h"

// Enum MQTTVARIANT for DefaultAPI_initialize
typedef enum  { mqtttransport_initialize_MQTTVARIANT_NULL = 0, mqtttransport_initialize_MQTTVARIANT_Standard, mqtttransport_initialize_MQTTVARIANT_MQTT_UDP } mqtttransport_initialize_mqtt_variant_e;


// Initialize the MQTT-Transport component
//
// Initialize the MQTT-Transport component
//
void
Mqtttransport_initialize(mqtttransport_apiClient_t *apiClient, mqtttransport_initialize_mqtt_variant_e mqtt_variant, char *config_profile, char *base_topic, char *node_id, char *webhook_url, char *mqtt_server_ip, int *mqtt_server_port);


void
Mqtttransport_onNewMessagePost(mqtttransport_apiClient_t *apiClient, message_t *message);


// Send message via MQTT Transport component
//
// Send message via MQTT Transport component
//
void
Mqtttransport_sendMessage(mqtttransport_apiClient_t *apiClient, char *source, char *destination, message_t *message);


