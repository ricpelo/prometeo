/*      SMIXC is Copyright 1995 by Ethan Brodsky.  All rights reserved      */

/* л smix.c v1.21 ллллллллллллллллллллллллллллллллллллллллллллллллллллллллл */

#define TRUE  1
#define FALSE 0

#define ON  1
#define OFF 0

#define BLOCK_LENGTH    256   /* Length of digitized sound output block     */
#define LOAD_CHUNK_SIZE 2048  /* Chunk size used to load sounds from disk   */
#define VOICES          8     /* Number of available simultaneous voices    */

typedef struct
  {
    int xmshandle;
    long startofs;
    long soundsize;
  } SOUND;

int  init_sb(int baseio, int irq, int dma, int dma16);
  /* Initializes control parameters, resets DSP and installs int. handler   */
  /*  Parameters:                                                           */
  /*   baseio    Sound card base IO address                                 */
  /*   irq       Sound card IRQ setting                                     */
  /*   dma       Sound card 8-bit DMA channel                               */
  /*   dma16     Sound card 16-bit DMA channel                              */
  /*  Returns:                                                              */
  /*   TRUE      Sound card successfully initialized                        */
  /*   FALSE     Sound card could not be initialized                        */

void shutdown_sb(void);
  /* Removes interrupt handler and resets DSP                               */


void init_mixing(void);
  /* Allocates internal buffers and starts digitized sound output           */

void shutdown_mixing(void);
  /* Deallocates internal buffers and stops digitized sound output          */


int  init_xms(void);
  /* Initializes extended memory driver                                     */
  /*  Returns:                                                              */
  /*   TRUE      Extended memory driver successfully initialized            */
  /*   FALSE     Extended memory driver could not be initialized            */

int  getfreexms(void);
  /* Returns amount of free extended memory (In kilobytes)                  */


void init_sharing(void);
  /* Allocates an EMB that all sound data will be stored in.  Using this    */
  /* will preserve extended memory handles, which are scarce resources.     */
  /* Call this on initialization and all sounds will automatically be       */
  /* stored in one EMB.  You can call load_sound as usual to allocate a     */
  /* sound, but free_sound will only deallocate the sound data structure.   */
  /* You must call shutdown_sharing before program termination in order     */
  /* to free all allocated extended memory.                                 */

void shutdown_sharing(void);
  /* Shuts down EMB sharing and frees shared EMB block                      */


void open_sound_resource_file(char *filename);
  /* Opens a resource file for loading sounds.  After this has been called, */
  /* the Key parameter in the LoadSound function is used as a resource key  */
  /* to locate the sound data in this file.                                 */
  /*  Parameters:                                                           */
  /*   FileName: File name of resource file                                 */

void close_sound_resource_file(void);
  /* Close sound resource file.  If you have called this, the Key parameter */
  /* to the LoadSound function will act as a filename instead of a resource */
  /* key.                                                                   */


void load_sound(SOUND **sound, char *key);
  /* Allocates an extended memory block and loads a sound from a file       */
  /*  Parameters:                                                           */
  /*   sound     Pointer to pointer to unallocated sound data structure     */
  /*   key       If a resource file has been opened then key is a resource  */
  /*             identifier.  Use the same ID as you used when adding the   */
  /*             sound resource with SNDLIB.  If a resource file is not     */
  /*             opened, then key is the filename from which to load the    */
  /*             sound data.                                                */

void free_sound(SOUND **sound);
  /* Frees sound data structure and extended memory block                   */
  /*  Parameters:                                                           */
  /*   sound     Pointer to pointer to allocated sound data structure       */


void start_sound(SOUND *sound, int index, int loop);
  /* Starts playing a sound                                                 */
  /*  Parameters:                                                           */
  /*   sound     Pointer to sound data structure                            */
  /*   index     A number to keep track of the sound with (Used to stop it) */
  /*   loop      Indicates whether sound should be continuously looped      */

void stop_sound(int index);
  /* Stops playing a sound                                                  */
  /*  Parameters:                                                           */
  /*   index     Index of sound to stop (All with given index are stopped)  */

int  sound_playing(int index);
  /* Checks if a sound is still playing                                     */
  /*  Parameters:                                                           */
  /*   index     Index used when the sound was started                      */
  /*  Returns:                                                              */
  /*   TRUE      At least one sound with the specified index is playing     */
  /*   FALSE     No sounds with the specified index are playing             */

