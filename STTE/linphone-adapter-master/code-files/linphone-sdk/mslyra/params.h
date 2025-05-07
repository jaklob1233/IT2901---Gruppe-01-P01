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
    char fin[256];
    char fout[256];
    int  noise_preprocessing;
    int  dtx;
    int  ptime;
} params_t;

int  params_init(void);
void params_uninit(void);
void params_lock(void);
void params_unlock(void);
params_t* params(int id);

#endif

