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
#include "fifo.h"
#include "params.h"
#include "version.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <mediastreamer2/msfilter.h>
#include <melp/melp.h>

#define FLENGTH          180    /* frame length @22.5ms (in samples) */
#define PTIME             90    /* packetization time        (in ms) */
#define SRATE           8000    /* sample rate               (in Hz) */
#define FSIZE           (FLENGTH*sizeof(int16_t))
#define NSAMPLES        (PTIME*SRATE/1000)

#define export          __attribute__((visibility("default")))
#define lengthof(x)     (int)(sizeof(x)/sizeof((x)[0]))
#ifndef NDEBUG
#define trace(...)      do { printf("libmsmelp: "); printf(__VA_ARGS__); printf("\n"); } while (0)
#else
#define trace(...)      (void)0
#endif

/*
 * helper
 */
static int m2b(const char* mime) {      /* mime to bitrate */
    int brate = atoi(mime+4);
    switch (brate) {
        case  600:  return MELP_RATE_0600;
        case 1200:  return MELP_RATE_1200;
        case 2400:  return MELP_RATE_2400;
        default:    ms_fatal("bad MELP mime type %s", mime);
    }
}



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
    MSBufferizer* bufferizer;   /* bufferizer             */
    struct melp_analyzer* melp; /* melp analyzer          */
    fifo_t* fifo;               /* fifo                   */
    FILE* file;                 /* source (file or micro) */
    int brate;                  /* bit rate               */
    uint32_t timestamp;         /* timestamp              */
} encoder_data_t;

static void encoder_init(MSFilter* filter) {
    encoder_data_t* data = ms_new0(encoder_data_t, 1);

    data->bufferizer = ms_bufferizer_new();
    data->brate = m2b(filter->desc->enc_fmt);
    data->fifo = fifo_new(2*NSAMPLES*sizeof(int16_t));
    data->timestamp = 0;

    params_lock();
    params_t* p = params(data->brate);
    trace("[%s] npp=%s, file=%s", filter->desc->enc_fmt, p->npp ? "true" : "false", p->file);
    ms_message("[%s] npp=%s, file=%s", filter->desc->enc_fmt, p->npp ? "true" : "false", p->fin);
    melp_analyzer_new(&data->melp, data->brate, p->npp);
    data->file = p->fin ? fopen(p->fin, "r") : NULL;
    params_unlock();

    filter->data = data;
}

static void encoder_uninit(MSFilter* filter) {
    encoder_data_t* data = (encoder_data_t*)filter->data;
    if (data->file)
        fclose(data->file);
    fifo_free(data->fifo);
    melp_analyzer_free(data->melp);
    ms_bufferizer_destroy(data->bufferizer);
    ms_free(data);
}