volatile long  intcount;       /* Current count of sound interrupts         */
volatile float dspversion;     /* Version of the sound card DSP chip        */
volatile int   voicecount;     /* Number of voices currently in use         */

int autoinit;
int sixteenbit;

/* лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл */

#include <alloc.h>
#include <conio.h>
#include <dos.h>
#include <mem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xms.h"

#define BUFFER_LENGTH BLOCK_LENGTH*2

#define BYTE unsigned char

#define lo(value) (unsigned char)((value) & 0x00FF)
#define hi(value) (unsigned char)((value) >> 8)

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) > (b)) ? (b) : (a))

static int resetport;
static int readport;
static int writeport;
static int pollport;
static int ackport;

static int pic_rotateport;
static int pic_maskport;

static int dma_maskport;
static int dma_clrptrport;
static int dma_modeport;
static int dma_addrport;
static int dma_countport;
static int dma_pageport;

static int irq_startmask;
static int irq_stopmask;
static int irq_intvector;

static int dma_startmask;
static int dma_stopmask;
static int dma_mode;
static int dma_length;

static int  shared_emb     = FALSE;
static int  shared_handle  = 0;
static long shared_size    = 0;

static void (interrupt far *oldintvector)(void);

static int handler_installed = FALSE;

static void write_dsp(BYTE value)
  {
    while ((inp(writeport) & 0x80));
    outp(writeport, value);
  }

static BYTE read_dsp(void)
  {
    while (!(inp(pollport) & 0x80));
    return(inp(readport));
  }

static int reset_dsp(void)
  {
    int i;

    outp(resetport, 1);
    delay(1);                            /* 1 millisecond */
    outp(resetport, 0);

    i = 100;
    while ((i-- > 0) && (read_dsp() != 0xAA));

    return(i > 0);
  }

void install_handler(void);
void uninstall_handler(void);
void smix_exitproc(void);

int init_sb(int baseio, int irq, int dma, int dma16)
  {
   /* Sound card IO ports */
    resetport  = baseio + 0x006;
    readport   = baseio + 0x00A;
    writeport  = baseio + 0x00C;
    pollport   = baseio + 0x00E;

   /* Reset DSP, get version, choose output mode */
    if (!reset_dsp())
      return(FALSE);
    write_dsp(0xE1);  /* Get DSP version number */
    dspversion = read_dsp();  dspversion += read_dsp() / 100.0;
    autoinit   = (dspversion > 2.0);
    sixteenbit = (dspversion > 4.0) && (dma16 != 0) && (dma16 > 3);

   /* Compute interrupt controller ports and parameters */
    if (irq < 8)
      { /* PIC1 */
        irq_intvector  = 0x08 + irq;
        pic_rotateport = 0x20;
        pic_maskport   = 0x21;
      }
    else
      { /* PIC2 */
        irq_intvector  = 0x70 + irq-8;
        pic_rotateport = 0xA0;
        pic_maskport   = 0xA1;
      }
    irq_stopmask  = 1 << (irq % 8);
    irq_startmask = ~irq_stopmask;

   /* Compute DMA controller ports and parameters */
    if (sixteenbit)
      { /* Sixteen bit */
        dma_maskport   = 0xD4;
        dma_clrptrport = 0xD8;
        dma_modeport   = 0xD6;
        dma_addrport   = 0xC0 + 4*(dma16-4);
        dma_countport  = 0xC2 + 4*(dma16-4);
        switch (dma16)
          {
            case 5:
              dma_pageport = 0x8B;
              break;
            case 6:
              dma_pageport = 0x89;
              break;
            case 7:
              dma_pageport = 0x8A;
              break;
          }
        dma_stopmask  = dma16-4 + 0x04;  /* 000001xx */
        dma_startmask = dma16-4 + 0x00;  /* 000000xx */
        if (autoinit)
          dma_mode = dma16-4 + 0x58;     /* 010110xx */
        else
          dma_mode = dma16-4 + 0x48;     /* 010010xx */
        ackport = baseio + 0x00F;
      }
    else
      { /* Eight bit */
        dma_maskport   = 0x0A;
        dma_clrptrport = 0x0C;
        dma_modeport   = 0x0B;
        dma_addrport   = 0x00 + 2*dma;
        dma_countport  = 0x01 + 2*dma;
        switch (dma)
          {
            case 0:
              dma_pageport = 0x87;
              break;
            case 1:
              dma_pageport = 0x83;
              break;
            case 2:
              dma_pageport = 0x81;
              break;
            case 3:
              dma_pageport = 0x82;
              break;
          }
        dma_stopmask  = dma + 0x04;      /* 000001xx */
        dma_startmask = dma + 0x00;      /* 000000xx */
        if (autoinit)
          dma_mode    = dma + 0x58;      /* 010110xx */
        else
          dma_mode    = dma + 0x48;      /* 010010xx */
        ackport = baseio + 0x00E;
      }
    if (autoinit)
      dma_length = BUFFER_LENGTH;
    else
      dma_length = BLOCK_LENGTH;

    install_handler();
    atexit(smix_exitproc);

    return(TRUE);
  }

