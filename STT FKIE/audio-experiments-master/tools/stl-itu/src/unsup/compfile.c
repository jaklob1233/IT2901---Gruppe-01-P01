/*                                                          v2.2 - 04/Jan/1999
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

       COMPFILE.C

       Description:
       ~~~~~~~~~~~~
       Program for comparing two files and displaying the samples that
       are different on the screen. Supported data types are short (in both
       decimal and hex formats) and float.

       Usage:
       ~~~~~~
       $ COMPFILE [-delay] [-h] [-d] [-float] [-double] [-short] [-long]
                  file1 file2
                  [BlockSize [1stBlock [NoOfBlocks [TypeOfDump [DataType]]]]]
       where:
        file1       is the first file name;
        file2       is the second file name;
        BlockSize   is the Block size, in samples;
        1stBlock    is the stating block;
        NoOfBlocks  the number of blocks to be displayed;
        TypeOfDump  whether the dump is decimal [d] or hexa [H];
        DataType    if the file contains integer [I] or real [R] data;

       Options:
	-q           operate quietly - only report total no.of differences
	-delay d     is the number of samples to delay. If d>0, the first
		     file should be delayed in relation to the second
                     file (samples from file 1 should be skipped). If d<0,
		     the 2nd file is delayed (i.e., samples are skipped).
        -h           set dump in hex mode - valid for integer data types
        -i           set dump in decimal mode for integer data types [default]
        -float       display float numbers
        -double      display double numbers
	    -short       display short numbers [default]
        -long        display long numbers
        -reltol X    Relative diff tolerance X (double format)
        -abstol X    Absolute diff tolerance X (format needs to match input)

       Compilation:
       ~~~~~~~~~~~~
       Sun:  cc -o cf compfile.c # K&R C, or [ok]
             acc -o cf compfile.c # ANSI C   [ok]
             gcc -fno-builtin -o cf compfile.c # gnu impl.of the ANSI C [ok]
       vaxc: cc compfile.c /obj=cf               !
             link cf                             ! [ok]
             cf:==$'f$environment("default")'cf  !
       tcc:  tcc compfile.c


       Original Author:
       ~~~~~~~~~~~~~~~~
       Simao Ferraz de Campos Neto
       DDS/Pr11                      Tel: +55-192-39-1396
       CPqD/Telebras                 Fax: +55-192-53-4754
       13088-061 Campinas SP Brazil  E-mail: <simao@cpqd.ansp.br>

       History:
       ~~~~~~~~
       08/Mar/1990  v1.0  Created
       29/Dec/1993  v2.0  Added options and UGST header and support to float
                          data files <simao>
       22/Feb/1996  v2.1  Removed compilation warnings, included headers as
                          suggested by Kirchherr (FI/DBP Telekom) to run under
			  OpenVMS/AXP <simao@ctd.comsat.com>
       06/Jan/1999  v2.2  Updated help message for delay option, corrected
                          possibly wrong reported number of samples
                          compared.
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#include "ugstdemo.h"

/* OS definition */
#if defined(__MSDOS__) && !defined(MSDOS)
#define MSDOS
#endif

/* includes in general */
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


/* ... Includes for O.S. specific headers ... */
#if defined(MSDOS)
#include <fcntl.h>
#include <io.h>                 /* For read(), write(), lseek() */
#include <sys\stat.h>
#elif defined(VMS)
#include <perror.h>
#include <file.h>
#include <stat.h>
#include <unixio.h>             /* For read(), write(), lseek() */
#else /* Unix */
#include <unistd.h>             /* For read(), write(), lseek() */
#include <sys/stat.h>
#endif

/* definitions for the program itself */
#define PRINT_RULE { int K; for (K=0;K<4;K++) printf("-------------------");}
#define PRINT_RULE2 { int K; for (K=0;K<4;K++) printf("===================");}
#define CR	   printf("\n");
#define RELATIVE_DIFF(a,b)  ((a == 0 || b == 0) ? 1 : fabs(1.0-(double)a/(double)b))

/* Function prototypes */
void display_usage ARGS ((void));
long compare_doubles ARGS ((char *File1, char *File2, int fh1, int fh2, long N, long N1, long N2, char quiet, double reltol, double abstol));
long compare_floats ARGS ((char *File1, char *File2, int fh1, int fh2, long N, long N1, long N2, char quiet, double reltol, float abstol));
long compare_longs ARGS ((char *File1, char *File2, int fh1, int fh2, long N, long N1, long N2, char KindOfDump, char quiet, double reltol, long abstol));
long compare_shorts ARGS( (char *File1, char *File2, int fh1, int fh2, long N, long N1, long N2, char KindOfDump, char quiet, double reltol, short abstol) );


