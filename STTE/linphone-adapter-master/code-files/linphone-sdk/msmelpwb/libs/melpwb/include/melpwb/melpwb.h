#ifndef MELPWB_H
#define MELPWB_H

#include <stdbool.h>
#include <stdint.h>

#define export          __attribute__((visibility("default")))
#define MELPWB_NOMEM    -1
#define MELPWB_RATE     -2

enum melpwb_npp_type {
    MELPWB_NO_NPP,
    MELPWB_NPP
};

enum melpwb_postfilter_type {
    MELPWB_NO_POSTFILTER,
    MELPWB_POSTFILTER
};

enum melpwb_rate {
    MELPWB_RATE_600_UNUSED,
    MELPWB_RATE_1200_UNUSED,
    MELPWB_RATE_1200,
    MELPWB_RATE_2400_UNUSED
};

export int  melpwb_analyzer_new(void** inst, enum melpwb_rate rate, enum melpwb_npp_type npp);
export void melpwb_analyzer_free(void* inst);
export void melpwb_analyzer_reset(void* inst);
export int  melpwb_analyze(void* inst, const int16_t* speech, uint8_t* bitstream);

export int  melpwb_synthesizer_new(void** inst, enum melpwb_rate rate, enum melpwb_postfilter_type pf);
export void melpwb_synthesizer_free(void* inst);
export void melpwb_synthesizer_reset(void* inst);
export int  melpwb_synthesize(void* inst, const uint8_t* bitstream, int16_t* speech);

#endif