void shutdown_sb(void)
  {
    if (handler_installed) uninstall_handler();
    reset_dsp();
  }

int init_xms(void)
  {
    xms_init();
    return(xms_installed());
  }

int getfreexms(void)
  {
    return(xms_getfreemem());
  }

/* Voice control */
typedef struct
  {
    SOUND *sound;
    int   index;
    long  curpos;
    int   loop;
  } VOICE;

static int   inuse[VOICES];
static VOICE voice[VOICES];

/* Sound buffer */

static signed char soundblock[BLOCK_LENGTH+1];   /* Signed 8 bit */
  /* The length of XMS copies using HIMEM.SYS must be a mutiple  */
  /* of two.  If the sound data ends in mid-block, it may not be */
  /* possible to round up without corrupting memory.  Therefore, */
  /* the copy buffer has been extended by one byte to eliminate  */
  /* this problem.                                               */

/* Mixing buffer */

static short int mixingblock[BLOCK_LENGTH];    /* Signed 16 bit */

/* Output buffers */

static void *outmemarea = NULL;
static unsigned char (*out8buf)[2][BLOCK_LENGTH]  = NULL; /* Unsigned 8 bit */
static short int     (*out16buf)[2][BLOCK_LENGTH] = NULL; /* Signed 16 bit  */

static int curblock;
static void *blockptr[2];
static void *curblockptr;

/* Addressing for auto-initialized transfers (Whole buffer) */
static unsigned long buffer_addr;
static unsigned char buffer_page;
static unsigned int  buffer_ofs;

/* Addressing for single-cycle transfers (One block at a time */
static unsigned long block_addr[2];
static unsigned char block_page[2];
static unsigned int  block_ofs[2];

static int handler_installed;

/* 8-bit clipping */

static unsigned char *clip_8_buf;  /* Pointer to clipping array             */
static unsigned char *clip_8;      /* Pointer to center of clipping array   */

static void start_dac(void)
  {
    outp(dma_maskport,   dma_stopmask);
    outp(dma_clrptrport, 0x00);
    outp(dma_modeport,   dma_mode);
    outp(dma_addrport,   lo(buffer_ofs));
    outp(dma_addrport,   hi(buffer_ofs));
    outp(dma_countport,  lo(dma_length-1));
    outp(dma_countport,  hi(dma_length-1));
    outp(dma_pageport,   buffer_page);
    outp(dma_maskport,   dma_startmask);

    if (sixteenbit)
      { /* Sixteen bit auto-initialized: SB16 and up (DSP 4.xx)             */
        write_dsp(0x41);                /* Set sound output sampling rate   */
        write_dsp(hi(22050));
        write_dsp(lo(22050));
        write_dsp(0xB6);                /* 16-bit cmd  - D/A - A/I - FIFO   */
        write_dsp(0x10);                /* 16-bit mode - signed mono        */
        write_dsp(lo(BLOCK_LENGTH-1));
        write_dsp(hi(BLOCK_LENGTH-1));
      }
    else
      { /* Eight bit */
        write_dsp(0xD1);                /* Turn on speaker                  */
        write_dsp(0x40);                /* Set sound output time constant   */
        write_dsp(211);                 /*  = 256 - (1000000 / rate)        */
        if (autoinit)
          { /* Eight bit auto-initialized:  SBPro and up (DSP 2.00+)        */
            write_dsp(0x48);            /* Set DSP block transfer size      */
            write_dsp(lo(BLOCK_LENGTH-1));
            write_dsp(hi(BLOCK_LENGTH-1));
            write_dsp(0x1C);            /* 8-bit auto-init DMA mono output  */
          }
        else
          { /* Eight bit single-cycle:  Sound Blaster (DSP 1.xx+)           */
            write_dsp(0x14);            /* 8-bit single-cycle DMA output    */
            write_dsp(lo(BLOCK_LENGTH-1));
            write_dsp(hi(BLOCK_LENGTH-1));
          }
      }
  }