/*
 * --------------------------------------------------------------------------
 * ... Display usage of program ...
 *     Simao 29.Dec.93
 * --------------------------------------------------------------------------
 */
void display_usage () {
  fprintf (stderr, "*********** compfile [cf] Version 2.2 of 05/Jan/1999 **********\n");
  fprintf (stderr, "Program for comparing two files and displaying the samples that\n");
  fprintf (stderr, "are different at the screen as integers or hex numbers; the\n");
  fprintf (stderr, "option for dumping float files has not been implemented yet.\n");
  fprintf (stderr, "\n");
  fprintf (stderr, "Usage:\n");
  fprintf (stderr, "$ cf [-q] [-delay] [-h] [-d] [-float] [-double] [-short] [-long] file1 \n");
  fprintf (stderr, "\tfile2 [BlockSize [1stBlock [NoOfBlocks [TypeOfDump [DataType]]]]]\n");
  fprintf (stderr, "Where:\n");
  fprintf (stderr, " file1       is the first file name;\n");
  fprintf (stderr, " file2       is the second file name;\n");
  fprintf (stderr, " BlockSize   is the Block size, in samples;\n");
  fprintf (stderr, " 1stBlock    is the stating block;\n");
  fprintf (stderr, " NoOfBlocks  the number of blocks to be displayed;\n");
  fprintf (stderr, " TypeOfDump  whether the dump is decimal [D] or hexa[H];\n");
  fprintf (stderr, " DataType    [I] for short and [R] float data\n\n");

  fprintf (stderr, "Options:\n");
  fprintf (stderr, " -q           operate quietly - only report total no.of differences\n");
  fprintf (stderr, " -delay d     is the number of samples to delay. If d>0, the first\n");
  fprintf (stderr, "              file should be delayed in relation to the second \n");
  fprintf (stderr, "              file (samples from file 1 should be skipped). If d<0, \n");
  fprintf (stderr, "              the 2nd file is delayed (i.e., samples are skipped).\n");
  fprintf (stderr, " -h           set dump in hex mode - valid for integer data types\n");
  fprintf (stderr, " -d           set dump in decimal mode for integer data types [default]\n");
  fprintf (stderr, " -float       display float numbers\n");
  fprintf (stderr, " -double      display double numbers\n");
  fprintf (stderr, " -short       display short numbers [default]\n");
  fprintf (stderr, " -long        display long numbers\n");
  fprintf( stderr, " -reltol X    Relative diff tolerance X (double format)\n" );
  fprintf( stderr, " -abstol X    Absolute diff tolerance X (format needs to match input)\n" );

  /* Quit program */
  exit (-128);
}

/* ....................... end of display_usage() ...........................*/

/*
  -------------------------------------------------------------------------
  long compare_floats(char *File1, char *File2, int fh1, int fh2,
		      long N, long N1, long N2, char quiet);

  Compare float data from 2 files and print different samples indicating
  block/sample no.

  Return the number of different samples OR -1 on error

  30.Dec.93  v1.0  Simao
  --------------------------------------------------------------------------
*/
long compare_floats (char *File1, char *File2, int fh1, int fh2, long N, long N1, long N2, char quiet, double reltol, float abstol) {
  long i, j, l, k, NrDiffs;
  char c;
  float *a, *b;

  /* Allocate memory for data vectors */
  if ((a = (float *) calloc (N, sizeof (float))) == NULL)
    return -1;
  if ((b = (float *) calloc (N, sizeof (float))) == NULL) {
    free (a);
    return -1;
  }

  /* Start loop */
  for (c = 0, NrDiffs = i = j = 0; i < N2; i++, j = 0) {
    if (!(c == 'Q' || c == 'X' || c == 27 || c == 'S')) {
      if ((l = read (fh1, a, sizeof (float) * N) / sizeof (float)) > 0 && (k = read (fh2, b, sizeof (float) * N) / sizeof (float)) > 0)
        while (j < l && j < k) {
          if( fabs( a[j] - b[j] ) > abstol && RELATIVE_DIFF(a[j],b[j]) > reltol ) {
            if (!quiet) {
              if (NrDiffs++ % 22 == 0) {
                CR;
                PRINT_RULE;
                printf ("\n Float compare of %s and %s\n", File1, File2);
                PRINT_RULE;
              }
              printf ("\nBLOCK: %6ld, SAMPLE: %4ld ===>", i + N1, j + 1);
              printf ("\t%12.6E\t%12.6E\t[Diff=%+5.2E]", a[j], b[j], a[j] - b[j]);
              if (NrDiffs % 22 == 0)
                if ((c = toupper (getchar ())) == 'Q' || c == 'X' || c == 27 || c == 'S')
                  break;
            } else
              NrDiffs++;
          }
          j++;
      } else {
        if (l < 0) {
          KILL (File1, 5);
        } else if (k < 0) {
          KILL (File2, 6);
        } else
          break;
      }
    }
  }
  if (NrDiffs > 0 && !quiet)
    CR;

  /* Release allocated memory */
  free (a);
  free (b);

  /* Return the number of different samples */
  return (NrDiffs);
}

