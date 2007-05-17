#ifndef __DEFTYPES_H__
#define __DEFTYPES_H__

#define UBYTE	unsigned char	// Unsigned byte (       $00 to $FF      )
#define BYTE	signed char	// Signed byte   (      -$80 to $7F      )
#define UWORD	unsigned short	// Unsigned word (     $0000 to $FFFF    )
#define ULONG	unsigned int	// Unsigned long ( $00000000 to $FFFFFFFF)
#define CPTR	unsigned int	// 32-bit address pointer format

/* Already defined by SEAL */
// #define WORD    short       // Signed word   (    -$8000 to $7FFF    )
// #define LONG    int     // Signed long   (-$80000000 to $7FFFFFFF)
// #define BOOL    int     // Boolean (0=TRUE)

#endif
