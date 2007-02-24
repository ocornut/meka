CC      = gcc
CFLAGS  = -O7 -mpentium -DLSB_FIRST
#CFLAGS  = -O7 -mpentium -DLSB_FIRST -DDEBUG

dasm:	dasm.c
	${CC} -o dasm dasm.c

M6502.o:	M6502.c M6502.h Tables.h Codes.h
	${CC} -o M6502.c ${CFLAGS}
Debug.o:	Debug.c M6502.h