/* ...................... end of compare_floats() ..........................*/


/*
  -------------------------------------------------------------------------
  long compare_doubles(char *File1, char *File2, int fh1, int fh2,
		      long N, long N1, long N2, char quiet);

  Compare double data from 2 files and print different samples indicating
  block/sample no.

  Return the number of different samples OR -1 on error

  30.Dec.93  v1.0  Simao
  --------------------------------------------------------------------------
*/
long compare_doubles (char *File1, char *File2, int fh1, int fh2, long N, long N1, long N2, char quiet, double reltol, double abstol ) {
  long i, j, l, k, NrDiffs;
  char c;
  double *a, *b;

  /* Allocate memory for data vectors */
  if ((a = (double *) calloc (N, sizeof (double))) == NULL)
    return -1;
  if ((b = (double *) calloc (N, sizeof (double))) == NULL) {
    free (a);
    return -1;
  }

  /* Start loop */
  for (c = 0, NrDiffs = i = j = 0; i < N2; i++, j = 0) {
    if (!(c == 'Q' || c == 'X' || c == 27 || c == 'S')) {
      if ((l = read (fh1, a, sizeof (double) * N) / sizeof (double)) > 0 && (k = read (fh2, b, sizeof (double) * N) / sizeof (double)) > 0)
        while (j < l && j < k) {
          if ( a[j] != 0 && b[j] != 0 )
          if( fabs( a[j] - b[j] ) > abstol && RELATIVE_DIFF( a[j], b[j] ) > reltol ) {
            if (!quiet) {
              if (NrDiffs++ % 22 == 0) {
                CR;
                PRINT_RULE;
                printf ("\n Double compare of %s and %s\n", File1, File2);
                PRINT_RULE;
              }
              printf ("\nBLOCK: %6ld, SAMPLE: %4ld ===>", i + N1, j + 1);
              printf ("\t%12.6E\t%12.6E\t[Diff=%+5.2E]", a[j], b[j], a[j] - b[j]);
              if (NrDiffs % 22 == 0)
                if ((c = toupper (getchar ())) == 'Q' || c == 'X' || c == 27 || c == 'S')
                  break;
            } else
              NrDiffs++;
          }
          j++;
      } else {
        if (l < 0) {
          KILL (File1, 5);
        } else if (k < 0) {
          KILL (File2, 6);
        } else
          break;
      }
    }
  }
  if (NrDiffs > 0 && !quiet)
    CR;

  /* Release allocated memory */
  free (a);
  free (b);

  /* Return the number of different samples */
  return (NrDiffs);
}

/* ...................... end of compare_doubles() ..........................*/


