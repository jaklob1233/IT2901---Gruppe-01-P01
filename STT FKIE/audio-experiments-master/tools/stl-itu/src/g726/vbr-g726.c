/*                                                           03.Feb.2010 v1.4
  ============================================================================

  VBR-G726.C
  ~~~~~~~~~~

  Description:
  ~~~~~~~~~~~~

  Demonstration program for UGST/ITU-T G.726 module using the variable
  bit rate feature. This version accepts either linear or G.711 A/u-law
  input. Since this implementation of the G.726 requires G.711 compressed
  samples, linear samples are converted to G.711 format before being
  processed. Therefore, the same ammount of quantization distortion should
  be expect either way.

  The modules called have been originally written in Fortran, and were
  translated into C by the converter f2c, version of October 15, 1990
  at 19:58:17.

  Input data is supposed to be aligned at word boundaries, i.e.,
  organized in 16-bit words, following the operating system normal
  organization (low-byte first for VMS and DOS; high byte first for most
  Unix systems). Linear samples are supposed to be 16-bit right-adjusted.
  G711 compressed data is supposed to be in the 8 LEAST
  significant bits of the word and the ADPCM data is in the LEAST 5
  bits. Both are without sign extension.

  Output data will be generated in the same format as decribed above for
  the input data.

  Usage:
  ~~~~~~
  $ VBR-G726 [-options] InpFile OutFile
             [FrameSize [1stBlock [NoOfBlocks [Reset]]]]
  where:
  InpFile     is the name of the file to be processed;
  OutFile     is the name with the processed data;
  FrameSize   is the frame size, in number of samples; the bitrate
              will only change in the boundaries of a frame
              [default: 16 samples]
  1stBlock    is the number of the first block of the input file
              to be processed [default: 1st block]
  NoOfBlocks  is the number of blocks to be processed, starting on
    	      block "1stBlock" [default: all blocks]

  Options:
  -law #      the letters A or a for G.711 A-law, letter u for
              G.711 u-law, or letter l for linear. If linear is
	      chosen, A-law is used to compress/expand samples to/from
	      the G.726 routines. Default is A-law.
  -rate #     is the bit-rate (in kbit/s): 40, 32, 24 or 16 (in kbit/s);
              or a combination of them using dashes (e.g. 32-24 or
	      16-24-32). Default is 32 kbit/s.
  -frame #    Number of samples per frame for switching bit rates.
              Default is 16 samples (or 2ms)
  -enc        run only the G.726 encoder on the samples
              [default: run encoder and decoder]
  -dec        run only the G.726 decoder on the samples
              [default: run encoder and decoder]
  -noreset    don't apply reset to the encoder/decoder
  -?/-help    print help message

  Example:
  $ vbr-G726 -law u -enc -rate 32 voice.ref voice.adp 256 3 45

  The command above takes the samples in file "voice.ref", already
  in mu law format, processes the data through the G726 encoder
  only at a rate of 32 bkit/s, saving them into the file
  "voice.rel". The processing starts at block 3 for 45 blocks,
  each block being 256 samples wide.

  Variables:
  ~~~~~~~~~~
  law  	  law to use (either A or u)
  conv    desired processing
  rate    desired rate
  inpfil  input file name;
  outfil  output file name;
  N  	  block size;
  N1  	  first block to process;
  N2  	  no. of blocks to process;

  Exit values:
  ~~~~~~~~~~~~
  0  success (all but VMS);
  1  success (only in VMS);
  2  error opening input file;
  3  error creating output file;
  4  error moving pointer to desired start of conversion;
  5  error reading input file;
  6  error writing to file;
  7  invalid law
  8  invalid conversion
  9  invalid rate

  Original author:
  ~~~~~~~~~~~~~~~~
  Simao Ferraz de Campos Neto
  Comsat Laboratories                  Tel:    +1-301-428-4516
  22300 Comsat Drive                   Fax:    +1-301-428-9287
  Clarksburg MD 20871 - USA            E-mail: simao@ctd.comsat.com

  History:
  ~~~~~~~~
  10/Mar/1995 v1.0  Created based on vbr-g726.c
  22/Feb/1996 v1.1  Removed compilation warnings, included headers as
                    suggested by Kirchherr (FI/DBP Telekom) to run under
		    OpenVMS/AXP <simao@ctd.comsat.com>
  22/Jul/1997 v1.2  Changed static allocation for data arrays to allow longer
                    frame sizes. Fix based on modifications by R. Kirchherr
                    <kirchherr@tzd.telekom.de>
  21.Mar.2000 v1.3  Corrected bug when the bitrate is not specified by
                    the user. Corrected bug that made incorrect
                    calculation on total number of blocks to process
                    when the block size is not a multiple of the file
                    size. <simao.campos@labs.comsat.com>
  02.Feb.2010 v1.4  Modified maximum string length (y.hiwasaki)
  ============================================================================
*/

