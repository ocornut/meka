/** 6502 Disassembler *****************************************/
/**                                                          **/
/**                        dasm6502.c                        **/
/**                                                          **/
/** This file contains the source of a portable disassembler **/
/** for the 6502 CPU.                                        **/
/**                                                          **/
/** Copyright (C) Marat Fayzullin 1996                       **/
/**               Alex Krasivsky  1996                       **/
/**     You are not allowed to distribute this software      **/
/**     commercially. Please, notify me, if you make any     **/
/**     changes to this file.                                **/
/**************************************************************/

#include <stdio.h>
#include <string.h>

#ifdef ZLIB
#include <zlib.h>
#define fopen          gzopen
#define fclose         gzclose
#define fread(B,N,L,F) gzread(F,B,(L)*(N))
#endif

typedef unsigned char byte;   /* This type is exactly 1 byte  */
typedef unsigned short word;  /* This type is exactly 2 bytes */

static int PrintHex;          /* Print hexadecimal codes      */
static unsigned long Counter; /* Address counter              */

enum { Ac=0,Il,Im,Ab,Zp,Zx,Zy,Ax,Ay,Rl,Ix,Iy,In,No };
    /* These are the addressation methods used by 6502 CPU.   */

static int DAsm(char *S,byte *A,unsigned long PC);
    /* This function will disassemble a single command and    */
    /* return the number of bytes disassembled.               */

static byte *MN[] =
{
  "adc","and","asl","bcc","bcs","beq","bit","bmi",
  "bne","bpl","brk","bvc","bvs","clc","cld","cli",
  "clv","cmp","cpx","cpy","dec","dex","dey","inx",
  "iny","eor","inc","jmp","jsr","lda","nop","ldx",
  "ldy","lsr","ora","pha","php","pla","plp","rol",
  "ror","rti","rts","sbc","sta","stx","sty","sec",
  "sed","sei","tax","tay","txa","tya","tsx","txs"
};

static byte AD[512] =
{
  10,Il, 34,Ix, No,No, No,No, No,No, 34,Zp,  2,Zp, No,No,
  36,Il, 34,Im,  2,Ac, No,No, No,No, 34,Ab,  2,Ab, No,No,
   9,Rl, 34,Iy, No,No, No,No, No,No, 34,Zx,  2,Zx, No,No,
  13,Il, 34,Ay, No,No, No,No, No,No, 34,Ax,  2,Ax, No,No,
  28,Ab,  1,Ix, No,No, No,No,  6,Zp,  1,Zp, 39,Zp, No,No,
  38,Il,  1,Im, 39,Ac, No,No,  6,Ab,  1,Ab, 39,Ab, No,No,
   7,Rl,  1,Iy, No,No, No,No, No,No,  1,Zx, 39,Zx, No,No,
  47,Il,  1,Ay, No,No, No,No, No,No,  1,Ax, 39,Ax, No,No,
  41,Il, 25,Ix, No,No, No,No, No,No, 25,Zp, 33,Zp, No,No,
  35,Il, 25,Im, 33,Ac, No,No, 27,Ab, 25,Ab, 33,Ab, No,No,
  11,Rl, 25,Iy, No,No, No,No, No,No, 25,Zx, 33,Zx, No,No,
  15,Il, 25,Ay, No,No, No,No, No,No, 25,Ax, 33,Ax, No,No,
  42,Il,  0,Ix, No,No, No,No, No,No,  0,Zp, 40,Zp, No,No,
  37,Il,  0,Im, 40,Ac, No,No, 27,In,  0,Ab, 40,Ab, No,No,
  12,Rl,  0,Iy, No,No, No,No, No,No,  0,Zx, 40,Zx, No,No,
  49,Il,  0,Ay, No,No, No,No, No,No,  0,Ax, 40,Ax, No,No,
  No,No, 44,Ix, No,No, No,No, 46,Zp, 44,Zp, 45,Zp, No,No,
  22,Il, No,No, 52,Il, No,No, 46,Ab, 44,Ab, 45,Ab, No,No,
   3,Rl, 44,Iy, No,No, No,No, 46,Zx, 44,Zx, 45,Zy, No,No,
  53,Il, 44,Ay, 55,Il, No,No, No,No, 44,Ax, No,No, No,No,
  32,Im, 29,Ix, 31,Im, No,No, 32,Zp, 29,Zp, 31,Zp, No,No,
  51,Il, 29,Im, 50,Il, No,No, 32,Ab, 29,Ab, 31,Ab, No,No,
   4,Rl, 29,Iy, No,No, No,No, 32,Zx, 29,Zx, 31,Zy, No,No,
  16,Il, 29,Ay, 54,Il, No,No, 32,Ax, 29,Ax, 31,Ay, No,No,
  19,Im, 17,Ix, No,No, No,No, 19,Zp, 17,Zp, 20,Zp, No,No,
  24,Il, 17,Im, 21,Il, No,No, 19,Ab, 17,Ab, 20,Ab, No,No,
   8,Rl, 17,Iy, No,No, No,No, No,No, 17,Zx, 20,Zx, No,No,
  14,Il, 17,Ay, No,No, No,No, No,No, 17,Ax, 20,Ax, No,No,
  18,Im, 43,Ix, No,No, No,No, 18,Zp, 43,Zp, 26,Zp, No,No,
  23,Il, 43,Im, 30,Il, No,No, 18,Ab, 43,Ab, 26,Ab, No,No,
   5,Rl, 43,Iy, No,No, No,No, No,No, 43,Zx, 26,Zx, No,No,
  48,Il, 43,Ay, No,No, No,No, No,No, 43,Ax, 26,Ax, No,No
};

