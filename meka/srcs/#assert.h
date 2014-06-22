#if 0
//-----------------------------------------------------------------------------
// MEKA - assert.h
// Assert functions
//-----------------------------------------------------------------------------

// Main Asserts (Always Enabled)
#define ASSERT_MSG_ALWAYS(expr,desc)	ASSERT_(expr,desc,__FILE__,__FUNCTION__,__LINE__)
#define ASSERT_ALWAYS(expr)				ASSERT_(expr,"",__FILE__,__FUNCTION__,__LINE__)

// Always enabled for now
// FIXME-OPT
#define ASSERT_ENABLED

// Debug-Enabled Asserts
#ifdef  ASSERT_ENABLED
#define ASSERT_MSG						ASSERT_MSG_ALWAYS
#define ASSERT							ASSERT_ALWAYS
#else
#define ASSERT_MSG(expr,desc)			((void)0)
#define ASSERT(expr)					((void)0)
#endif

//---------------------------------------------------------------------------
// NITRO ASSERT macro
//---------------------------------------------------------------------------

#ifdef ARCH_NITRO

inline void StopCPU()
{
	asm { bkpt 0 };
}

#define ASSERT_(expr,desc,file,func,line)				\
do														\
{														\
	if (!(expr))										\
	{													\
		C_Assert::Assert(#expr, func, file, line, desc);\
	}													\
} while (0)

#endif // ARCH_NITRO

//---------------------------------------------------------------------------
// WIN32 ASSERT macro
//---------------------------------------------------------------------------

#ifdef ARCH_WIN32

#define ASSERT_(expr,desc,file,func,line)	\
do											\
{											\
	static bool skip_all = false;			\
	if (!skip_all && !(expr))				\
	switch(C_Assert::Assert(#expr, func, file, line, desc))	\
	{										\
	case C_Assert::BREAK:					\
		_asm {int 3} break;					\
	case C_Assert::SKIP: break;				\
	case C_Assert::SKIP_ALL:				\
		skip_all = true; break;				\
	case C_Assert::STOP:					\
		/* this should not happen */		\
		break;								\
	}										\
} while (0)

#endif // ARCH_WIN32

#ifdef ARCH_LINUX

#define ASSERT_(expr,desc,file,func,line)	assert(expr)

#endif // ARCH_WIN32

//---------------------------------------------------------------------------
// GNU/LINUX ASSERT macro
//---------------------------------------------------------------------------

#ifdef ARCH_LINUX

#define ASSERT_(expr,desc,file,func,line)	assert(expr)

#endif // ARCH_WIN32

//---------------------------------------------------------------------------
// ASSERT
//---------------------------------------------------------------------------

class	C_Assert
{
	//-----------------------------------------------------------------------
	// Members
	//-----------------------------------------------------------------------
private:
	// Storage
#ifdef ARCH_OPENGL
	static const char *	m_Expr;
	static const char *	m_Desc;
	static const char *	m_Func;
	static const char *	m_File;
	static int			m_Line;
#endif // ARCH_WIN32

	//-----------------------------------------------------------------------
	// Static Methods
	//-----------------------------------------------------------------------
public:
	// Assert
	enum C_AssertResult
	{
		BREAK = 0,
		SKIP,
		SKIP_ALL,
		STOP
	};
	static C_AssertResult Assert(const char *expr, const char *func, const char *file, int line, const char *format, ...);

	// Static Print
	static const char *			StaticPrintf(const char *format, ...);
	static inline const char *	StaticPrintfDummy(...)							{ return ""; }

	//-----------------------------------------------------------------------
	// Private Methods
	//-----------------------------------------------------------------------
#ifdef ARCH_WIN32
private:
	static INT_PTR	AssertCallback(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
};

//---------------------------------------------------------------------------
#endif