#pragma once
#include <boost/filesystem.hpp>
#include <fstream>


class ConfigReader
{

	std::string ConfigPath;

	bool ConfigCreator() const;
	void CheckConfig() const;
public:

	ConfigReader(const std::string& config_path) : ConfigPath(config_path)
	{
		CheckConfig();
	}
};
