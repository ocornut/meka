//-----------------------------------------------------------------------------
// MEKA - tools.h
// Various tools - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Path Functions
//-----------------------------------------------------------------------------

void    StrPath_GetExtension(char* buf);
void    StrPath_RemoveExtension(char* buf);

// Extract filename from 'src' and copy it to location 'dst'
void    StrPath_RemoveDirectory(char* buf);
void    StrPath_RemoveDirectory(char* dst, const char* src);

//-----------------------------------------------------------------------------