static void stop_dac(void)
  {
    if (sixteenbit)
      {
        write_dsp(0xD5);                /* Pause 16-bit DMA sound I/O       */
      }
    else
      {
        write_dsp(0xD0);                /* Pause 8-bit DMA sound I/O        */
        write_dsp(0xD3);                /* Turn off speaker                 */
      }
    outp(dma_maskport, dma_stopmask);
  }


/* Setup for storinng all sounds in one EMB (Saves handles) */

void init_sharing(void)
  {
    shared_emb  = TRUE;
    shared_size = 0;
    xms_allocate(&shared_handle, shared_size);
  }

void shutdown_sharing(void)
  {
    if (shared_emb) xms_free(&shared_handle);
    shared_emb    = FALSE;
    shared_handle = 0;
    shared_size   = 0;
  }

/* Setup for sound resource files */

static int  resource_file = FALSE;
static char resource_filename[64] = "";

void open_sound_resource_file(char *filename)
  {
    resource_file = TRUE;
    strcpy(resource_filename, filename);
  }

void close_sound_resource_file(void)
  {
    resource_file = FALSE;
    strcpy(resource_filename, "");
  }


/* Loading and freeing sounds */

static FILE *sound_file;
static long sound_size;

typedef struct
  {
    char key[8];
    long start;
    long size;
  } RESOURCE_HEADER;

void get_sound_file(char *key)
  {
    short numsounds;
    int   found;
    int   i;
    RESOURCE_HEADER res_header;

    found = FALSE;

    if (resource_file)
      {
        sound_file = fopen(resource_filename, "rb");
        fread(&numsounds, sizeof(numsounds), 1, sound_file);

        for (i = 0; i < numsounds; i++)
          {
            fread(&res_header, sizeof(res_header), 1, sound_file);
            if (!strnicmp(key, res_header.key, 8))
              {
                found = TRUE;
                break;
              }
          }

        if (found)
          {
            fseek(sound_file, res_header.start, SEEK_SET);
            sound_size = res_header.size;
          }
        else
          exit(EXIT_FAILURE);
      }
    else
      {
        sound_file = fopen(key, "rb");
        fseek(sound_file, 0, SEEK_END);
        sound_size = ftell(sound_file);
        fseek(sound_file, 0, SEEK_SET);
      }
  }

void load_sound(SOUND **sound, char *key)
  {
    char inbuf[LOAD_CHUNK_SIZE];
    static MOVEPARAMS moveparams;

    get_sound_file(key);

    *sound = (SOUND *)farmalloc(sizeof(SOUND));

    (*sound)->soundsize = sound_size;

    if (!shared_emb)
      {
        (*sound)->startofs = 0;
        xms_allocate(&((*sound)->xmshandle), (sound_size + 1023) / 1024);
      }
    else
      {
        (*sound)->startofs  = shared_size;
        (*sound)->xmshandle = shared_handle;
        shared_size += sound_size;
        xms_reallocate(shared_handle, (shared_size + 1023) / 1024);
      }

    moveparams.sourcehandle = 0;
    moveparams.sourceoffset = (long)(&inbuf);
    moveparams.desthandle   = (*sound)->xmshandle;
    moveparams.destoffset   = (*sound)->startofs;

    do
      {
        moveparams.length = fread(&inbuf, 1, sizeof(inbuf), sound_file);
        moveparams.length = ((moveparams.length+1) / 2) * 2;
          /* XMS copy lengths must be a multiple of two */
        xms_move(&moveparams);
        moveparams.destoffset += moveparams.length;
      }
    while (moveparams.length != 0);

    fclose(sound_file);
  }

void free_sound(SOUND **sound)
  {
    if (!shared_emb) xms_free(&((*sound)->xmshandle));
    free(*sound);
    *sound = NULL;
  }

/* Voice maintainance */

static void deallocate_voice(int voicenum)
  {
    inuse[voicenum] = FALSE;
    voice[voicenum].sound  = NULL;
    voice[voicenum].index  = -1;
    voice[voicenum].curpos = 0;
    voice[voicenum].loop   = 0;
  }

