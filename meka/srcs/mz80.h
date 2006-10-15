/* Multi-Z80 32 Bit emulator */

/* Copyright 1999, Neil Bradley, All rights reserved
 *
 * License agreement:
 *
 * The mZ80 emulator may be distributed in unmodified form to any medium.
 *
 * mZ80 May not be sold, or sold as a part of a commercial package without
 * the express written permission of Neil Bradley (neil@synthcom.com). This
 * includes shareware.
 *
 * Modified versions of mZ80 may not be publicly redistributed without author
 * approval (neil@synthcom.com). This includes distributing via a publicly
 * accessible LAN. You may make your own source modifications and distribute
 * mZ80 in object only form.
 *
 * mZ80 Licensing for commercial applications is available. Please email
 * neil@synthcom.com for details.
 *
 * Synthcom Systems, Inc, and Neil Bradley will not be held responsible for
 * any damage done by the use of mZ80. It is purely "as-is".
 *
 * If you use mZ80 in a freeware application, credit in the following text:
 *
 * "Multi-Z80 CPU emulator by Neil Bradley (neil@synthcom.com)"
 *
 * must accompany the freeware application within the application itself or
 * in the documentation.
 *
 * Legal stuff aside:
 *
 * If you find problems with mZ80, please email the author so they can get
 * resolved. If you find a bug and fix it, please also email the author so
 * that those bug fixes can be propogated to the installed base of mZ80
 * users. If you find performance improvements or problems with mZ80, please
 * email the author with your changes/suggestions and they will be rolled in
 * with subsequent releases of mZ80.
 *
 * The whole idea of this emulator is to have the fastest available 32 bit
 * Multi-z80 emulator for the PC, giving maximum performance. 
 */ 

/* General z80 based defines */

#ifndef	_MZ80_H_
#define	_MZ80_H_

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef UINT32
#define UINT32  unsigned long int
#endif

#ifndef UINT16
#define UINT16  unsigned short int
#endif

#ifndef UINT8
#define UINT8   unsigned char
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _MEMORYREADWRITEBYTE_
#define _MEMORYREADWRITEBYTE_

struct MemoryWriteByte
{
	UINT32 lowAddr;
	UINT32 highAddr;
	void (*memoryCall)(UINT32, UINT8, struct MemoryWriteByte *);
	void *pUserArea;
};      

struct MemoryReadByte
{
	UINT32 lowAddr;
	UINT32 highAddr;
	UINT8 (*memoryCall)(UINT32, struct MemoryReadByte *);
	void *pUserArea;
};      

#endif // _MEMORYREADWRITEBYTE_

struct z80PortWrite
{
	UINT16 lowIoAddr;
	UINT16 highIoAddr;
	void (*IOCall)(UINT16, UINT8, struct z80PortWrite *);
	void *pUserArea;
};

struct z80PortRead
{
	UINT16 lowIoAddr;
	UINT16 highIoAddr;
	UINT16 (*IOCall)(UINT16, struct z80PortRead *);
	void *pUserArea;
};	

struct z80TrapRec
{
  	UINT16 trapAddr;
	UINT8  skipCnt;
	UINT8  origIns;
};

struct mz80context
{
	UINT8 *z80Base;
	struct MemoryReadByte *z80MemRead;
	struct MemoryWriteByte *z80MemWrite;
	struct z80PortRead *z80IoRead;
	struct z80PortWrite *z80IoWrite;
	UINT32 z80clockticks;
	UINT32 z80inInterrupt;
	UINT32 z80interruptMode;
	UINT32 z80interruptState;
	UINT32 z80halted;
	UINT16 z80af;
	UINT16 z80bc;
	UINT16 z80de;
	UINT16 z80hl;
	UINT16 z80afprime;
	UINT16 z80bcprime;
	UINT16 z80deprime;
	UINT16 z80hlprime;
	UINT16 z80ix;
	UINT16 z80iy;
	UINT16 z80sp;
	UINT16 z80pc;
	UINT16 z80nmiAddr;
	UINT16 z80intAddr;
	UINT8 z80i;
	UINT8 z80r;
} RETRO_PACKED ;

extern UINT8 *mz80Base;
extern UINT32 mz80exec(unsigned long int);
extern UINT32 mz80GetContextSize(void);
extern UINT32 mz80GetElapsedTicks(UINT32);
extern void mz80ReleaseTimeslice();
extern void mz80GetContext(void *);
extern void mz80SetContext(void *);
extern void mz80reset(void);
extern UINT32 mz80int(UINT32);
extern UINT32 mz80nmi(void);
extern UINT16 z80intAddr;
extern UINT16 z80nmiAddr;
extern UINT16 z80pc;

typedef struct mz80context CONTEXTMZ80;

#ifdef __cplusplus
};
#endif

#endif	// _MZ80_H_
