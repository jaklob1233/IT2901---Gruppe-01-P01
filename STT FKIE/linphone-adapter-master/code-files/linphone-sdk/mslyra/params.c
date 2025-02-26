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
#include "params.h"
#include <pthread.h>
#include <pwd.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "mediastreamer2/mscodecutils.h"

static char*           fname;
static pthread_mutex_t mutex;
static params_t        singleton[3];

static char* find(void);
static void  parse(const char* fname);

int params_init(void) {
    fname = find();
    pthread_mutex_init(&mutex, NULL);
    memset(singleton, 0, sizeof(singleton));
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

params_t* params(int id) {
    struct stat s;
    if (stat(fname, &s) == 0) {
        static time_t time;
        if (time != s.st_mtim.tv_sec) {
            time = s.st_mtim.tv_sec;
            parse(fname);
        }
    }
    else {
        memset(singleton, 0, sizeof(singleton));
        ms_message("Lyra - could not open config file");
    }

    return &singleton[id];
}

static char* find(void) {
    #define libname "libmslyra"
    #define LIBNAME "LIBMSLYRA"
    #define PATH    "/.config/linphone/"

    char* fname;
    if ((fname = getenv(LIBNAME"_CONF")) == NULL) {
        const char* home;
        if ((home = getenv("HOME")) == NULL)
            home = getpwuid(getuid())->pw_dir;

        fname = (char*)malloc(strlen(home)+strlen(PATH)+strlen(libname)+strlen(".conf")+1);
        sprintf(fname, "%s"PATH"%s.conf", home, libname);
        ms_message("Lyra - config file is searched under %s", fname);
        return fname;
    }
    else
        return strdup(fname);
}

static void parse(const char* fname) {
    memset(singleton, 0, sizeof(singleton));

    FILE* file = fopen(fname, "r");
    if (!file) {
        ms_message("Lyra - could not read config file");
        return;
    }
    ms_message("Lyra - config file read");

    params_t* p = NULL;
    size_t size = 0;
    char*  line = NULL;
    while (getline(&line, &size, file) != -1) {
        {
            char* mime;
            int ret = sscanf(line, " [ %m[^]] ] ", &mime);
            if (ret == 1) {
                if (strcasecmp(mime, "LYRA3200") == 0)
                    p = &singleton[0];
                else if (strcasecmp(mime, "LYRA6000") == 0)
                    p = &singleton[1];
                else if (strcasecmp(mime, "LYRA9200") == 0)
                    p = &singleton[2];
                else
                    (void)0;

                free(mime);
                continue;
            }
            if (p == NULL)
                continue;
        }

        {
            char* key;
            char* value;
            int ret = sscanf(line, " %m[^= \t] = %ms ", &key, &value);
            if (ret < 2) {
                if (ret == 1)
                    free(key);
                continue;
            }
            if (strcasecmp(key, "fin") == 0)
                strcpy(p->fin, value);
            else if (strcasecmp(key, "fout") == 0)
                strcpy(p->fout, value);
            else if (strcasecmp(key, "dtx") == 0)
                p->dtx = strcasecmp(value, "true") == 0;
            else if (strcasecmp(key, "noise_preprocessing") == 0)
                p->noise_preprocessing = strcasecmp(value, "true") == 0;
            else if (strcasecmp(key, "ptime") == 0)
                p->ptime = atoi(value);
            else
                (void)0;

            free(value);
            free(key);
        }
    }
    if (line)
        free(line);

    fclose(file);
}