/* ..... General definitions for UGST demo programs ..... */
#include "ugstdemo.h"

/* ..... General include ..... */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#if defined(VMS)
#include <unixio.h>
#include <stat.h>
#else /* Unix/DOS */
#include <sys/stat.h>
#endif

/* ..... G.726 module as include functions ..... */
#include "g726.h"
#include "g711.h"

/*
 -------------------------------------------------------------------------
 void parse_rate(char *str, short *rate);
 ~~~~~~~~~~~~~~~
 Parses string str with a list of bitrates for the operation of the G726
 algorithm and return a list of them in rate, and the number of rates read.

 Parameters:
 ~~~~~~~~~~~
 str ..... string pointing to a list of dash-separated bitrates to be used.
           Valid examples are: 16 (single rate), 32-24 (duo-rate), etc.
 rate .... string of short numbers with each of the specified rates.

 Return:
 ~~~~~~~
 Returns the number of bitrates for the ADPCM operation, 0 if str is
 empty or improper, and -1 if there are too many rates requested.

 History:
 ~~~~~~~~
 10.Mar.95 v1.0 Created <simao@ctd.comsat.com>
 -------------------------------------------------------------------------
*/
int parse_rate (char *str, short **rate) {
  char *s = str;
  int count = 1, i;

  if (str == NULL)
    return 0;

  while ((s = strchr (s, '-')) != NULL) {
    s++;
    count++;
  }

  /* Allocates memory for the necessary number of rates */
  *rate = (short *) calloc (sizeof (short), count);
  if (*rate == NULL)
    return (-1);

  /* Save rates in the array */
  for (s = strtok (str, "-"), i = 0; i < count; i++) {
    /* Convert to short & save */
    (*rate)[i] = atoi (s);

    /* Classification of rate - return 0 if invalid rate was specified */
    if ((*rate)[i] > 5) {
      if ((*rate)[i] == 40)
        (*rate)[i] = 5;
      else if ((*rate)[i] == 32)
        (*rate)[i] = 4;
      else if ((*rate)[i] == 24)
        (*rate)[i] = 3;
      else if ((*rate)[i] == 16)
        (*rate)[i] = 2;
      else
        return (0);
    }

    /* Update s to the next valid rate number */
    s = strtok (NULL, "-");
  }

  /* Return the number of rates */
  return (count);
}

/* .................... End of parse_rate() ........................... */


