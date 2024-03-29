/* лл DETECT.H лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл */
#ifndef _DETECT_H
#define _DETECT_H

#define TRUE 1
#define FALSE 0

int detect_settings(int *baseio, int *irq, int *dma, int *dma16);
  /* Detects sound card settings using BLASTER environment variable */
  /* Parameters:                                                    */
  /*   baseio    Sound card base IO address                         */
  /*   irq       Sound card IRQ                                     */
  /*   dma       Sound card 8-bit DMA channel                       */
  /*   dma16     Sound card 16-bit DMA channel (0 if none)          */
  /* Returns:                                                       */
  /*   TRUE      Sound card settings detected successfully          */
  /*   FALSE     Sound card settings could not be detected          */

/* лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл */


#endif
