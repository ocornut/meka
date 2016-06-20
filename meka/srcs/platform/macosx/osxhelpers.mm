// Mac OS 
#include <CoreFoundation/CoreFoundation.h>
#import <Foundation/Foundation.h>
#include "osxhelpers.h"

static void NSStringToCString(NSString *s, char *out, size_t outSize);
static BOOL MakeDirectoryIfAbsent(NSURL *url, NSError **error);
static void CopyPathToBuffer(NSSearchPathDirectory searchPath, NSString *subDirectory, char *outBuffer, size_t bufferSize);
static NSURL* strToNSURL(const char *str);
static NSString *strToNSString(const char *str);

static void NSStringToCString(NSString *s, char *out, size_t outSize)
{
    [s getCString:out
        maxLength:outSize
         encoding:NSUTF8StringEncoding];
}

static NSString *strToNSString(const char *str)
{
    return [NSString stringWithCString:str
                              encoding:NSUTF8StringEncoding];
}

static NSURL* strToNSURL(const char *str)
{
    return [NSURL URLWithString:strToNSString(str)];

}

void GetResourcePath( char* outBuffer, int bufferSize )
{
    @autoreleasepool {
        NSStringToCString([[NSBundle mainBundle] resourcePath], 
                          outBuffer, 
                          bufferSize);
    }
}

static BOOL MakeDirectoryIfAbsent(NSURL *url, NSError **error)
{
    BOOL isDirectory;
    NSFileManager *fileManager  = [NSFileManager defaultManager];
    if (![fileManager fileExistsAtPath:[url path]
                           isDirectory:&isDirectory]) {
        return [fileManager createDirectoryAtURL:url
                     withIntermediateDirectories:YES
                                      attributes:[NSDictionary dictionary]
                                           error:error];
    }
    return YES;
}

static void CopyPathToBuffer(NSSearchPathDirectory searchPath, NSString *subDirectory, char *outBuffer, size_t bufferSize)
{
	NSURL *url = [[[NSFileManager defaultManager] URLsForDirectory:searchPath
					                                     inDomains:NSUserDomainMask] firstObject];
    url = [url URLByAppendingPathComponent:subDirectory];
    if (url != nil) {
        NSError *error;
        if (!MakeDirectoryIfAbsent(url, &error)) {
            NSLog(@"Error while creating: %@: %@", url, error);
            GetResourcePath( outBuffer, bufferSize );
        }
        else {
            NSStringToCString([url path], outBuffer, bufferSize);
        }
    }
    else {
        GetResourcePath( outBuffer, bufferSize );
    }
}

void PopulateWritableInternalResourcesPath( const char *path , const char **files)
{
    @autoreleasepool {
        NSError *error = nil; // used to store errors during copying
        NSString *destPath = strToNSString(path);
        NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
        const char **itr = files;
        NSFileManager *fm = [NSFileManager defaultManager];
        while (*itr) {
            NSString *filename = strToNSString(*itr);
            NSString *srcFile = [resourcePath stringByAppendingPathComponent:filename];
            NSString *destFile = [destPath stringByAppendingPathComponent:filename];
            if (![fm fileExistsAtPath:destFile]) {
                BOOL copyOK = [fm copyItemAtPath:srcFile
                                          toPath:destFile
                                           error:&error];
                if (!copyOK) {
                    NSLog(@"Error while copying %@ to %@ (%@)",
                          srcFile,
                          destPath,
                          error);
                }

            }
            ++itr;
        }
    }
}


void GetWritableInternalResourcePath( char *outBuffer, int bufferSize )
{
    @autoreleasepool {
        CopyPathToBuffer(NSApplicationSupportDirectory,
                         [[NSBundle mainBundle] bundleIdentifier],
                         outBuffer,
                         bufferSize);
    }
}

void GetWritableExternalResourcePath( char *outBuffer, int bufferSize )
{
    @autoreleasepool {
        CopyPathToBuffer(NSDocumentDirectory,
                         @"Meka",
                         outBuffer,
                         bufferSize);
    }
}
