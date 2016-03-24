#ifndef OSXHELPERS_H_
#define OSXHELPERS_H_

void GetResourcePath( char* outBuffer, int bufferSize );
void GetWritableInternalResourcePath( char *outBuffer, int bufferSize );
void GetWritableExternalResourcePath( char *outBuffer, int bufferSize );
void PopulateWritableInternalResourcesPath( const char *path , const char **files );


#endif