void start_sound(SOUND *sound, int index, int loop)
  {
    int i, slot;
    slot = -1; i = 0;
    do
      {
        if (!inuse[i])
          slot = i;
        i++;
      }
    while ((slot == -1) && (i < VOICES));
    if (slot != -1)
      {
        voice[slot].sound  = sound;
        voice[slot].index  = index;
        voice[slot].curpos = 0;
        voice[slot].loop   = loop;

        ++voicecount;
        inuse[slot] = TRUE;
      }
  }

void stop_sound(int index)
  {
    int i;
    for (i=0; i < VOICES; i++)
      if (voice[i].index == index)
        {
          deallocate_voice(i);
          --voicecount;
        }
  }

int  sound_playing(int index)
  {
    int i;

   /* Search for a sound with the specified index */
    for (i=0; i < VOICES; i++)
      if (voice[i].index == index)
        return(TRUE);

   /* Sound not found */
    return(FALSE);
  }

static void update_voices(void)
  {
    int voicenum;

    for (voicenum=0; voicenum < VOICES; voicenum++)
      {
        if (inuse[voicenum])
          {
            if (voice[voicenum].curpos >= voice[voicenum].sound->soundsize)
              {
                deallocate_voice(voicenum);
                --voicecount;
              }
          }
      }
  }

/* Utility functions */

static void setcurblock(int blocknum)
  {
    curblockptr = blockptr[(curblock = blocknum)];
  }

static void silenceblock(void)
  {
    _fmemset(&mixingblock, 0x00, BLOCK_LENGTH * sizeof(short int));
  }

static long getlinearaddr(void far *ptr)
  {
    return((long)FP_SEG(ptr)*16 + (long)FP_OFF(ptr));
  }

/* Mixing initialization */

static void init_clip8(void)
  {
    int i;
    int value;

    clip_8_buf = farmalloc(256*VOICES);
    clip_8     = clip_8_buf + 128*VOICES;

    for (i = -128*VOICES; i < 128*VOICES; i++)
      {
        value = i;
        value = max(value, -128);
        value = min(value, 127);

        clip_8[i] = value + 128;
      }
  }

void init_mixing(void)
  {
    int i;

    for (i=0; i < VOICES; i++)
      deallocate_voice(i);
    voicecount = 0;

    if (sixteenbit)
      {
       /* Find a block of memory that does not cross a page boundary */
        out16buf = outmemarea = farmalloc(4*BUFFER_LENGTH);
        if ((((getlinearaddr(outmemarea) >> 1) % 65536) + BUFFER_LENGTH) > 65536)
          out16buf += BUFFER_LENGTH;
        for (i=0; i<2; i++) blockptr[i] = (void *)&((*out16buf)[i]);

       /* DMA parameters */
        buffer_addr = getlinearaddr(out16buf);
        buffer_page = buffer_addr >> 16;
        buffer_ofs  = (buffer_addr >> 1) % 65536;

        _fmemset((void *)out16buf, 0x00, BUFFER_LENGTH*2);
      }
    else
      {
       /* Find a block of memory that does not cross a page boundary */
        out8buf = outmemarea = farmalloc(2*BUFFER_LENGTH);
        if (((getlinearaddr(outmemarea) % 65536) + BUFFER_LENGTH) > 65536)
          out8buf += BUFFER_LENGTH;
        for (i=0; i<2; i++) blockptr[i] = (void *)&((*out8buf)[i]);

       /* DMA parameters */
        buffer_addr = getlinearaddr(out8buf);
        buffer_page = buffer_addr / 65536;
        buffer_ofs  = buffer_addr % 65536;
        for (i=0; i<2; i++)
          {
            block_addr[i] = getlinearaddr(blockptr[i]);
            block_page[i] = block_addr[i] / 65536;
            block_ofs[i]  = block_addr[i]  % 65536;
          }
        _fmemset((void *)out8buf, 0x80, BUFFER_LENGTH);

        init_clip8();
      }

    silenceblock();
    setcurblock(0);
    intcount = 0;
    start_dac();
  }

void shutdown_mixing(void)
  {
    stop_dac();

    if (!sixteenbit) free(clip_8_buf);

    free((void *)outmemarea);
  }

