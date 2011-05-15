//-----------------------------------------------------------------------------
// MEKA - errors.h
// Error codes and handling
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

// Error Codes ----------------------------------------------------------------
#define MEKA_ERR_OTHER                 (-1)
#define MEKA_ERR_OK                     (0)
#define MEKA_ERR_FAIL                   (1)
#define MEKA_ERR_MEMORY                 (2)
#define MEKA_ERR_SYNTAX                 (3)
#define MEKA_ERR_EMPTY                  (4)
#define MEKA_ERR_INCOHERENT             (5)
#define MEKA_ERR_FILE_OPEN              (6)
#define MEKA_ERR_FILE_READ              (7)
#define MEKA_ERR_FILE_WRITE             (8)
#define MEKA_ERR_FILE_CLOSE             (9)
#define MEKA_ERR_FILE_EMPTY             (10)
#define MEKA_ERR_MISSING                (11)
#define MEKA_ERR_UNKNOWN                (12)
#define MEKA_ERR_INCOMPLETE             (13)
#define MEKA_ERR_ZIP_NOT_SUPPORTED      (14)
#define MEKA_ERR_ZIP_LOADING            (15)
#define MEKA_ERR_ZIP_INTERNAL           (16)
#define MEKA_ERR_CONSOLE_WIN32_INIT     (17)
#define MEKA_ERR_CANCEL                 (18)
#define MEKA_ERR_ALREADY_DEFINED        (19)
#define MEKA_ERR_VALUE_OUT_OF_BOUND	    (20)
#define MEKA_ERR_VALUE_INCORRECT        (21)

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

extern int      meka_errno;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

const char *    meka_strerror(void);

#ifdef ARCH_WIN32
void            ShowWindowsErrorMessage(void);
#endif

//-----------------------------------------------------------------------------

