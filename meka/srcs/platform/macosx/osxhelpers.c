// Mac OS 
#include <CoreFoundation/CoreFoundation.h>

void GetResourcePath( char* outBuffer, int bufferSize )
{
	CFBundleRef mainBundle;
	// Get the main bundle for the app
	mainBundle = CFBundleGetMainBundle();
	CFURLRef resourceDirURL = CFBundleCopyResourceURL( mainBundle, CFSTR("Resources"), NULL, NULL );
	CFURLGetFileSystemRepresentation( resourceDirURL, true, (UInt8*)outBuffer, bufferSize );
}
