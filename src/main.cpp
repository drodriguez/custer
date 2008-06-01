#include "custer.h"

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include <iostream>

namespace bpo = boost::program_options;

int main(int argc, char* argv[])
{
	bpo::options_description custer_options("Opciones permitidas");
	custer_options.add_options()
		("help,h", "muestra este mensaje de ayuda")
		("port,p", bpo::value<int>()->default_value(80, " 80"),
			"puerto al que asociarse")
		("directory,d", bpo::value<std::string>()->default_value("./", " ./"),
			"directorio que se servira")
		("verbose,v", "imprime más informacion durante la ejecucion")
		("debug,d", "imprime informacion de depuracion");
	
	bpo::variables_map cmdline_variables;
	bpo::store(bpo::command_line_parser(argc, argv).options(custer_options).
		run(), cmdline_variables);
	bpo::notify(cmdline_variables);
	
	if (cmdline_variables.count("help")) {
		std::cout << custer_options << std::endl;
		return 0;
	}
	
	if (cmdline_variables.count("verbose")) {
		logLevel = INFO;
	}
	if (cmdline_variables.count("debug")) {
		logLevel = DEBUG;
	}
	
	debug("Puerto: %d", cmdline_variables["port"].as<int>());
	
	debug("Directorio: %s", cmdline_variables["directory"].as<std::string>().c_str());
	
	return 0;
}