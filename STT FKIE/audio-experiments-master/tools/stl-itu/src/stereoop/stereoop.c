/*
stereoop.c
Sample program for stereo file operations

Compile in STL2005r1 , e.g. from utl directory use:
gcc stereoop.c  -I ../utl -o stereoop.exe
*/
/* revision history
v1.0 April 11, 2008:
 first version submitted to ITU-T, Jonas Svedberg Ericsson AB

v1.01 April 23, 2008:
  corrected Mono-downmix synthesis, Jonas Svedberg Ericsson AB

v1.02 Sept 4, 2009:
  added rounding to   Mono-downmix output, Yusuke Hiwasaki, NTT Cyber Space Labs
                                           Jonas Svedberg, Ericsson AB
v1.03 Feb  2, 2010:
  modified maximum filename length to avoid buffer overruns (y.hiwasaki)
*/

#include "ugstdemo.h"           /* general UGST definitions and DEFINES */
#include <stdio.h>              /* Standard I/O Definitions */
#include <stdio.h>              /* Standard I/O Definitions */
#include <math.h>
#include <stdlib.h>
#include <string.h>             /* memset, strcmp */

/* ..... Definitions used by the program ..... */
#define VERSION        "stereoop.c 1.03 Feb 2, 2010"
#define MAX_STR        1024
#define MAX_OFILES     2
#define MAX_IFILES     2
#define MAX16      32767
#define MIN16     -32768

enum Mode { NONE = -1,
  INTER,
  SPLIT,
  LEFT,
  RIGHT,
  MAXENVAL,
  MONO,
  N_MODES                       /* number of modes */
};

/* local functions */
/*-------------------------------------------------------------------------
display_usage(int level);  Shows program usage.
-------------------------------------------------------------------------*/
void display_usage (int level) {
  printf (VERSION);
  printf ("\n");
  if (level) {
    printf ("Program Description:\n");
    printf (" Program to perform basic stereo operations on 16 bit (short) files \n\n");
    printf ("\n");
  } else {
    printf ("Program to perform basic stereo operations \n\n");
  }

  printf ("Usage:\n");
  printf ("stereoop [option] file1 file2 [file3] \n");
  printf ("\n");
  printf ("Option:\n");
  printf (" -interleave. Compose a 2ch stereo file from left and right single channel files \n");
  printf (" -split...... Provide left and right channel files from a 2ch stereo file\n");
  printf (" -left....... Provide left channel from a 2ch stereo file\n");
  printf (" -right...... Provide right channel from a 2ch stereo file\n");
  printf (" -maxenval... Provide a single channel maximum energy level analysis file from a 2ch stereo file\n");
  printf (" -mono....... Provide mono downmix(M=(L+R)/2) single channel file from a 2ch stereo file\n");

  printf ("Stereoop operating examples\n");
  printf ("          option      input(s)             output(s):\n");
  printf ("-----------------------------------------------------------------------\n");
  printf (" stereoop -interleave FileL.1ch FileR.1ch  OutfileLR.2ch\n");
  printf (" stereoop -split      FileLR.2ch           OutfileL.1ch OutFileR.1ch\n");
  printf (" stereoop -left       FileLR.2ch           OutfileL.1ch\n");
  printf (" stereoop -right      FileLR.2ch           OutfileR.1ch\n");
  printf (" stereoop -maxenval   FileLR.2ch           Outfile.maxenval.1ch\n");
  printf (" stereoop -mono       FileLR.2ch           Outfile.mono.1ch\n");

  printf ("\n\n NB! If input files have inconsistent lengths, the program exits with a non-zero status value\n");

  /* Quit program */
  exit (-128);
}

char *mode_str[N_MODES] = {
  "interleave",
  "split",
  "left",
  "right",
  "maxenval",
  "mono"
};
short n_infiles[N_MODES] = { 2, 1, 1, 1, 1, 1 };
short n_outfiles[N_MODES] = { 1, 2, 1, 1, 1, 1 };