/*
 -------------------------------------------------------------------------
 void display_usage(void);
 ~~~~~~~~~~~~~~~~~~
 Display proper usage for the demo program. Generated automatically from
 program documentation.

 History:
 ~~~~~~~~
 10.Mar.95 v1.0 Created <simao>.
 -------------------------------------------------------------------------
*/
void display_usage () {
  printf ("Version 1.3 of 21/Mar/2000 \n\n");

  printf ("  VBR-G726.C \n");
  printf ("  Demonstration program for UGST/ITU-T G.726 module using the variable\n");
  printf ("  bit rate feature. This version accepts either linear or G.711 A/u-law\n");
  printf ("  input. Since this implementation of the G.726 requires G.711 compressed\n");
  printf ("  samples, linear samples are converted to G.711 format before being\n");
  printf ("  processed. Therefore, the same ammount of quantization distortion should\n");
  printf ("  be expect either way.\n");
  printf ("  Input data is supposed to be aligned at word boundaries, i.e.,\n");
  printf ("  organized in 16-bit words, following the operating system normal\n");
  printf ("  organization (low-byte first for VMS and DOS; high byte first for most\n");
  printf ("  Unix systems). Linear samples are supposed to be 16-bit right-adjusted. \n");
  printf ("  G711 compressed data is supposed to be in the 8 LEAST\n");
  printf ("  significant bits of the word and the ADPCM data is in the LEAST 5\n");
  printf ("  bits. Both are without sign extension.\n");
  printf ("  \n");
  printf ("  Output data will be generated in the same format as decribed above for\n");
  printf ("  the input data.\n");
  printf ("  \n");
  printf ("  Usage:\n");
  printf ("  VBR-G726 [-options] InpFile OutFile \n");
  printf ("             [FrameSize [1stBlock [NoOfBlocks [Reset]]]]\n");
  printf ("  where:\n");
  printf ("  InpFile     is the name of the file to be processed;\n");
  printf ("  OutFile     is the name with the processed data;\n");
  printf ("  FrameSize   is the frame size, in number of samples; the bitrate \n");
  printf ("              will only change in the boundaries of a frame \n");
  printf ("              [default: 16 samples]\n");
  printf ("  1stBlock    is the number of the first block of the input file\n");
  printf ("              to be processed [default: 1st block]\n");
  printf ("  NoOfBlocks  is the number of blocks to be processed, starting on\n");
  printf ("              block \"1stBlock\" [default: all blocks]\n");
  printf ("\n");
  printf ("  Options:\n");
  printf ("  -law #      the letters A or a for G.711 A-law, letter u for \n");
  printf ("              G.711 u-law, or letter l for linear. If linear is\n");
  printf ("              chosen, A-law is used to compress/expand samples to/from\n");
  printf ("              the G.726 routines. Default is A-law.\n");
  printf ("  -rate #     is the bit-rate (in kbit/s): 40, 32, 24 or 16 (in kbit/s); \n");
  printf ("              or a combination of them using dashes (e.g. 32-24 or\n");
  printf ("              16-24-32). Default is 32 kbit/s.\n");
  printf ("  -frame #    Number of samples per frame for switching bit rates.\n");
  printf ("              Default is 16 samples (or 2ms) \n");
  printf ("  -enc        run only the G.726 encoder on the samples \n");
  printf ("              [default: run encoder and decoder]\n");
  printf ("  -dec        run only the G.726 decoder on the samples \n");
  printf ("              [default: run encoder and decoder]\n");
  printf ("  -noreset    don't apply reset to the encoder/decoder\n");
  printf ("  -?/-help    print help message\n\n");

  /* Quit program */
  exit (-128);
}

/* .................... End of display_usage() ........................... */


