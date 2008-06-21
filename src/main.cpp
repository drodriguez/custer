#include "custer.h"
#include "CusterServer.h"

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
		("directory,d", bpo::value<std::string>()->default_value(".", " dir. actual"),
			"directorio que se servira")
		("verbose,v", "imprime más informacion durante la ejecucion")
		("debug,D", "imprime informacion de depuracion");
	
	bpo::variables_map cmdline_variables;
	try {
		bpo::store(bpo::command_line_parser(argc, argv).
			options(custer_options).run(), cmdline_variables);
		bpo::notify(cmdline_variables);
	} catch (bpo::error& err) {
		fatal("%s", err.what()); 
	}
	
	if (cmdline_variables.count("help")) {
		std::cout << custer_options << std::endl;
		return 0;
	}
	
	if (cmdline_variables.count("verbose")) {
		logLevel = LOG_INFO;
	}
	if (cmdline_variables.count("debug")) {
		logLevel = LOG_DEBUG;
	}
	
	uint16_t port = cmdline_variables["port"].as<int>();
	std::string directory = cmdline_variables["directory"].as<std::string>();
	debug("Puerto: %u", port);
	debug("Directorio: '%s'", directory.c_str());
	
	boost::shared_ptr<custer::CusterServer> custerServer(
		new custer::CusterServer(port, directory));
	
#ifdef WIN32
	// Inicializamos WinSock
	WSADATA wsaData;
	int returnCode;
	if ((returnCode = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		fatal("Inicializando WinSock 2.2 (codigo: %d)", returnCode);
	}
	
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		WSACleanup();
		fatal("Version WinSock 2.2 no disponible (version disponible: %d.%d)",
			LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
	}
#endif
	
	std::cout << "Iniciando servidor..." << std::endl;
	custerServer->run();
	
	return 0;
}