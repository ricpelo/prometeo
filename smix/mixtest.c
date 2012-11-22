/*      SMIXC is Copyright 1995 by Ethan Brodsky.  All rights reserved      */

/* лл MIXTEST.C ллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл */

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "detect.h"
#include "smix.h"

#define ON  1
#define OFF 0

#define TRUE  1
#define FALSE 0

#define SHAREDEMB    /* Undefine this to store all sounds in separate EMBs */
#define NUMSOUNDS 6  /* Change this if you want to add more sounds         */

char *resource_file = "mixtest.snd";

char *sound_key[NUMSOUNDS] =
  {
    "JET",
    "GUN",
    "CRASH",
    "CANNON",
    "LASER",
    "GLASS"
  };

int baseio, irq, dma, dma16;

SOUND *sound[NUMSOUNDS];

int  stop;

long counter;

char inkey;
int  num;
int  temp;

void ourexitproc(void)
  {
    int i;

    for (i=0; i < NUMSOUNDS; ++i)
      if (sound[i] != NULL) free_sound(sound+i);
#ifdef SHAREDEMB
    shutdown_sharing();
#endif
  }

void init(void)
  {
    int i;

    randomize();

    printf("-------------------------------------------\n");
    printf("Sound Mixing Library v1.21 by Ethan Brodsky\n");
    if (!detect_settings(&baseio, &irq, &dma, &dma16))
      {
        printf("ERROR:  Invalid or non-existant BLASTER environment variable!\n");
        exit(EXIT_FAILURE);
      }
    else
      {
        if (!init_sb(baseio, irq, dma, dma16))
          {
            printf("ERROR:  Error initializing sound card!\n");
            exit(EXIT_FAILURE);
          }
      }

    printf("BaseIO=%Xh     IRQ%u     DMA8=%u     DMA16=%u\n", baseio, irq, dma, dma16);

    printf("DSP version %.2f:  ", dspversion);
    if (sixteenbit)
      printf("16-bit, ");
    else
      printf("8-bit, ");
    if (autoinit)
      printf("Auto-initialized\n");
    else
      printf("Single-cycle\n");

    if (!init_xms())
      {
        printf("ERROR:  Can not initialize extended memory\n");
        printf("HIMEM.SYS must be installed\n");
        exit(EXIT_FAILURE);
      }
    else
      {
        printf("Extended memory successfully initialized\n");
        printf("Free XMS memory: %uk    ", getfreexms());
        if (!getfreexms())
          {
            printf("ERROR:  Insufficient free XMS\n");
            exit(EXIT_FAILURE);
          }
        else
          {
            printf("Loading sounds\n");
#ifdef SHAREDEMB
            init_sharing();
#endif
            open_sound_resource_file(resource_file);

            for (i=0; i < NUMSOUNDS; i++)
              load_sound(&(sound[i]), sound_key[i]);
            atexit(ourexitproc);

            close_sound_resource_file();
          }
      }
    init_mixing();
    printf("\n");
  }

void shutdown(void)
  {
    int i;

    shutdown_mixing();
    shutdown_sb();

    for (i=0; i < NUMSOUNDS; i++)
      free_sound(sound+i);
#ifdef SHAREDEMB
    shutdown_sharing();
#endif
    printf("\n");
  }

int main(void)
  {
    init();

/*    start_sound(sound[0], 0, ON);*/ /* Start up the jet engine */

    printf("Press:\n");
    printf("  1)  Machine Gun\n");
    printf("  2)  Crash\n");
    printf("  3)  Cannon\n");
    printf("  4)  Laser\n");
    printf("  5)  Breaking glass\n");
    printf("  Q)  Quit\n");

    stop = FALSE;

    counter = 0;

    do
      {
       /* Increment and display counters */
        counter++;
        cprintf("%8lu %8lu %4u", counter, intcount, voicecount);
        gotoxy(1, wherey());

       /* Maybe start a random sound */
        if (!random(10000))
          {
           num = (random(NUMSOUNDS-1))+1;
           start_sound(sound[num], num, OFF);
          }

       /* Start a sound if a key is pressed */
        if (kbhit())
          {
            inkey = getch();
            if ((inkey >= '0') && (inkey <= '9'))
              {
                num = inkey - '0'; /* Convert to integer */
                if (num < NUMSOUNDS)
                  start_sound(sound[num], num, FALSE);
              }
            else
              stop = TRUE;
          }
      }
    while (!stop);

    printf("\n");
    stop_sound(1); /* Stop the jet engine */

    shutdown();

    return(EXIT_SUCCESS);
  }
