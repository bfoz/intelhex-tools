/*  Concatenate Intel Hex files

    Copyright 2009 Brandon Fosdick (BSD License)
*/

#include <fstream>

#include <getopt.h>

#include <intelhex.h>

intelhex::container data;

void usage()
{
    printf("usage: inhx_cat [-nu] [file ...]\n\n"
	   "  -n\tNumber the output lines, starting at 1 (not implemented)\n"
	   "  -u\tDisable output buffering (not implemented)\n\n");
}

int main(int argc, char* argv[])
{
    int ch;

    while( (ch = getopt_long(argc, argv, "nu", NULL, NULL)) != -1)
    {
	switch(ch)
	{
	    case 'n':
	    case 'u':
		break;
	    default:
		usage();
		return 1;
	}
    }
    argc -= optind;
    argv += optind;

    // Read from stdin if it's a pipe or if no input files were given
    if( !isatty(STDIN_FILENO) || (0 == argc) )
	data.read(std::cin);

    for(unsigned i=0; i<argc; ++i)
    {
	if( 0 == strncmp("-", argv[i], sizeof("-")) )
	    data.read(std::cin);
	else
	    data.load(argv[i]);
    }

    data.write(std::cout);

    return 0;
}