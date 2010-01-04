#include <stdio.h>
#include <sstream>
#include <iomanip>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>

#include <intelhex.h>

#define	TIDY_WIDTH  16

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
    CFRelease(path);

    // Load the file and convert it into a canonical-style hexdump string
    intelhex::container hex(buffer);
    hex.compact();
    hex.tidy(TIDY_WIDTH);

    std::ostringstream ss;
    ss.setf(std::ios::hex, std::ios::basefield);	//Set the stream to ouput hex instead of decimal
    ss.setf(std::ios::uppercase);			//Use uppercase hex notation
    ss.fill('0');

    int width = 4;
    if( hex.max_address() > 0xFFFF )
	width = 8;

    intelhex::container::iterator i = hex.begin();
    for(; i != hex.end(); ++i)
    {
	// Block address
	ss.width(width);
	ss << (*i).first;

	// Hex output
	intelhex::container::data_container::iterator j = (*i).second.begin();
	for(; j != (*i).second.end(); ++j)
	{
	    ss << ' ';
	    ss.width(2);
	    ss << (int)(*j);
	}

	// Pad short blocks with spaces
	int k = TIDY_WIDTH - (*i).second.size();
	while( k-- )
	    ss << "   ";

	// ASCII output
	ss << " |";
	j = (*i).second.begin();
	for(; j != (*i).second.end(); ++j)
	{
	    if( isprint(*j) )
		ss << (*j);
	    else
		ss << '.';
	}
	ss << "|\n";
    }

    // Convert the string to a CFData object
    CFDataRef data = CFDataCreate(NULL, (const UInt8*)ss.str().c_str(), ss.str().length());

    // Pass the CFData as text for display by the standard handler
    QLPreviewRequestSetDataRepresentation(preview, data, kUTTypePlainText, NULL);

    CFRelease(data);

    return noErr;
}

extern "C" void CancelPreviewGeneration(void* thisInterface, QLPreviewRequestRef preview)
{
    // implement only if supported
}