/*
  -------------------------------------------------------------------------
  long compare_shorts(char *File1, char *File2, int fh1, int fh2,
		      long N, long N1, long N2, char KindOfDump, char quiet);

  Compare short data from 2 files and print in decimal or hex format the
  different samples, indicating block/sample no.

  Return the number of different samples OR -1 on error

  30.Dec.93  v1.0  Simao
  --------------------------------------------------------------------------
*/
long compare_shorts (char *File1, char *File2, int fh1, int fh2, long N, long N1, long N2, char KindOfDump, char quiet, double reltol, short abstol ) {
  long i, j, l, k, NrDiffs;
  char c;
  short *a, *b;

  /* Allocate memory for data vectors */
  if ((a = (short *) calloc (N, sizeof (short))) == NULL)
    return -1;
  if ((b = (short *) calloc (N, sizeof (short))) == NULL) {
    free (a);
    return -1;
  }

  /* Start loop */
  for (c = 0, NrDiffs = i = j = 0; i < N2; i++, j = 0) {
    if (!(c == 'Q' || c == 'X' || c == 27 || c == 'S')) {
      if ((l = read (fh1, a, sizeof (short) * N) / sizeof (short)) > 0 && (k = read (fh2, b, sizeof (short) * N) / sizeof (short)) > 0)
        while (j < l && j < k) {
          if( abs( a[j] - b[j] ) > abstol && RELATIVE_DIFF( a[j], b[j] ) > reltol ) {
            if (!quiet) {
              if (NrDiffs++ % 22 == 0) {
                CR;
                PRINT_RULE;
                printf ("\n Short compare of %s and %s\n", File1, File2);
                PRINT_RULE;
              }
              printf ("\nBLOCK: %6ld, SAMPLE: %4ld ===>", i + 1, j + 1);
              if (KindOfDump == 'H')
                printf ("\t%6.4X\t\t%6.4X", (short unsigned) a[j], (short unsigned) b[j]);
              else
                printf ("\t%6d\t\t%6d", a[j], b[j]);
              if (NrDiffs % 22 == 0)
                if ((c = toupper (getchar ())) == 'Q' || c == 'X' || c == 27 || c == 'S')
                  break;
            } else
              NrDiffs++;
          }
          j++;
      } else {
        if (l < 0) {
          KILL (File1, 5);
        } else if (k < 0) {
          KILL (File2, 6);
        } else
          break;
      }
    }
  }
  if (NrDiffs > 0 && !quiet)
    CR;

  /* Release allocated memory */
  free (a);
  free (b);

  /* Return the number of different samples */
  return (NrDiffs);
}

/* ...................... end of compare_shorts() ..........................*/


/*
  -------------------------------------------------------------------------
  long compare_longs(char *File1, char *File2, int fh1, int fh2,
		      long N, long N1, long N2, char KindOfDump, char quiet);

  Compare long data from 2 files and print in decimal or hex format the
  different samples, indicating block/sample no.

  Return the number of different samples OR -1 on error

  30.Dec.93  v1.0  Simao
  --------------------------------------------------------------------------
*/
long compare_longs (char *File1, char *File2, int fh1, int fh2, long N, long N1, long N2, char KindOfDump, char quiet, double reltol, long abstol ) {
  long i, j, l, k, NrDiffs;
  char c;
  long *a, *b;

  /* Allocate memory for data vectors */
  if ((a = (long *) calloc (N, sizeof (long))) == NULL)
    return -1;
  if ((b = (long *) calloc (N, sizeof (long))) == NULL) {
    free (a);
    return -1;
  }

  /* Start loop */
  for (c = 0, NrDiffs = i = j = 0; i < N2; i++, j = 0) {
    if (!(c == 'Q' || c == 'X' || c == 27 || c == 'S')) {
      if ((l = read (fh1, a, sizeof (long) * N) / sizeof (long)) > 0 && (k = read (fh2, b, sizeof (long) * N) / sizeof (long)) > 0)
        while (j < l && j < k) {
          if( labs( a[j] - b[j] ) > abstol && RELATIVE_DIFF( a[j], b[j] ) > reltol ) {
            if (!quiet) {
              if (NrDiffs++ % 22 == 0) {
                CR;
                PRINT_RULE;
                printf ("\n Long compare of %s and %s\n", File1, File2);
                PRINT_RULE;
              }
              printf ("\nBLOCK: %6ld, SAMPLE: %4ld ===>", i + 1, j + 1);
              if (KindOfDump == 'H')
                printf ("\t%12.4lX\t\t%12.4lX", (long unsigned) a[j], (long unsigned) b[j]);
              else
                printf ("\t%12ld\t\t%12ld", a[j], b[j]);
              if (NrDiffs % 22 == 0)
                if ((c = toupper (getchar ())) == 'Q' || c == 'X' || c == 27 || c == 'S')
                  break;
            } else
              NrDiffs++;
          }
          j++;
      } else {
        if (l < 0) {
          KILL (File1, 5);
        } else if (k < 0) {
          KILL (File2, 6);
        } else
          break;
      }
    }
  }
  if (NrDiffs > 0 && !quiet)
    CR;

  /* Release allocated memory */
  free (a);
  free (b);

  /* Return the number of different samples */
  return (NrDiffs);
}

