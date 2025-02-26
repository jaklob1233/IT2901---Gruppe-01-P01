/*
 * Company:    THALES Communications & Security
 *
 * Copyright   2023 - Gennevilliers - THALES Communications & Security
 *
 * All rights especially the right for copying and distribution as
 * well as translation reserved.
 * No part of the product shall be reproduced or stored, processed
 * copied or distributed with electronic tools or by paper copy or
 * any other process without written authorization of
 * THALES Communications & Security
 */
#include "params.h"
#include "version.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <mediastreamer2/msfilter.h>
#include <melpwb/melpwb.h>

#define PTIME              90   /* packetization time (in ms)    */
#define SRATE           16000   /* sample rate        (in Hz)    */
#define NBYTES              7   /* bitstream size     (in bytes) */
#define NSAMPLES          360   /* samples per frame             */
#define N       (10*PTIME/225)  /* frames per PTIME              */

#define export          __attribute__((visibility("default")))
#ifndef NDEBUG
#define trace(...)      do { printf("libmsmelpwb: "); printf(__VA_ARGS__); printf("\n"); } while (0)
#else
#define trace(...)      (void)0
#endif

/*
 * load/unload
 */
static void __attribute__((constructor)) load(void) {
    trace("load v"VERSION);
    params_init();
}

static void __attribute__((destructor)) unload(void) {
    trace("unload");
    params_uninit();
}



/*
 * encoder
 */
typedef struct encoder_data_s {
    MSBufferizer* bufferizer;   /* bufferizer               */
    void* melpwb;               /* melpwb wideband analyzer */
    FILE* file;                 /* source (file or micro)   */
    uint32_t timestamp;         /* timestamp                */
} encoder_data_t;

static void encoder_init(MSFilter* filter) {
    encoder_data_t* data = ms_new0(encoder_data_t, 1);

    data->bufferizer = ms_bufferizer_new();
    data->timestamp = 0;

    params_lock();
    params_t* p = params();
    trace("npp=%s, file=%s", p->npp ? "true" : "false", p->fin);
    melpwb_analyzer_new(&data->melpwb, MELPWB_RATE_1200, p->npp);
    data->file = p->fin ? fopen(p->fin, "r") : NULL;
    params_unlock();

    filter->data = data;
}

static void encoder_uninit(MSFilter* filter) {
    encoder_data_t* data = (encoder_data_t*)filter->data;
    if (data->file)
        fclose(data->file);
    melpwb_analyzer_free(data->melpwb);
    ms_bufferizer_destroy(data->bufferizer);
    ms_free(data);
}

static void encoder_process(MSFilter* filter) {
    encoder_data_t* data = (encoder_data_t*)filter->data;

    ms_filter_lock(filter);
    mblk_t* im;
    while (im = ms_queue_get(filter->inputs[0]))
        ms_bufferizer_put(data->bufferizer, im);

    int16_t buf[N*NSAMPLES];
    while (ms_bufferizer_read(data->bufferizer, (uint8_t*)buf, sizeof(buf)) == sizeof(buf)) {
        if (data->file) {   /* if file is available, replace input source by file content */
            size_t size = fread(buf, 1, sizeof(buf), data->file);
            if (size < sizeof(buf)) {
                rewind(data->file);
                size = fread(&buf[size/sizeof(buf[0])], 1, sizeof(buf)-size, data->file);
            }
        }

        mblk_t* om = allocb(N*NBYTES, 0);
        ms_bufferizer_fill_current_metas(data->bufferizer, om);
        mblk_set_timestamp_info(om, data->timestamp);
        data->timestamp += N*NSAMPLES;

        for (int i = 0; i < N; ++i)
            melpwb_analyze(data->melpwb, &buf[i*NSAMPLES], &om->b_wptr[i*NBYTES]);

        om->b_wptr += N*NBYTES;
        ms_queue_put(filter->outputs[0], om);
    }
    ms_filter_unlock(filter);
}

static int encoder_nchannels(MSFilter* filter, void* arg) {
    (void)filter;
    *(int*)arg = 1;
    return 0;
}

static int encoder_srate(MSFilter* filter, void* arg) {
    (void)filter;
    *(int*)arg = SRATE;
    return 0;
}



/*
 * decoder
 */
typedef struct decoder_data_s {
    void* melpwb;   /* melp wideband synthesizer */
    FILE* file;     /* sink (optional)           */
} decoder_data_t;