static void copy_sound(SOUND *sound, long *curpos, int copylength, int loop)
  {
    long soundsize;
    char far *destptr;
    static MOVEPARAMS moveparams;

    soundsize = sound->soundsize;
    destptr   = (char far *)(&soundblock);
    moveparams.sourcehandle = sound->xmshandle;
    moveparams.desthandle   = 0;

    do
      {
       /* Compute transfer size */
        moveparams.length = min(copylength, soundsize - (*curpos));

       /* Compute starting source offset and update offset for next block */
        moveparams.sourceoffset = sound->startofs + (*curpos);
        (*curpos) += moveparams.length;
        if (loop)
          (*curpos) %= soundsize;

       /* Compute starting dest offset and update offset for next block */
        moveparams.destoffset = (long)destptr;
        destptr += moveparams.length;

       /* Update remaining count for next iteration (If any) */
        copylength -= moveparams.length;

       /* Copy block down from extended memory */
        moveparams.length = ((moveparams.length + 1) / 2) * 2;
          /* XMS copy lengths must be a multiple of two */
        xms_move(&moveparams);  /* Luckily, the XMS driver is re-entrant */
      }
    while (copylength > 0);
  }

static void mix_voice(VOICE *voice)
  {
    int i;

    int mixlength;

    if (voice->loop)
      mixlength = BLOCK_LENGTH;
    else
      mixlength = MIN(BLOCK_LENGTH, voice->sound->soundsize - voice->curpos);
    copy_sound(voice->sound, &(voice->curpos), mixlength, voice->loop);

    for (i=0; i < BLOCK_LENGTH; i++)
      mixingblock[i] += soundblock[i];
  }

static void mix_voices(void)
  {
    int i;

    silenceblock();
    for (i=0; i < VOICES; i++)
      if (inuse[i]) mix_voice(&(voice[i]));
  }

static void copy_sound16(void)
  {
    int i;
    signed int *destptr;

    destptr = blockptr[curblock];

    for (i=0; i < BLOCK_LENGTH; i++)
      destptr[i] = mixingblock[i] << 5;
  }

static void copy_sound8(void)
  {
    unsigned char *outptr;
    int i;

    outptr = &((*out8buf)[curblock][0]);
    for (i=0; i < BLOCK_LENGTH; i++)
      *outptr++ = clip_8[mixingblock[i]];
  }

static void copydata(void)
  {
    if (sixteenbit)
      copy_sound16();
    else
      copy_sound8();
  }

static void startblock_sc(void) /* Starts a single-cycle DMA transfer */
  {
    outp(dma_maskport,   dma_stopmask);
    outp(dma_clrptrport, 0x00);
    outp(dma_modeport,   dma_mode);
    outp(dma_addrport,   lo(block_ofs[curblock]));
    outp(dma_addrport,   hi(block_ofs[curblock]));
    outp(dma_countport,  lo(BLOCK_LENGTH-1));
    outp(dma_countport,  hi(BLOCK_LENGTH-1));
    outp(dma_pageport,   block_page[curblock]);
    outp(dma_maskport,   dma_startmask);
    write_dsp(0x14);                /* 8-bit single-cycle DMA sound output  */
    write_dsp(lo(BLOCK_LENGTH-1));
    write_dsp(hi(BLOCK_LENGTH-1));
  }

static void interrupt inthandler(void)
  {
    intcount++;

    if (!autoinit)     /* Start next block first if not using auto-init DMA */
      {
        startblock_sc();
        copydata();
        setcurblock(!curblock);    /* Toggle block */
      }

    update_voices();
    mix_voices();

    if (autoinit)
      {
        copydata();
        setcurblock(!curblock);    /* Toggle block */
      }

    inp(ackport);                  /* Acknowledge interrupt with sound card */
    outp(0xA0, 0x20);              /* Acknowledge interrupt with PIC2       */
    outp(0x20, 0x20);              /* Acknowledge interrupt with PIC1       */
  }

static void install_handler(void)
  {
    disable();
    outp(pic_maskport, (inp(pic_maskport) | irq_stopmask));

    oldintvector = _dos_getvect(irq_intvector);
    _dos_setvect(irq_intvector, inthandler);

    outp(pic_maskport, (inp(pic_maskport) & irq_startmask));
    enable();

    handler_installed = TRUE;
  }

static void uninstall_handler(void)
  {
      disable();
      outp(pic_maskport, (inp(pic_maskport) | irq_stopmask));

      _dos_setvect(irq_intvector, oldintvector);

      enable();

      handler_installed = FALSE;
  }

static void smix_exitproc(void)
  {
    stop_dac();
    shutdown_sb();
  }