static void encoder_process(MSFilter* filter) {
    encoder_data_t* data = (encoder_data_t*)filter->data;

    ms_filter_lock(filter);
    mblk_t* im;
    while (im = ms_queue_get(filter->inputs[0]))
        ms_bufferizer_put(data->bufferizer, im);

    int16_t buf[NSAMPLES];
    while (ms_bufferizer_read(data->bufferizer, (uint8_t*)buf, sizeof(buf)) == sizeof(buf)) {
        if (data->file) {   /* if file is available, replace input source by file content */
            size_t size = fread(buf, 1, sizeof(buf), data->file);
            if (size < sizeof(buf)) {
                rewind(data->file);
                size = fread(&buf[size/sizeof(buf[0])], 1, sizeof(buf)-size, data->file);
            }
        }

        fifo_push(data->fifo, buf, sizeof(buf));

        int bs = melp_get_bitstream_size(data->brate);
        int nf = melp_get_nb_frames(data->brate);
        int  n = (int)(fifo_count(data->fifo)/(FSIZE*nf));

        mblk_t* om = allocb(n*bs, 0);
        ms_bufferizer_fill_current_metas(data->bufferizer, om);
        mblk_set_timestamp_info(om, data->timestamp);
        data->timestamp += n*FLENGTH*nf;

        for (int i = 0; i < n; ++i) {
            int16_t frm[FLENGTH*nf];
            fifo_pop(data->fifo, frm, sizeof(frm));

            for (int f = 0; f < nf; ++f)
                melp_analyze(data->melp, &frm[FLENGTH*f], &om->b_wptr[bs*i]);
        }
        om->b_wptr += n*bs;
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
    struct melp_synthesizer* melp;  /* melp analyzer          */
    fifo_t* fifo;                   /* fifo                   */
    int brate;                      /* bit rate               */
    FILE* file;     /* sink (optional)           */
} decoder_data_t;

static void decoder_init(MSFilter* filter) {
    decoder_data_t* data = ms_new0(decoder_data_t, 1);

    data->brate = m2b(filter->desc->enc_fmt);
    data->fifo = fifo_new(2*NSAMPLES*sizeof(int16_t));

    params_lock();
    params_t* p = params(data->brate);
    trace("[%s] pf = %s", filter->desc->enc_fmt, p->pf ? "true" : "false");
    ms_message("[%s] pf = %s", filter->desc->enc_fmt, p->pf ? "true" : "false");
    melp_synthesizer_new(&data->melp, data->brate, p->pf);
    data->file = p->fout ? fopen(p->fout, "w") : NULL;
    params_unlock();

    filter->data = data;
}

static void decoder_uninit(MSFilter* filter) {
    decoder_data_t* data = (decoder_data_t*)filter->data;
    if (data->file)
        fclose(data->file);

    fifo_free(data->fifo);
    melp_synthesizer_free(data->melp);
    ms_free(data);
}

static void decoder_process(MSFilter* filter) {
    decoder_data_t* data = (decoder_data_t*)filter->data;

    mblk_t* im;
    while ((im = ms_queue_get(filter->inputs[0]))) {
        msgpullup(im, -1);

        int bs = melp_get_bitstream_size(data->brate);
        int nf = melp_get_nb_frames(data->brate);
        int  n = (im->b_wptr-im->b_rptr)/bs;

        for (int i = 0; i < n; ++i) {
            int16_t frm[FLENGTH*nf];

            for (int f = 0; f < nf; ++f)
                melp_synthesize(data->melp, &im->b_rptr[bs*i], &frm[FLENGTH*f]);
            fifo_push(data->fifo, frm, sizeof(frm));

            if (data->file)     /* if file is available, record frame */
                fwrite(frm, 1, sizeof(frm), data->file);
        }

        size_t sz = NSAMPLES*sizeof(int16_t);
        if (fifo_count(data->fifo) >= sz) {
            mblk_t* om = allocb(sz, 0);
            mblk_meta_copy(im, om);
            fifo_pop(data->fifo, om->b_wptr, sz);
            om->b_wptr += sz;
            ms_queue_put(filter->outputs[0], om);
        }

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
MSFilterDesc melp_0600_encoder_desc = { .id          = MS_FILTER_PLUGIN_ID,
                                        .name        = "melp_600_encoder",
                                        .text        = "MELP encoder at 600 bits/s",
                                        .category    = MS_FILTER_ENCODER,
                                        .enc_fmt     = "MELP600",
                                        .ninputs     = 1,
                                        .noutputs    = 1,
                                        .init        = encoder_init,
                                        .preprocess  = NULL,
                                        .process     = encoder_process,
                                        .postprocess = NULL,
                                        .uninit      = encoder_uninit,
                                        .methods     = encoder_methods,
                                        .flags       = 0 };
MSFilterDesc melp_1200_encoder_desc = { .id          = MS_FILTER_PLUGIN_ID,
                                        .name        = "melp_1200_encoder",
                                        .text        = "MELP encoder at 1200 bits/s",
                                        .category    = MS_FILTER_ENCODER,
                                        .enc_fmt     = "MELP1200",
                                        .ninputs     = 1,
                                        .noutputs    = 1,
                                        .init        = encoder_init,
                                        .preprocess  = NULL,
                                        .process     = encoder_process,
                                        .postprocess = NULL,
                                        .uninit      = encoder_uninit,
                                        .methods     = encoder_methods,
                                        .flags       = 0 };
MSFilterDesc melp_2400_encoder_desc = { .id          = MS_FILTER_PLUGIN_ID,
                                        .name        = "melp_2400_encoder",
                                        .text        = "MELP encoder at 2400 bits/s",
                                        .category    = MS_FILTER_ENCODER,
                                        .enc_fmt     = "MELP2400",
                                        .ninputs     = 1,
                                        .noutputs    = 1,
                                        .init        = encoder_init,
                                        .preprocess  = NULL,
                                        .process     = encoder_process,
                                        .postprocess = NULL,
                                        .uninit      = encoder_uninit,
                                        .methods     = encoder_methods,
                                        .flags       = 0 };
MS_FILTER_DESC_EXPORT(melp_0600_encoder_desc);
MS_FILTER_DESC_EXPORT(melp_1200_encoder_desc);
MS_FILTER_DESC_EXPORT(melp_2400_encoder_desc);

static MSFilterMethod decoder_methods[] = {{ MS_AUDIO_DECODER_HAVE_PLC, decoder_plc       },
                                           {   MS_FILTER_GET_NCHANNELS, decoder_nchannels },
                                           { MS_FILTER_GET_SAMPLE_RATE, decoder_srate     },
                                           {                         0, NULL              }};
MSFilterDesc melp_0600_decoder_desc = { .id          = MS_FILTER_PLUGIN_ID,
                                        .name        = "melp_600_decoder",
                                        .text        = "MELP decoder at 600 bits/s",
                                        .category    = MS_FILTER_DECODER,
                                        .enc_fmt     = "MELP600",
                                        .ninputs     = 1,
                                        .noutputs    = 1,
                                        .init        = decoder_init,
                                        .preprocess  = NULL,
                                        .process     = decoder_process,
                                        .postprocess = NULL,
                                        .uninit      = decoder_uninit,
                                        .methods     = decoder_methods,
                                        .flags       = 0 };
MSFilterDesc melp_1200_decoder_desc = { .id          = MS_FILTER_PLUGIN_ID,
                                        .name        = "melp_1200_decoder",
                                        .text        = "MELP decoder at 1200 bits/s",
                                        .category    = MS_FILTER_DECODER,
                                        .enc_fmt     = "MELP1200",
                                        .ninputs     = 1,
                                        .noutputs    = 1,
                                        .init        = decoder_init,
                                        .preprocess  = NULL,
                                        .process     = decoder_process,
                                        .postprocess = NULL,
                                        .uninit      = decoder_uninit,
                                        .methods     = decoder_methods,
                                        .flags       = 0 };
MSFilterDesc melp_2400_decoder_desc = { .id          = MS_FILTER_PLUGIN_ID,
                                        .name        = "melp_2400_decoder",
                                        .text        = "MELP decoder at 2400 bits/s",
                                        .category    = MS_FILTER_DECODER,
                                        .enc_fmt     = "MELP2400",
                                        .ninputs     = 1,
                                        .noutputs    = 1,
                                        .init        = decoder_init,
                                        .preprocess  = NULL,
                                        .process     = decoder_process,
                                        .postprocess = NULL,
                                        .uninit      = decoder_uninit,
                                        .methods     = decoder_methods,
                                        .flags       = 0 };
MS_FILTER_DESC_EXPORT(melp_0600_decoder_desc);
MS_FILTER_DESC_EXPORT(melp_1200_decoder_desc);
MS_FILTER_DESC_EXPORT(melp_2400_decoder_desc);

export void libmsmelp_init(MSFactory *factory) {
    ms_factory_register_filter(factory, &melp_0600_encoder_desc); ms_factory_register_filter(factory, &melp_0600_decoder_desc);
    ms_factory_register_filter(factory, &melp_1200_encoder_desc); ms_factory_register_filter(factory, &melp_1200_decoder_desc);
    ms_factory_register_filter(factory, &melp_2400_encoder_desc); ms_factory_register_filter(factory, &melp_2400_decoder_desc);
    ms_message("plugin libmsmelp v"VERSION" loaded");
}