static void decoder_init(MSFilter* filter) {
    decoder_data_t* data = ms_new0(decoder_data_t, 1);

    params_lock();
    params_t* p = params();
    trace("pf=%s, file=%s", p->pf ? "true" : "false", p->fout);
    melpwb_synthesizer_new(&data->melpwb, MELPWB_RATE_1200, p->pf);
    data->file = p->fout ? fopen(p->fout, "w") : NULL;
    params_unlock();

    filter->data = data;
}

static void decoder_uninit(MSFilter* filter) {
    decoder_data_t* data = (decoder_data_t*)filter->data;
    if (data->file)
        fclose(data->file);
    melpwb_synthesizer_free(data->melpwb);
    ms_free(data);
}

static void decoder_process(MSFilter* filter) {
    decoder_data_t* data = (decoder_data_t*)filter->data;

    mblk_t* im;
    while ((im = ms_queue_get(filter->inputs[0]))) {
        msgpullup(im, -1);

        int16_t frm[N*NSAMPLES];
        for (int i = 0; i < N; ++i) {
            int ret = melpwb_synthesize(data->melpwb, &im->b_rptr[i*NBYTES], &frm[i*NSAMPLES]);
            if (ret == 0)
                memset(&frm[i*NSAMPLES], 0, NSAMPLES*sizeof(int16_t));
        }
        if (data->file)     /* if file is available, record frame */
            fwrite(frm, 1, sizeof(frm), data->file);

        mblk_t* om = allocb(sizeof(frm), 0);
        mblk_meta_copy(im, om);
        memcpy(om->b_wptr, frm, sizeof(frm));
        om->b_wptr += sizeof(frm);
        ms_queue_put(filter->outputs[0], om);
        freemsg(im);
    }
}

static int decoder_plc(MSFilter* filter, void* arg) {
    (void)filter;
    *(int*)arg = 0;
    return 0;
}

static int decoder_nchannels(MSFilter* filter, void* arg) {
    (void)filter;
    *(int*)arg = 1;
    return 0;
}

static int decoder_srate(MSFilter* filter, void* arg) {
    (void)filter;
    *(int*)arg = SRATE;
    return 0;
}



/*
 * plugin
 */
static MSFilterMethod encoder_methods[] = {{   MS_FILTER_GET_NCHANNELS, encoder_nchannels },
                                           { MS_FILTER_GET_SAMPLE_RATE, encoder_srate     },
                                           {                         0, NULL              }};
MSFilterDesc melpwb_encoder_desc = { .id          = MS_FILTER_PLUGIN_ID,
                                     .name        = "melpwb_encoder",
                                     .text        = "MELP wideband encoder",
                                     .category    = MS_FILTER_ENCODER,
                                     .enc_fmt     = "MELPWB",
                                     .ninputs     = 1,
                                     .noutputs    = 1,
                                     .init        = encoder_init,
                                     .preprocess  = NULL,
                                     .process     = encoder_process,
                                     .postprocess = NULL,
                                     .uninit      = encoder_uninit,
                                     .methods     = encoder_methods,
                                     .flags       = 0 };
MS_FILTER_DESC_EXPORT(melpwb_encoder_desc);

static MSFilterMethod decoder_methods[] = {{ MS_AUDIO_DECODER_HAVE_PLC, decoder_plc       },
                                           {   MS_FILTER_GET_NCHANNELS, decoder_nchannels },
                                           { MS_FILTER_GET_SAMPLE_RATE, decoder_srate     },
                                           {                         0, NULL              }};
MSFilterDesc melpwb_decoder_desc = { .id          = MS_FILTER_PLUGIN_ID,
                                     .name        = "melpwb_decoder",
                                     .text        = "MELP wideband decoder",
                                     .category    = MS_FILTER_DECODER,
                                     .enc_fmt     = "MELPWB",
                                     .ninputs     = 1,
                                     .noutputs    = 1,
                                     .init        = decoder_init,
                                     .preprocess  = NULL,
                                     .process     = decoder_process,
                                     .postprocess = NULL,
                                     .uninit      = decoder_uninit,
                                     .methods     = decoder_methods,
                                     .flags       = 0 };
MS_FILTER_DESC_EXPORT(melpwb_decoder_desc);

export void libmsmelpwb_init(MSFactory* factory) {
    ms_factory_register_filter(factory, &melpwb_encoder_desc);
    ms_factory_register_filter(factory, &melpwb_decoder_desc);
    ms_message("plugin libmsmelpwb v"VERSION" loaded");
}

