#include "stdio.h"

//use extern "C" for C++ programs
//extern "C" {
/*sub function declarations*/
int KeyW(void);
void decodePCX(FILE *fp2,int width,int depth,int pos);
void SetPal1(void);
/*main function declarartions*/
void SetPal(void);
void SetVGA(void);
void SetTXT(void);
void Quit(char message[]);
char InitGameBuff(unsigned long buffw,unsigned long buffd);
void ClearBuffer(unsigned char colour);
void DisplayBuffer(int x,int y);
void DWinBuffer(int buffx,int buffy,int scrx,int scry,int width,int height);
void PutPixel(int x,int y,unsigned char colour);
void Rectangle(int x,int y,int width,int height,unsigned char colour,char fill);
char LoadPic(char name[],int position);
char LoadPic1(char name[],int position);
char LoadMBM(char name[],int position);
void SetColour(unsigned char colour,unsigned char red,unsigned char green,unsigned char blue);
void PutPic(int no,int X,int Y);
void PutPicM(int no,int X,int Y);
void PutMBM(int no,int X,int Y);
void Line(int x1,int y1,int x2,int y2,int colour);
void CutOut(int x,int y,int width,int height,unsigned char *ptr);
void PutBitmap(int x,int y,unsigned char *ptr);
unsigned char TestBuffPix(int x,int y);
void InitKeyScan(void); void CloseKeyScan(void);
char CheckKey(char kno);
void FreeMem(void);
void LineVGA(int x1,int y1,int x2,int y2,int colour);
void PutPicVGA(int no,int X,int Y);
//};
