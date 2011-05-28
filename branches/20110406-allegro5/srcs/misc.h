//-----------------------------------------------------------------------------
// MEKA - misc.h
// Miscellaneous - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    Close_Button_Callback   (void);
void    Switch_In_Callback      (void);
void    Switch_Out_Callback     (void);

void    Change_System_Misc      (void);
void    Change_Mode_Misc        (void); // Do various things when changing mode

void    Show_End_Message        (void);

void    Quit (void);
void    Quit_Msg (const char *format, ...)      FORMAT_PRINTF (1);

template<typename T>
static inline T Clamp(T v, T mn, T mx)
{
	if (v < mn) return mn;
	if (v > mx) return mx;
	return v;
}

static inline int LinearRemap(int src, int src_min, int src_max, int dst_min, int dst_max)
{
	const float t = (float)(src - src_min) / (float)(src_max - src_min);
	return dst_min + (dst_max - dst_min) * t;
}

static inline int LinearRemapClamp(int src, int src_min, int src_max, int dst_min, int dst_max)
{
	src = Clamp<int>(src, src_min, src_max);

	const float t = (float)(src - src_min) / (float)(src_max - src_min);
	return dst_min + (dst_max - dst_min) * t;
}

//-----------------------------------------------------------------------------
