//-----------------------------------------------------------------------------
// MEKA - eagle.h
// Eagle Graphic Filter - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

extern void eagle       (unsigned long *lb,
                         unsigned long *lb2,
                         short width,
                         int destination_segment,
                         void *screen_address1,  // int
                         void *screen_address2); // int

extern void eagle_mmx16 (unsigned long *lb,
                         unsigned long *lb2,
                         short width,
                         int destination_segment,
                         void *screen_address1,
                         void *screen_address2);

extern void eagle_bmp   (unsigned long *lb,
                         unsigned long *lb2,
                         short width,
                         int destination_segment,
                         void *screen_address1,
                         void *screen_address2);

//-----------------------------------------------------------------------------