/** DAsm() ****************************************************/        
/** This function will disassemble a single command and      **/    
/** return the number of bytes disassembled.                 **/  
/**************************************************************/
int DAsm(char *S,byte *A,unsigned long PC)
{
  byte *B,J;
  int OP;

  B=A;OP=(*B++)*2;

  switch(AD[OP+1])
  {
    case Ac: sprintf(S,"%s a",MN[AD[OP]]);break;
    case Il: sprintf(S,"%s",MN[AD[OP]]);break;

    case Rl: J=*B++;PC+=2+((J<0x80)? J:(J-256)); 
             sprintf(S,"%s $%08lX",MN[AD[OP]],PC);break;

    case Im: sprintf(S,"%s #$%02X",MN[AD[OP]],*B++);break;
    case Zp: sprintf(S,"%s $%02X",MN[AD[OP]],*B++);break;
    case Zx: sprintf(S,"%s $%02X,x",MN[AD[OP]],*B++);break;
    case Zy: sprintf(S,"%s $%02X,y",MN[AD[OP]],*B++);break;
    case Ix: sprintf(S,"%s ($%02X,x)",MN[AD[OP]],*B++);break;
    case Iy: sprintf(S,"%s ($%02X),y",MN[AD[OP]],*B++);break;

    case Ab: sprintf(S,"%s $%04X",MN[AD[OP]],B[1]*256+B[0]);B+=2;break;
    case Ax: sprintf(S,"%s $%04X,x",MN[AD[OP]],B[1]*256+B[0]);B+=2;break;
    case Ay: sprintf(S,"%s $%04X,y",MN[AD[OP]],B[1]*256+B[0]);B+=2;break;
    case In: sprintf(S,"%s ($%04X)",MN[AD[OP]],B[1]*256+B[0]);B+=2;break;

    default: sprintf(S,".db $%02X\t\t; <Invalid OPcode>",OP/2);
  }
  return(B-A);
}

/** main() ****************************************************/
/** This is the main function from which execution starts.   **/
/**************************************************************/
int main(int argc,char *argv[])
{
  FILE *F;
  int N,I,J;
  byte Buf[32];
  char S[128];

  Counter=0L;PrintHex=0;

  for(N=1;(N<argc)&&(*argv[N]=='-');N++)
    switch(argv[N][1])
    {
      case 'o': sscanf(argv[J],"-o%lx",&Counter);
                Counter&=0xFFFFFFFFL;break;
      default: 
        for(J=1;argv[N][J];J++)
          switch(argv[N][J])
          {
            case 'h': PrintHex=1;break;
            default:
              fprintf(stderr,"%s: Unknown option -%c\n",argv[0],argv[N][J]);
          }
    }

  if(N==argc)  
  {
    fprintf(stderr,"DASM6502 6502 Disassembler v.2.0 by Marat Fayzullin\n");
#ifdef ZLIB
    fprintf(stderr,"  This program will transparently uncompress singular\n");
    fprintf(stderr,"  GZIPped and PKZIPped files.\n");
#endif
    fprintf(stderr,"Usage: %s [-h] [-oOrigin] file\n",argv[0]);
    fprintf(stderr,"  -h - Print hexadecimal values\n");
    fprintf(stderr,"  -o - Count addresses from a given origin (hex)\n");
    return(1);
  }
    
  if(!(F=fopen(argv[N],"rb")))
  { printf("\n%s: Can't open file %s\n",argv[0],argv[N]);return(1); }
    
  for(N=0;N+=fread(Buf+N,1,16-N,F);)
  {
    memset(Buf+N,0,32-N);
    I=DAsm(S,Buf,Counter);
    printf("%08lX:",Counter);
    if(PrintHex)
    {
      for(J=0;J<I;J++) printf(" %02X",Buf[J]);
      if(I<3) printf("\t");
    }
    printf("\t%s\n",S);
    Counter+=I;N-=I;
    if(N>0) for(J=0;J<N;J++) Buf[J]=Buf[16-N+J];
    else if(N<0) N=0;
  }
    
  fclose(F);return(0);
}
