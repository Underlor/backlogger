#include <boost/algorithm/string.hpp>
#include <fstream>
#include <list>
#include <iostream>
#include <string>
#include "ConfigReader.h"

using namespace boost::algorithm;

int main()
{	

	const std::string path = "/etc/backloger/config.conf";
	ConfigReader cfg(path);
	return 0;
}