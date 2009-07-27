/*  Display leading bytes of Intel Hex files

    Copyright 2009 Brandon Fosdick (BSD License)
*/

#include <fstream>

#include <getopt.h>

#include <intelhex.h>

intelhex::container data;

void usage()
{
    printf("usage: inhx_head [-a addresses | -c bytes] [file ...]\n");
}

int main(int argc, char* argv[])
{
    int ch;
    bool bytes = true;
    unsigned count = 10;

    while( (ch = getopt_long(argc, argv, "a:c:", NULL, NULL)) != -1)
    {
	switch(ch)
	{
	    case 'a':
		bytes = false;
		count = strtoul(optarg, NULL, 0);
		break;
	    case 'c':
		bytes = true;
		count = strtoul(optarg, NULL, 0);
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

    // count can either be the number of addresses to keep, or the number of bytes
    //  If it's the number of bytes, find the address range that contains
    //  the desired number of set addresses.
    if( bytes )
    {
	intelhex::address_type first = data.min_address()+count;
	const intelhex::address_type max = data.max_address();
	while( (data.size_below_addr(first) < count) && (first < max) )
	    ++first;
	data.erase(first, max);
    }
    else    // Keep count addresses, starting at the first set address
	data.erase(data.min_address()+count, data.max_address());

    data.write(std::cout);

    return 0;
}
