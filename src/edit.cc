/*  Edit Intel Hex files

    Copyright 2009 Brandon Fosdick (BSD License)
*/

#include <fstream>
#include <vector>

#include <getopt.h>

#include <intelhex.h>

typedef	intelhex::address_type address_type;
typedef	intelhex::value_type value_type;
typedef	std::pair<address_type, address_type> address_range_type;
typedef std::vector<value_type>	hold_space_type;

std::vector<std::string> commands;
hold_space_type	hold;

intelhex::container data;

void usage()
{
    printf("usage: inhx_edit [-e command] [file ...]\n");
}

address_range_type parse_address(std::string& address)
{
    // Single address or an address range?
    const size_t index = address.find("..");
    if( std::string::npos == index )
    {
	address_range_type range;
	range.first = strtoul(address.c_str(), NULL, 0);
	range.second = range.first;
	return range;
    }
    return std::make_pair(strtoul(address.substr(0,index).c_str(), NULL, 0), strtoul(address.substr(index+2).c_str(), NULL, 0));
}

void exchange(address_range_type& range)
{
    hold_space_type temp;
    hold_space_type::iterator j = hold.begin();
    for(address_type i = range.first; i <= range.second; ++i)
    {
	temp.push_back(data.get(i));
	if( j != hold.end() )
	    data.set(i, *j++);
	else
	    data.erase(i);
    }
    hold = temp;
}

int main(int argc, char* argv[])
{
    int ch;

    while( (ch = getopt_long(argc, argv, "e:", NULL, NULL)) != -1)
    {
	switch(ch)
	{
	    case 'e':
		commands.push_back(optarg);
		break;
	    default:
		usage();
		return 1;
	}
    }
    argc -= optind;
    argv += optind;

    for(unsigned i=0; i<argc; ++i)
    {
	data.clear();
	data.load(argv[i]);
	hold.clear();
	std::vector<std::string>::iterator j = commands.begin();
	for(; j != commands.end(); ++j)
	{
	    if( (*j)[0] == '/' )	// Look for address restriction
	    {
		size_t index = j->find('/', 1);	// Find the end of the address
		if( (std::string::npos == index) || (1 == index) )
		{
		    std::cerr << "Missing address terminator\n";
		    continue;
		}
		std::string address(*j, 1, index-1);
		// Look for illegal characters
		if( address.find_first_not_of(".x0123456789abcdefABCDEF") !=
		    std::string::npos )
		{
		    std::cerr << "Bad address\n";
		    continue;
		}
		address_range_type range = parse_address(address);
		if( range.first > range.second )
		    std::swap(range.first, range.second);
		// Check for a command character
		if( index >= j->size() )
		    continue;
		// Handle the command
		switch( (*j)[++index] )
		{
		    case 'x':	// Exchange hold space with pattern space
			exchange(range);
			break;
		    default:
			std::cerr << "Unrecognized command '" << (*j)[index] << "'\n";
			break;
		}
	    }
	}
 	data.write(std::cout);
    }

    return 0;
}
