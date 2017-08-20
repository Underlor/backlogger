#pragma once
#include <boost/filesystem.hpp>
#include <fstream>
#include <vector>

struct pathnode
{
	std::vector<std::string> types;
	std::string path;
};

class ConfigReader
{
public:
	ConfigReader(const std::string& config_path) : ConfigPath(config_path)
	{
		CheckConfig();
		Reader();
	}

	std::string PatchForSave;

private:
	std::string ConfigPath;
	std::vector<pathnode> Date;
	bool ConfigCreator() const;
	void CheckConfig() const;
	static void ConfError(std::string str);
	void Reader();
	std::string Trimmer(std::string str);
	std::string* CheckLastSlash(std::string* str);
};