/*
   **************************************************************************
   ***                                                                    ***
   ***        Demo-Program for testing the correct implementation         ***
   ***               and to show how to use the programs                  ***
   ***                                                                    ***
   **************************************************************************
*/
int main (int argc, char *argv[]) {
  G726_state encoder_state, decoder_state;
  long N = 16, N1 = 1, N2 = 0, cur_blk, smpno;
  short *tmp_buf, *inp_buf, *out_buf, reset = 1;
  short inp_type, out_type, *rate = 0;
  char encode = 1, decode = 1, law[4] = "A", def_rate[] = "32";
  int rateno = 1, rate_idx;

  /* General-purpose, progress indication */
  static char quiet = 0, funny[9] = "|/-\\|/-\\";

/* File variables */
  char FileIn[MAX_STRLEN], FileOut[MAX_STRLEN];
  FILE *Fi, *Fo;
  int inp, out;
  long start_byte;
#ifdef VMS
  char mrs[15];
#endif

/*
 * ......... PARAMETERS FOR PROCESSING .........
 */

  /* GETTING OPTIONS */

  if (argc < 2)
    display_usage ();
  else {
    while (argc > 1 && argv[1][0] == '-')
      if (strcmp (argv[1], "-noreset") == 0) {
        /* No reset */
        reset = 0;

        /* Update argc/argv to next valid option/argument */
        argv++;
        argc--;
      } else if (strcmp (argv[1], "-enc") == 0) {
        /* Encoder-only operation */
        encode = 1;
        decode = 0;

        /* Move argv over the option to the next argument */
        argv++;
        argc--;
      } else if (strcmp (argv[1], "-dec") == 0) {
        /* Decoder-only operation */
        encode = 0;
        decode = 1;

        /* Move argv over the option to the next argument */
        argv++;
        argc--;
      } else if (strcmp (argv[1], "-law") == 0) {
        /* Define law for operation: A, u, or linear */
        switch (toupper ((int) argv[2][0])) {
        case 'A':
          law[0] = '1';
          break;
        case 'U':
          law[0] = '0';
          break;
        case 'L':
          law[0] = '2';
          break;
        default:
          error_terminate (" Invalid law (A or u)! Aborted...\n", 7);
        }

        /* Move argv over the option to the next argument */
        argv += 2;
        argc -= 2;
      } else if (strcmp (argv[1], "-frame") == 0) {
        /* Define Frame size for rate change during operation */
        N = atoi (argv[2]);

        /* Move argv over the option to the next argument */
        argv += 2;
        argc -= 2;
      } else if (strcmp (argv[1], "-rate") == 0) {
        /* Define rate(s) for operation */
        rateno = parse_rate (argv[2], &rate);
        if (rateno == 0) {
          fprintf (stderr, "Invalid bitrate list: %s\n", argv[2]);
          exit (9);
        }
        /* Move argv over the option to the next argument */
        argv += 2;
        argc -= 2;
      } else if (strcmp (argv[1], "-q") == 0) {
        /* Don't print progress indicator */
        quiet = 1;

        /* Move argv over the option to the next argument */
        argv++;
        argc--;
      } else if (strcmp (argv[1], "-?") == 0 || strcmp (argv[1], "-help") == 0) {
        /* Print help */
        display_usage ();
      } else {
        fprintf (stderr, "ERROR! Invalid option \"%s\" in command line\n\n", argv[1]);
        display_usage ();
      }
  }

  /* Now get regular parameters */
  GET_PAR_S (1, "_Input File: .................. ", FileIn);
  GET_PAR_S (2, "_Output File: ................. ", FileOut);
  FIND_PAR_L (3, "_Block Size: .................. ", N, N);
  FIND_PAR_L (4, "_Starting Block: .............. ", N1, N1);
  FIND_PAR_L (5, "_No. of Blocks: ............... ", N2, N2);

  /* Uses default rate if none is given */
  if (rate == 0)
    rateno = parse_rate (def_rate, &rate);

  /* Inform user of the compading law used: 0->u, 1->A, 2->linear */
  fprintf (stderr, "Using %s\n", law[0] == '1' ? "A-law" : (law[0] == '0' ? "u-law" : "linear PCM"));

  /* Find starting byte in file */
  start_byte = sizeof (short) * (long) (--N1) * (long) N;

  /* Check if is to process the whole file */
  if (N2 == 0) {
    struct stat st;

    /* ... find the input file size ... */
    stat (FileIn, &st);
    N2 = ceil ((st.st_size - start_byte) / (double) (N * sizeof (short)));
  }

  /* Define correct data I/O types */
  if (encode && decode) {
    inp_type = out_type = (law[0] == '2' ? IS_LIN : IS_LOG);
  } else if (encode) {
    inp_type = law[0] == '2' ? IS_LIN : IS_LOG;
    out_type = IS_ADPCM;
  } else {
    inp_type = IS_ADPCM;
    out_type = law[0] == '2' ? IS_LIN : IS_LOG;
  }

  /* Force law to be used *by the ADPCM* to A-law, if input is linear */
  if (law[0] == '2')
    law[0] = '1';

/*
 * ...... MEMORY ALLOCATION .........
 */

  if ((inp_buf = (short *) calloc (N, sizeof (short))) == NULL)
    error_terminate ("Error in memory allocation!\n", 1);
  if ((out_buf = (short *) calloc (N, sizeof (short))) == NULL)
    error_terminate ("Error in memory allocation!\n", 1);
  if ((tmp_buf = (short *) calloc (N, sizeof (short))) == NULL)
    error_terminate ("Error in memory allocation!\n", 1);

/*
 * ......... FILE PREPARATION .........
 */

  /* Opening input file; abort if there's any problem */
  if ((Fi = fopen (FileIn, "rb")) == NULL)
    KILL (FileIn, 2);
  inp = fileno (Fi);

  /* Creates output file */
#ifdef VMS
  sprintf (mrs, "mrs=%d", 512);
#endif
  if ((Fo = fopen (FileOut, WB)) == NULL)
    KILL (FileOut, 3);
  out = fileno (Fo);

  /* Move pointer to 1st block of interest */
  if (fseek (Fi, start_byte, 0) < 0l)
    KILL (FileIn, 4);

/*
 * ......... PROCESSING ACCORDING TO ITU-T G.726 .........
 */
  /* Reset VBR counters */
  rate_idx = 0;

  for (cur_blk = 0; cur_blk < N2; cur_blk++) {
    /* Set the proper rate index */
    rate_idx = cur_blk % rateno;

    /* Print progress flag */
    if (!quiet)
#ifdef DISPLAY_CURRENT_RATE
      fprintf (stderr, "%d-", 8 * rate[rate_idx]);
#else
      fprintf (stderr, "%c\r", funny[cur_blk % 8]);
#endif

    /* Read a block of samples */
    if ((smpno = fread (inp_buf, sizeof (short), N, Fi)) < 0)
      KILL (FileIn, 5);

    /* Compress linear input samples */
    if (inp_type == IS_LIN) {
      /* Compress using A-law */
      alaw_compress (smpno, inp_buf, tmp_buf);

      /* copy temporary buffer over input buffer */
      memcpy (inp_buf, tmp_buf, sizeof (short) * smpno);
    }

    /* Check if reset is needed */
    reset = (reset == 1 && cur_blk == 0) ? 1 : 0;

    /* Carry out the desired operation */
    if (encode && !decode)
      G726_encode (inp_buf, out_buf, smpno, law, rate[rate_idx], reset, &encoder_state);
    else if (decode && !encode)
      G726_decode (inp_buf, out_buf, smpno, law, rate[rate_idx], reset, &decoder_state);
    else if (encode && decode) {
      G726_encode (inp_buf, tmp_buf, smpno, law, rate[rate_idx], reset, &encoder_state);
      G726_decode (tmp_buf, out_buf, smpno, law, rate[rate_idx], reset, &decoder_state);
    }

    /* Expand linear input samples */
    if (out_type == IS_LIN) {
      /* Compress using A-law */
      alaw_expand (smpno, out_buf, tmp_buf);

      /* copy temporary buffer over input buffer */
      memcpy (out_buf, tmp_buf, sizeof (short) * smpno);
    }

    /* Write ADPCM output word */
    if ((smpno = fwrite (out_buf, sizeof (short), smpno, Fo)) < 0)
      KILL (FileOut, 6);
  }

/*
 * ......... FINALIZATIONS .........
 */

  /* Free allocated memory */
  free (tmp_buf);
  free (out_buf);
  free (inp_buf);
  free (rate);

  /* Close input and output files */
  fclose (Fi);
  fclose (Fo);

  /* Exit with success for non-vms systems */
#ifndef VMS
  return (0);
#endif
}

/* ............................. end of main() ............................. */
