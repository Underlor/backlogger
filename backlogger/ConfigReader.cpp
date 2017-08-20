#include "ConfigReader.h"



bool ConfigReader::ConfigCreator() const
{
	std::string ConfigDir = ConfigPath.substr(0, ConfigPath.find_last_of("/"));
	boost::filesystem::path dir(ConfigDir);

	if (boost::filesystem::create_directory(ConfigDir))
			std::cout << "....Successfully Created !" << std::endl;
	std::ofstream cfg(ConfigPath.c_str());
	if (cfg.is_open())
	{
		cfg << "#This is an automatically generated config template.\n"
			<< "#For comments, you can use \"#\" or \":\"\n"
			<< "#Be sure to follow the examples of configuring the config, this affects the working capacity of the program.\n"
			<< "#PatchForSave = /etc/backupsys\n"
			<< "PatchForSave = /etc/backupsys\n"
			<< "#Be sure to specify the file types for the package, like that \".log.txt = path to the folder with logs | \"\n"
			<< ".log.txt = /root/ProgrammsTest/logs";
		return true;
	}
	else
		return false;
}

void ConfigReader::CheckConfig() const
{
	if (!std::ifstream(ConfigPath.c_str()).is_open())
	{
		std::cout << "Cant find config file here: " << ConfigPath << std::endl;
		std::cout << "Create new config?(y/n)" << std::endl;
		std::string answer;
		std::cin >> answer;
		if (answer == "yes" || answer == "y" || answer == "Y")
		{
			std::cout << "Wait. We creating new config..." << std::endl;
			if (ConfigCreator())
			{
				std::cout << "We create default config. Check and configure it here:" << ConfigPath << std::endl;
			}
			else
			{
				std::cout << "We had some problems when creating the config. Check the access rights of the program." << std::endl;
			}
		}
		else
		{
			std::cout << "The configuration was not created, create it manually." << std::endl;
		}
		exit(1);
	}
	exit(0);
}