/* ...................... end of compare_longs() ..........................*/

int main (int argc, char *argv[]) {
  char C[1];
  int fh1, fh2;

  long N, N1, N2, NrDiffs = 0, tot_smp;
  long delay = 0, start_byte1, start_byte2, samplesize;
  char File1[50], File2[50];
  char KindOfDump = 'D', TypeOfData = 'I', quiet = 0;
  FILE *f1, *f2;
  struct stat st;
  long k, l, s1, s2;

  char *abstol;
  double reltol;
  double abstol_double;
  float abstol_float;
  long abstol_long;
  short abstol_short;
  int result;

#ifdef VMS
  char mrs[15] = "mrs=";
#endif

  abstol_double = 0.0;
  abstol_float = 0.0f;
  abstol_long = 0;
  abstol_short = 0;
  abstol = NULL;

  /* ......... GET PARAMETERS ......... */

  /* Check options */
  if (argc < 2)
    display_usage ();
  else {
    while (argc > 1 && argv[1][0] == '-')
      if (strcmp (argv[1], "-delay") == 0) {
        /* Get skip length */
        delay = atol (argv[2]);

        /* Move arg{c,v} over the option to the next argument */
        argc -= 2;
        argv += 2;
      } else if (strcmp (argv[1], "-h") == 0) {
        /* Set dump as hex type */
        KindOfDump = 'H';

        /* Move arg{c,v} over the option to the next argument */
        argc--;
        argv++;
      } else if (strcmp (argv[1], "-d") == 0) {
        /* Set dump in decimal format */
        KindOfDump = 'D';

        /* Move arg{c,v} over the option to the next argument */
        argc--;
        argv++;
      } else if (strcmp (argv[1], "-float") == 0) {
        /* Set data type as real */
        TypeOfData = 'R';

        /* Move arg{c,v} over the option to the next argument */
        argc--;
        argv++;
      } else if (strcmp (argv[1], "-double") == 0) {
        /* Set data type as double real */
        TypeOfData = 'D';

        /* Move arg{c,v} over the option to the next argument */
        argc--;
        argv++;
      } else if (strcmp (argv[1], "-short") == 0) {
        /* Set data type as real */
        TypeOfData = 'I';

        /* Move arg{c,v} over the option to the next argument */
        argc--;
        argv++;
      } else if (strcmp (argv[1], "-long") == 0) {
        /* Set data type as real */
        TypeOfData = 'L';

        /* Move arg{c,v} over the option to the next argument */
        argc--;
        argv++;
      } else if (strcmp (argv[1], "-q") == 0) {
        /* Set quiet compare - only log the total differences */
        quiet = 1;

        /* Move arg{c,v} over the option to the next argument */
        argc--;
        argv++;
      } else if( strcmp( argv[1], "-reltol" ) == 0 ) {
          /* Set relative tolerance */
          argc--;
          argv++;
          sscanf( argv[1], "%lf", &reltol );

          /* Move arg{c,v} over the option to the next argument */
          argc--;
          argv++;
      }
      else if( strcmp( argv[1], "-abstol" ) == 0 ) {
          /* Set relative tolerance */
          argc--;
          argv++;
          abstol = argv[1]; /* Parse value once the input is known */

          /* Move arg{c,v} over the option to the next argument */
          argc--;
          argv++;

      } else {
        fprintf (stderr, "ERROR! Invalid option \"%s\" in command line\n\n", argv[1]);
        display_usage ();
      }
  }

  /* Read parameters for processing */
  GET_PAR_S (1, "First file? ........................... ", File1);
  GET_PAR_S (2, "Second file? .......................... ", File2);
  FIND_PAR_L (3, "Record Length? ........................ ", N, 256);
  FIND_PAR_L (4, "Starting Record? ...................... ", N1, 1);
  FIND_PAR_L (5, "Number of Records? .................... ", N2, 0);
  FIND_PAR_C (6, "Decimal or Hex dump? (D,H) [D] ........ ", C[0], KindOfDump);
  KindOfDump = (C[0] != 'H') ? 'D' : 'H';
  FIND_PAR_C (7, "Short(I), long(L), float (R) or double (D) data? .. ", C[0], TypeOfData);
  TypeOfData = C[0];


  /* ......... SOME INITIALIZATIONS ......... */
  --N1;
  /* Define sample size */
  result = 1;
  switch (TypeOfData) {
  case 'R':
    samplesize = sizeof (float);
    if ( abstol != NULL )
    {
        result = sscanf( abstol, "%f", &abstol_float );
    }
    break;
  case 'D':
    samplesize = sizeof (double);
    if( abstol != NULL )
    {
        result = sscanf( abstol, "%lf", &abstol_double );
    }
    break;
  case 'I':
    samplesize = sizeof (short);
    if( abstol != NULL )
    {
        result = sscanf( abstol, "%hd", &abstol_short );
    }
    break;
  case 'L':
    samplesize = sizeof (long);
    if( abstol != NULL )
    {
        result = sscanf( abstol, "%ld", &abstol_long );
    }
    break;
  default:
    error_terminate ("++==++==++ UNSUPPORTED DATA TYPE ++==++==++\007\n", 7);
  }

  if ( result != 1 ){
      error_terminate("++==++==++ COULD NOT READ ABSOLUTE TOLERANCE ++==++==++\007\n", 7 );
  }

  /* Define 1st sample to compare */
  start_byte1 = start_byte2 = samplesize;
  if (delay >= 0) {
    start_byte1 *= (N1 * N + delay);
    start_byte2 *= (N1 * N);
  } else {
    start_byte1 *= (N1 * N);
    start_byte2 *= (N1 * N - delay);
  }

  //Determine file sizes
  /* ... find the shortest of the 2 files and the number of blks from it */
  /* ... hey, need to skip the delayed samples! ... */
  stat (File1, &st);
  s1 = st.st_size - start_byte1;
  k = ceil ((st.st_size - start_byte1) / (double) (N * samplesize));
  stat (File2, &st);
  s2 = st.st_size - start_byte2;
  l = ceil ((st.st_size - start_byte2) / (double) (N * samplesize));
  N2 = k < l ? k : l;
  tot_smp = (s1 > s2 ? s1 : s2) / samplesize;
  if (k != l) {
    fprintf (stderr, "%%CMP-W-DIFSIZ: Files have different sizes!\n");
  }

  /* Opening test file; abort if there's any problem */
#ifdef VMS
  sprintf (mrs, "mrs=%d", sizeof (short) * N);
#endif

  /* Open input files */
  if ((f1 = fopen (File1, RB)) == NULL)
    KILL (File1, 3);
  if ((f2 = fopen (File2, RB)) == NULL)
    KILL (File2, 4);
  fh1 = fileno (f1);
  fh2 = fileno (f2);


  /* Move pointer to 1st block of interest */
  if (lseek (fh1, start_byte1, 0l) < 0l)
    KILL (File1, 3);
  if (lseek (fh2, start_byte2, 0l) < 0l)
    KILL (File2, 4);

  /* Some preliminaries */
  N1++;

  /* Dumps the file to the screen */
  switch (TypeOfData) {
  case 'I':                    /* short data */
    NrDiffs = compare_shorts (File1, File2, fh1, fh2, N, N1, N2, KindOfDump, quiet, reltol, abstol_short);
    break;

  case 'L':                    /* long data */
    NrDiffs = compare_longs (File1, File2, fh1, fh2, N, N1, N2, KindOfDump, quiet, reltol, abstol_long );
    break;

  case 'R':                    /* float data */
    NrDiffs = compare_floats (File1, File2, fh1, fh2, N, N1, N2, quiet, reltol, abstol_float );
    break;

  case 'D':                    /* double data */
    NrDiffs = compare_doubles (File1, File2, fh1, fh2, N, N1, N2, quiet, reltol, abstol_double );
    break;
  }


  /* Final make-ups */
  if (NrDiffs > 0 && !quiet) {
    PRINT_RULE2;
    CR;
  }
  if (quiet)
    printf ("%s and %s: %ld different samples found out of %ld.\n", File1, File2, NrDiffs, tot_smp);
  else
    printf ("%%CMP-I-NROFDIFFS, %ld different samples found out of %ld.\n", NrDiffs, tot_smp);

  if (NrDiffs > 0 && !quiet) {
    PRINT_RULE2;
    CR;
  }

  /* Finalizations */
  fclose (f1);
  fclose (f2);

  return (s1 == s2 && NrDiffs == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
