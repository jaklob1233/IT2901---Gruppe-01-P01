/*
 * Company:    Fraunhofer FKIE
 *
 * Copyright   2024
 *
 * All rights especially the right for copying and distribution as
 * well as translation reserved.
 * No part of the product shall be reproduced or stored, processed
 * copied or distributed with electronic tools or by paper copy or
 * any other process without written authorization of
 * THALES Communications & Security
 */
#include "params.h"
#include <pthread.h>
#include <pwd.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "mediastreamer2/msfilter.h"

static char*           fname;
static pthread_mutex_t mutex;
static params_t        singleton;

static char* find(void);
static void  parse(const char* fname);

int params_init(void) {
    fname = find();
    pthread_mutex_init(&mutex, NULL);
    memset(&singleton, 0, sizeof(singleton));
    return 0;
}

void params_uninit(void) {
    pthread_mutex_destroy(&mutex);
    if (fname)
        free(fname);
}

void params_lock(void) {
    pthread_mutex_lock(&mutex);
}

void params_unlock(void) {
    pthread_mutex_unlock(&mutex);
}

params_t* params(void) {
    struct stat s;
    if (stat(fname, &s) == 0) {
        static time_t time;
        if (time != s.st_mtim.tv_sec) {
            time = s.st_mtim.tv_sec;
            parse(fname);
        }
    }
    else
        memset(&singleton, 0, sizeof(singleton));

    return &singleton;
}

static char* find(void) {
    #define libname "libmsspeechtext"
    #define LIBNAME "LIBMSSPEECHTEXT"
    #define PATH    "/.config/linphone/"

    char* fname;
    if ((fname = getenv(LIBNAME"_CONF")) == NULL) {
        const char* home;
        if ((home = getenv("HOME")) == NULL)
            home = getpwuid(getuid())->pw_dir;

        fname = (char*)malloc(strlen(home)+strlen(PATH)+strlen(libname)+strlen(".conf")+1);
        sprintf(fname, "%s"PATH"%s.conf", home, libname);
        return fname;
    }
    else
        return strdup(fname);
}

static void parse(const char* fname) {
    memset(&singleton, 0, sizeof(singleton));

    FILE* file = fopen(fname, "r");
    if (!file)
        return;

    params_t* p = NULL;
    size_t size = 0;
    char*  line = NULL;
    while (getline(&line, &size, file) != -1) {
        char* key;
        char* value;
        int ret = sscanf(line, " %m[^= \t] = %ms ", &key, &value);
        if (ret < 2) {
            if (ret == 1)
                free(key);
            continue;
        }
        if (strcasecmp(key, "dil_interface_name") == 0)
            strcpy(singleton.dil_interface_name, value);
        else if (strcasecmp(key, "fin") == 0)
            strcpy(singleton.fin, value);
        else if (strcasecmp(key, "fout") == 0)
            strcpy(singleton.fout, value);
        else if (strcasecmp(key, "ptime") == 0)
            singleton.ptime = atoi(value);
        else if (strcasecmp(key, "audio_chunk_size") == 0)
            singleton.audio_chunk_size = atoi(value);
        else if (strcasecmp(key, "url_central_logger") == 0) {
            strcpy(singleton.url_central_logger, value);
        }
        else if (strcasecmp(key, "url_speech_to_text_webhook") == 0)
            strcpy(singleton.url_speech_to_text_webhook, value);
        else if (strcasecmp(key, "url_text_to_speech_webhook") == 0)
            strcpy(singleton.url_text_to_speech_webhook, value);
        else if (strcasecmp(key, "url_mqtt_transport_webhook") == 0)
            strcpy(singleton.url_mqtt_transport_webhook, value);
        else if (strcasecmp(key, "speechtotext_config_profile") == 0)
            strcpy(singleton.speechtotext_config_profile, value);
        else if (strcasecmp(key, "texttospeech_config_profile") == 0)
            strcpy(singleton.texttospeech_config_profile, value);
        else if (strcasecmp(key, "speechtotext_variant") == 0)
            strcpy(singleton.speechtotext_variant, value);
        else if (strcasecmp(key, "texttospeech_variant") == 0)
            strcpy(singleton.texttospeech_variant, value);
        else if (strcasecmp(key, "mqtt_variant") == 0)
            strcpy(singleton.mqtt_variant, value);
        else if (strcasecmp(key, "mqtt_server_ip") == 0)
            strcpy(singleton.mqtt_server_ip, value);
        else if (strcasecmp(key, "mqtt_server_port") == 0)
            singleton.mqtt_server_port = atoi(value);
        else if (strcasecmp(key, "mqtt_source_ip") == 0)
            strcpy(singleton.mqtt_source_ip, value);
        else if (strcasecmp(key, "mqtt_destination_ip") == 0)
            strcpy(singleton.mqtt_destination_ip, value);
        else if (strcasecmp(key, "mqtt_sender_id") == 0)
            strcpy(singleton.mqtt_sender_id, value);
        else if (strcasecmp(key, "mqtt_receiver_id") == 0)
            strcpy(singleton.mqtt_receiver_id, value);
        else if (strcasecmp(key, "mqtt_config_profile") == 0)
            strcpy(singleton.mqtt_config_profile, value);
        else if (strcasecmp(key, "mqtt_base_topic") == 0)
            strcpy(singleton.mqtt_base_topic, value);
        else if (strcasecmp(key, "mqtt_node_id") == 0)
            strcpy(singleton.mqtt_node_id, value);
        else
            (void)0;

        free(value);
        free(key);
    }
    if (line)
        free(line);

    fclose(file);
}

