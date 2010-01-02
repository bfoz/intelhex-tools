#include <stdio.h>
#include <sstream>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>

#include <intelhex.h>

/* -----------------------------------------------------------------------------
   Generate a preview for file

   This function's job is to create preview for designated file
   ----------------------------------------------------------------------------- */

extern "C" OSStatus GeneratePreviewForURL(void *thisInterface, QLPreviewRequestRef preview, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options)
{
    // Translate the CFURL into a file path because libintelhex uses STL to open
    //  the file, which can't handle URLs
    char buffer[FILENAME_MAX];
    CFStringRef path = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
    if( !CFStringGetCString(path, buffer, sizeof(buffer), kCFStringEncodingASCII) )
    {
	CFRelease(path);
	return -1;
    }

    // Load the file and convert it into a string
    intelhex::container hex(buffer);
    std::ostringstream ss;
    hex.write(ss);

    // Convert the string to a CFData object
    CFDataRef data = CFDataCreate(NULL, (const UInt8*)ss.str().c_str(), ss.str().length());

    // Pass the CFData as text for display by the standard handler
    QLPreviewRequestSetDataRepresentation(preview, data, kUTTypePlainText, NULL);

    CFRelease(path);
    CFRelease(data);

    return noErr;
}

extern "C" void CancelPreviewGeneration(void* thisInterface, QLPreviewRequestRef preview)
{
    // implement only if supported
}