int main (int argc, char *argv[]) {
  FILE *Fif[MAX_IFILES];        /* Pointer to input files */
  FILE *Fof[MAX_OFILES];        /* Pointer to output files */
  char ifname[MAX_IFILES][MAX_STRLEN];  /* Input file names */
  char ofname[MAX_OFILES][MAX_STRLEN];  /* Output file names */
  char tmp_str[MAX_STR];
  long cnt_samples = 0;
  int quiet = 0;
  enum Mode mode = NONE;
  int i, n_in, n_out;
  short tmp_short[2], tmp_short_1ch[1];
  double tmp_f;

  /* Check options */
  if (argc < 3) {
    display_usage (1);
  } else {
    while (argc > 1 && argv[1][0] == '-')
      if ((strcmp (argv[1], "-interleave") == 0) && (mode < 0)) {
        mode = INTER;
        argc--;
        argv++;
      } else if ((strcmp (argv[1], "-split") == 0) && (mode < 0)) {
        mode = SPLIT;
        argc--;
        argv++;
      } else if ((strcmp (argv[1], "-left") == 0) && (mode < 0)) {
        mode = LEFT;
        argc--;
        argv++;
      } else if ((strcmp (argv[1], "-right") == 0) && (mode < 0)) {
        mode = RIGHT;
        argc--;
        argv++;
      } else if ((strcmp (argv[1], "-maxenval") == 0) && (mode < 0)) {
        mode = MAXENVAL;
        argc--;
        argv++;
      } else if ((strcmp (argv[1], "-mono") == 0) && (mode < 0)) {
        mode = MONO;
        argc--;
        argv++;
      } else if (strcmp (argv[1], "-q") == 0) {
        /* Set quiet mode */
        quiet = 1;
        argc--;
        argv++;
      } else if (strcmp (argv[1], "-h") == 0) {
        display_usage (0);
      } else if (strstr (argv[1], "-help")) {
        display_usage (1);
      } else {
        fprintf (stderr, "ERROR! Invalid option \"%s\" in command line\n\n", argv[1]);
        display_usage (0);
      }
  }
  if (mode == NONE) {
    fprintf (stderr, "ERROR! At least one option is required on command line\n\n");
    display_usage (1);
  }
  if ((argc) != (n_infiles[mode] + n_outfiles[mode] + 1)) {
    error_terminate ("Illegal number of files in command line \n", 1);
  }
  /* Get file parameters */
  for (i = 0; i < n_infiles[mode]; i++) {
    GET_PAR_S (1 + i, "_Input file ..................: ", ifname[i]);
  }
  for (i = 0; i < n_outfiles[mode]; i++) {
    GET_PAR_S (1 + n_infiles[mode] + i, "_Output file .................: ", ofname[i]);
  }

  if (!quiet) {
    printf ("stereoop option: %s\n", mode_str[mode]);
    for (i = 0; i < n_infiles[mode]; i++) {
      printf ("Input file %d..................: %s\n", i + 1, ifname[i]);
    }
    for (i = 0; i < n_outfiles[mode]; i++) {
      printf ("Output file %d.................: %s\n", i + 1, ofname[i]);
    }
  }

  /* Open files */
  for (i = 0; i < n_infiles[mode]; i++) {
    if ((Fif[i] = fopen (ifname[i], RB)) == NULL) {
      sprintf (tmp_str, "Could not open input file %d,(%s)\n", i + 1, ifname[i]);
      error_terminate (tmp_str, 1);
    }
  }
  for (i = 0; i < n_outfiles[mode]; i++) {
    if ((Fof[i] = fopen (ofname[i], WB)) == NULL) {
      sprintf (tmp_str, "Could not create output file %d,(%s)\n", i + 1, ofname[i]);
      error_terminate (tmp_str, 1);
    }
  }

  /* start of actual operation */
  memset (tmp_short, 0, sizeof (short) * 2);
  if (mode == INTER) {          /* read two mono file samples */
    while ((n_in = fread (tmp_short, sizeof (short), 1, Fif[0])) == 1) {
      if ((n_in = fread (&tmp_short[1], sizeof (short), 1, Fif[1])) != 1) {
        error_terminate ("Error, 1ch input file 2(right), shorter than 1ch input file 1(Left)\n", 1);
      }
      if ((n_out = fwrite (tmp_short, sizeof (short), 2, Fof[0])) != 2) {
        error_terminate ("Error, could not write to 2ch output stereo file \n", 1);
      }
      cnt_samples++;
      memset (tmp_short, 0, sizeof (short) * 2);
    }                           /* while (1ch,1ch)->2ch */

    /* check if samples are still available in right channel */
    if ((n_in = fread (&tmp_short[1], sizeof (short), 1, Fif[1])) != 0) {
      error_terminate ("Error, 1ch input file 1(Left), shorter than 1ch input file 2(right)!\n", 1);
    }
  } else {
    /* reading of 2ch stereo file input samples */
    while ((n_in = fread (tmp_short, sizeof (short), 2, Fif[0])) == 2) {

      if (mode == SPLIT) {
        if ((n_out = fwrite (tmp_short, sizeof (short), 1, Fof[0])) != 1) {
          error_terminate ("Error, could not write to 1ch output file 1 (Left)\n", 1);
        }
        if ((n_out = fwrite (&tmp_short[1], sizeof (short), 1, Fof[1])) != 1) {
          error_terminate ("Error, could not write to 1ch output file 2 (Right)\n", 1);
        }
      } else {                  /* all 2ch to 1ch options */
        tmp_short_1ch[0] = 0;
        switch (mode) {
        case LEFT:
          tmp_short_1ch[0] = tmp_short[0];
          break;
        case RIGHT:
          tmp_short_1ch[0] = tmp_short[1];
          break;
        case MAXENVAL:         /* select value(sample) with maximum energy */
          if (abs (tmp_short[0]) > abs (tmp_short[1])) {
            tmp_short_1ch[0] = tmp_short[0];
          } else {
            tmp_short_1ch[0] = tmp_short[1];
          }
          break;
        case MONO:             /* M=(L+R)/2, with 16 bit saturation and rounding */
          tmp_f = ((double) tmp_short[0] + (double) tmp_short[1]) * 0.5;
          tmp_f += ((tmp_f > 0) ? 0.5 : -0.5);  /* rounding */

          if (tmp_f >= (double) MAX16) {
            tmp_short_1ch[0] = MAX16;
          } else if (tmp_f <= (double) MIN16) {
            tmp_short_1ch[0] = MIN16;
          } else {
            tmp_short_1ch[0] = (short) tmp_f;
          }
          break;
        default:
          error_terminate ("Error, illegal mode option\n", 1);
          break;
        }
        if ((n_out = fwrite (tmp_short_1ch, sizeof (short), 1, Fof[0])) != 1) {
          error_terminate ("Error, could not write to 1ch output file 1\n", 1);
        }
      }
      cnt_samples++;
      memset (tmp_short, 0, sizeof (short) * 2);
    }                           /* while 2ch input */

    /* check consistecy of 2ch stereo file reading */
    if (n_in != 0) {
      error_terminate ("Error, 2ch input file has odd number of samples !! \n", 1);
    }
  }                             /* mode=INTER */

  /* finalization */
  if (!quiet) {
    fprintf (stdout, "\n---\n  Finished stereoop (-%s) \n", mode_str[mode]);
    fprintf (stdout, "(Total %ld samples processed)\n\n", cnt_samples);
  }
  for (i = 0; i < n_infiles[mode]; i++) {
    fclose (Fif[i]);
  }
  for (i = 0; i < n_outfiles[mode]; i++) {
    fclose (Fof[i]);
  }
  return 0;
}
