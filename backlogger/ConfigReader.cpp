#include "ConfigReader.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <vector>


bool ConfigReader::ConfigCreator() const
{
	std::string ConfigDir = ConfigPath.substr(0, ConfigPath.find_last_of("/"));

	try
	{
		if (boost::filesystem::create_directory(ConfigDir))
			std::cout << "....Successfully Created !" << std::endl;
	}
	catch (const boost::filesystem::filesystem_error& e)
	{
		if (e.code() == boost::system::errc::permission_denied)
			std::cout << "Can't create directory for config. No permissions." << ConfigPath << "\n";
	}
	std::ofstream cfg(ConfigPath.c_str());
	if (cfg.is_open())
	{
		cfg << "#This is an automatically generated config template." << std::endl
			<< "#For comments, you can use \"" << std::endl
			<< "#Be sure to follow the examples of configuring the config, this affects the working capacity of the program." << std::endl
			<< "#PatchForSave: /etc/backupsys" << std::endl
			<< "PatchForSave: /etc/backupsys" << std::endl
			<< "#Be sure to specify the file types for the package, like that \"" << std::endl
			<< "BackupedPaths:" << std::endl
			<< "log,txt | /root/ProgrammsTest/logs";
		cfg.close();
		return true;
	}
	cfg.close();
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
}

void ConfigReader::ConfError(std::string str)
{
	std::cout << "ERROR: " << str << std::endl;
	exit(1);
}


void ConfigReader::Reader()
{
	bool pfs = false;
	std::ifstream cfg(ConfigPath.c_str());
	int line = 0;//Iterator of lines in a file

	//Begin to read the config
	while (!cfg.eof())
	{
		line++;

		char buff[1024];
		cfg.getline(buff, 1024);
		std::string buffer = buff;

		// Ignoring comments
		if(Trimmer(buffer)[0] == '#')
			continue;

		
		if (Trimmer(boost::to_lower_copy(buffer.substr(0, buffer.find(":")))) == "patchforsave")
		{
			PatchForSave = boost::trim_copy(buffer.substr(buffer.find(":") + 1, buffer.length()));
			PatchForSave = PatchForSave.substr(PatchForSave.find("/"), PatchForSave.length());
			CheckLastSlash(&PatchForSave);
			std::cout << PatchForSave << std::endl;
			if(!boost::filesystem::exists(PatchForSave))
			{
				std::cout << "Directory for save not found here: " << PatchForSave << ".Create it ? (yes / no)" << std::endl;
				std::string answer;
				std::cin >> answer;
				if(boost::to_lower_copy(answer) == "yes")
				{
					try
					{
						if (boost::filesystem::create_directory(PatchForSave))
							std::cout << "....Successfully Created !" << std::endl;
					}
					catch (const boost::filesystem::filesystem_error& e)
					{
						if (e.code() == boost::system::errc::permission_denied)
							std::cout << "Can't create directory for config. No permissions." << ConfigPath << "\n";
					}
				}
				else
				{
					exit(0);
				}
			}

			pfs = true;
		}

		if(Trimmer(boost::to_lower_copy(buffer.substr(0, buffer.find_last_of(":")))) == "backupedpaths")
		{
			while (!cfg.eof())
			{
				char b[1024];
				cfg.getline(b, 1024);
				std::string buf = b;

				if (Trimmer(buffer)[0] == '#')
					continue;

				pathnode ptnode;
				std::string types = Trimmer(buf.substr(0, buf.find("|")));

				while (true)
				{
					if (types.substr(0, types.find(",")) != "" && types.find(",") != std::string::npos)
					{
						ptnode.types.push_back(types.substr(0, types.find(",")));
					}
					else
					{
						if (types.find(",") == std::string::npos)
						{
							ptnode.types.push_back(types);
							break;
						}

						ConfError(std::string("Can't read config.File type error. Have \"" + buf.substr(0, buf.find("|"))) + "\". In line " + std::to_string(line));
					}
					types = types.substr(types.find(",") + 1, types.length());
				}
				if(buf.substr(buf.find("|")), buf.length() >= 1)
				{
					ptnode.path = buf.substr(buf.find("|"));
				}

				Date.push_back(ptnode);
			}
		}
	}


	std::cout << "Test info: " << std::endl;
	for (auto el : Date)
	{
		std::cout << "Path: " << el.path << std::endl;
		for (auto el1 : el.types)
		{
			std::cout << el1 << std::endl;
		}
	}
	if(!pfs)
	{
		ConfError("Can't find \"PatchForSave:\". Check config!");
	}
}

std::string ConfigReader::Trimmer(std::string str)
{
	boost::erase_all(str, " ");
	return str;
}

std::string* ConfigReader::CheckLastSlash(std::string* str)
{
	if (str->find_last_of("/") == str->length()-1)
		*str = str->substr(0, str->find_last_of("/"));
	return str;
}


