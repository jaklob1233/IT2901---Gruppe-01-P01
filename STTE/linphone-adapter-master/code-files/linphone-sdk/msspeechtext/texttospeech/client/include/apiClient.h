#ifndef INCLUDE_TEXTTOSPEECH_API_CLIENT_H
#define INCLUDE_TEXTTOSPEECH_API_CLIENT_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <curl/curl.h>
#include "../include/list.h"
#include "../include/keyValuePair.h"
#include "../include/binary.h"

typedef struct texttospeech_sslConfig_t {
    char *clientCertFile;         /* client certificate */
    char *clientKeyFile;          /* client private key */
    char *CACertFile;             /* CA certificate */
    int  insecureSkipTlsVerify ;  /* 0 -- verify server certificate */
                                  /* 1 -- skip ssl verify for server certificate */
} texttospeech_sslConfig_t;

typedef struct texttospeech_apiClient_t {
    char *basePath;
    texttospeech_sslConfig_t *sslConfig;
    void *dataReceived;
    long dataReceivedLen;
    void (*data_callback_func)(void **, long *);
    int (*progress_func)(void *, curl_off_t, curl_off_t, curl_off_t, curl_off_t);
    void *progress_data;
    long response_code;
} texttospeech_apiClient_t;

texttospeech_apiClient_t* texttospeech_apiClient_create();

texttospeech_apiClient_t* texttospeech_apiClient_create_with_base_path(const char *basePath
, texttospeech_sslConfig_t *sslConfig
);

void texttospeech_apiClient_free(texttospeech_apiClient_t *apiClient);

void texttospeech_apiClient_invoke(texttospeech_apiClient_t *apiClient,const char* operationParameter, list_t *queryParameters, list_t *headerParameters, list_t *formParameters,list_t *headerType,list_t *contentType, const char *bodyParameters, const char *requestType);

texttospeech_sslConfig_t *texttospeech_sslConfig_create(const char *clientCertFile, const char *clientKeyFile, const char *CACertFile, int insecureSkipTlsVerify);

void texttospeech_sslConfig_free(texttospeech_sslConfig_t *sslConfig);

char *texttospeech_strReplace(char *orig, char *rep, char *with);

/*
 * In single thread program, the function apiClient_setupGlobalEnv is not needed.
 * But in multi-thread program, apiClient_setupGlobalEnv must be called before any worker thread is created
 */
void texttospeech_apiClient_setupGlobalEnv();

/*
 * This function apiClient_unsetupGlobalEnv must be called whether single or multiple program.
 * In multi-thread program, it is must be called after all worker threads end.
 */
void texttospeech_apiClient_unsetupGlobalEnv();

#endif // INCLUDE_TEXTTOSPEECH_API_CLIENT_H
