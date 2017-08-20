#include "config_reader.h"
#include <fstream>
#include <vector>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/erase.hpp>
#include "boost/date_time/posix_time/posix_time.hpp" //include all types plus i/o


void config_reader::KeyChecker(int argc, char* argv[])
{
	ignore_ = noanswer_ = newcfg_ = false;
	for (int i = 0; i < argc; ++i)
	{
		std::string arg = boost::to_lower_copy(std::string(argv[i]));
		if (arg == "-ignore" || arg == "-i")
			ignore_ = true;
		if (arg == "-noanswer" || arg == "-na")
			noanswer_ = true;
		if (arg == "-newcfg" || arg == "-nc")
			newcfg_ = true;
	}
}

config_reader::config_reader(const std::string& config_path, int argc, char* argv[]): config_path_(config_path)
{

	KeyChecker(argc, argv);
	if (newcfg_)
	{
		if (config_creator())
		{
			std::cout << "We create default config. Check and configure it here:" << config_path_ << std::endl;
		}
		else
		{
			std::cout << "We had some problems when creating the config. Check the access rights of the program." << std::endl;
		}
		exit(0);
	}
	if (!noanswer_)
	{
		check_config();
		parce_config();
	}
	else
	{
		std::string ConfigDir = config_path_.substr(0, config_path_.find_last_of("/"));
		std::string logger_name = ConfigDir + "/" + boost::posix_time::to_simple_string(boost::posix_time::second_clock::local_time()) + ".log";
		logger_name.replace(logger_name.find(" "), 1, "_");
		logger = new std::ofstream(logger_name);
		check_config_na();
		parce_config_na();
	} 


}


std::vector<config_reader::patch_info> config_reader::backuped_paths() const
{
	return backuped_paths_;
}


bool config_reader::config_creator() const
{
	// Получаем путь до дирректории конфига
	std::string ConfigDir = config_path_.substr(0, config_path_.find_last_of("/")); 

	// Пытаемся создать дирректорию конфига
	try
	{
		if (boost::filesystem::create_directory(ConfigDir))
			std::cout << "....Successfully Created !" << std::endl;
	}
	catch (const boost::filesystem::filesystem_error& e)
	{
		if (e.code() == boost::system::errc::permission_denied)
			std::cout << "Can't create directory for config. No permissions." << config_path_ << "\n";
	}

	// Создаем дефотный конфиг
	std::ofstream cfg(config_path_.c_str());
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

void config_reader::check_config() const
{
	//Проверяем возможность записи в конфиг
	if (!std::ifstream(config_path_.c_str()).is_open())
	{
		// Интересуемся создать ли конфиг
		std::cout << "Cant find config file here: " << config_path_ << std::endl;
		std::cout << "Create new config?(y/n)" << std::endl;
		std::string answer;
		std::cin >> answer;
		if (answer == "yes" || answer == "y" || answer == "Y")
		{
			std::cout << "Wait. We creating new config..." << std::endl;
			if (config_creator())
			{
				std::cout << "We create default config. Check and configure it here:" << config_path_ << std::endl;
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

void config_reader::check_config_na() const
{
	//Проверяем возможность записи в конфиг
	if (!std::ifstream(config_path_.c_str()).is_open())
	{
		// Интересуемся создать ли конфиг
		*logger << "Cant find config file here: " << config_path_ << std::endl;
		*logger << "Create new config?(y/n)" << std::endl;
		std::string answer;
		std::cin >> answer;
		if (answer == "yes" || answer == "y" || answer == "Y")
		{
			*logger << "Wait. We creating new config..." << std::endl;
			if (config_creator())
			{
				*logger << "We create default config. Check and configure it here:" << config_path_ << std::endl;
			}
			else
			{
				*logger << "We had some problems when creating the config. Check the access rights of the program." << std::endl;
			}
		}
		else
		{
			*logger << "The configuration was not created, create it manually." << std::endl;
		}
		exit(1);
	}
}

void config_reader::fatal_error(std::string str)
{
	std::cout << "ERROR: " << str << std::endl;
	exit(1);
}


void config_reader::parce_config()
{
	bool is_patch_for_save_ = false;//To check if there is a save path
	std::ifstream cfg(config_path_.c_str());
	int line = 0;//Iterator of lines in a file

				 //Begin to read the config
	while (!cfg.eof())
	{
		line++;

		char buff[1024];
		cfg.getline(buff, 1024);
		std::string buffer = buff;

		// Игнорируем комментарии
		if (trimmer(buffer)[0] == '#')
			continue;
		// 
		if (trimmer(boost::to_lower_copy(buffer.substr(0, buffer.find(":")))) == "patchforsave")
		{
			//Found a PathForSave
			patch_for_save_ = boost::trim_copy(buffer.substr(buffer.find(":") + 1, buffer.length()));
			patch_for_save_ = patch_for_save_.substr(patch_for_save_.find("/"), patch_for_save_.length());
			check_last_slash(&patch_for_save_);
			//Получили путь для сохранения. Проверяем есть ли по этому пути папка.
			if (!boost::filesystem::exists(patch_for_save_))
			{
				std::cout << "Directory for save not found here: " << patch_for_save_ << ".Create it ? (yes / no)" << std::endl;
				std::string answer;
				std::cin >> answer;

				if (boost::to_lower_copy(answer) == "yes")
				{
					try
					{
						if (boost::filesystem::create_directory(patch_for_save_))
							std::cout << "....Successfully Created !" << std::endl;
					}
					catch (const boost::filesystem::filesystem_error& e)
					{
						if (e.code() == boost::system::errc::permission_denied)
							std::cout << "Can't create directory for the config. No permissions." << config_path_ << "\n";
					}
				}
				else
				{
					exit(0);
				}
			}

			is_patch_for_save_ = true;
		}
		// Читаем пути
		if (trimmer(boost::to_lower_copy(buffer.substr(0, buffer.find_last_of(":")))) == "backupedpaths")
		{
			while (!cfg.eof())
			{
				line++;

				char buff_[1024];
				cfg.getline(buff_, 1024);
				std::string buffer_ = buff_;

				// Игнорируем комментарии
				if (trimmer(buffer)[0] == '#')
					continue;

				patch_info ptnode;
				// Проверяем
				if (buffer_.find("|") == std::string::npos || trimmer(buffer_.substr(0, buffer_.find("|"))) == "")
				{
					if (!ignore_)
						fatal_error(std::string("Can't read config. No file type specified. Line \"" + buffer_ + "\". In line " + std::to_string(line)));
					else
						continue;
				}

				// Парсим типы файлов
				std::string types = trimmer(buffer_.substr(0, buffer_.find("|")));
				bool err = false;
				while (true)
				{
					// Проверка на дурака с запятой в конце перечисления типов
					if (types[types.find_last_of(",") + 1] == '\0')
					{
						if (!ignore_)
							fatal_error(std::string("Can't read config.File type error. Have \"" + buffer_.substr(0, buffer_.find("|")) + "\". In line " + std::to_string(line)));
						else
						{
							err = true;
							break;
						}
					}
					// Пока не пусто и пока есть запятые
					if (types.substr(0, types.find(",")) != "" && types.find(",") != std::string::npos)
					{
						ptnode.types.push_back("." + types.substr(0, types.find(",")));
					}
					else
					{
						// если нет запятых
						if (types.find(",") == std::string::npos)
						{
							ptnode.types.push_back("." + types);
							break;
						}
						// если получили пустую строку выбрасываем ошибку
						if (!ignore_)
							fatal_error(std::string("Can't read config.File type error. Have \"" + buffer_.substr(0, buffer_.find("|")) + "\". In line " + std::to_string(line)));
						else
						{
							err = true;
							break;
						}
					}
					types = types.substr(types.find(",") + 1, types.length());
				}
				if (err)
					continue;
				// Парсим путь до папки с файлами
				if (trimmer(buffer_.substr(buffer_.find("|") + 1, buffer_.length())) != "")
				{
					if (!boost::filesystem::exists(buffer_.substr(buffer_.find("/"), buffer_.length())))
					{
						if (!ignore_)
							fatal_error(std::string("Specified directory not found. Have \"" + buffer_.substr(buffer_.find("/"), buffer_.length()) + "\". In line " + std::to_string(line)));
						else
							continue;
					}
					ptnode.path = buffer_.substr(buffer_.find("/"), buffer_.length());
				}
				else
					if (!ignore_)
						fatal_error(std::string("Can't read config.File type error. Have \"" + buffer_ + "\". In line " + std::to_string(line)));
					else
						continue;

				// Добавляем наш путь в список
				backuped_paths_.push_back(ptnode);
			}
		}
	}

	if (!is_patch_for_save_)//Check if the save path was found
	{
		fatal_error("Can't find \"PatchForSave:\". Check config!");
	}
}

void config_reader::parce_config_na()
{
	bool is_patch_for_save_ = false;//To check if there is a save path
	std::ifstream cfg(config_path_.c_str());
	int line = 0;//Iterator of lines in a file

				 //Begin to read the config
	while (!cfg.eof())
	{
		line++;

		char buff[1024];
		cfg.getline(buff, 1024);
		std::string buffer = buff;

		// Игнорируем комментарии
		if (trimmer(buffer)[0] == '#')
			continue;
		// 
		if (trimmer(boost::to_lower_copy(buffer.substr(0, buffer.find(":")))) == "patchforsave")
		{
			//Found a PathForSave
			patch_for_save_ = boost::trim_copy(buffer.substr(buffer.find(":") + 1, buffer.length()));
			patch_for_save_ = patch_for_save_.substr(patch_for_save_.find("/"), patch_for_save_.length());
			check_last_slash(&patch_for_save_);
			//Получили путь для сохранения. Проверяем есть ли по этому пути папка.
			if (!boost::filesystem::exists(patch_for_save_))
			{
				*logger << "Directory for save not found here: " << patch_for_save_ << std::endl;
				exit(0);
			}

			is_patch_for_save_ = true;
		}
		// Читаем пути
		if (trimmer(boost::to_lower_copy(buffer.substr(0, buffer.find_last_of(":")))) == "backupedpaths")
		{
			while (!cfg.eof())
			{
				line++;

				char buff_[1024];
				cfg.getline(buff_, 1024);
				std::string buffer_ = buff_;

				// Игнорируем комментарии
				if (trimmer(buffer)[0] == '#')
					continue;

				patch_info ptnode;
				// Проверяем
				if (buffer_.find("|") == std::string::npos || trimmer(buffer_.substr(0, buffer_.find("|"))) == "")
				{
					*logger << std::string("Can't read config. No file type specified. Line \"" + buffer_ + "\". In line " + std::to_string(line)) << " IGNORED" << std::endl;
				}

				// Парсим типы файлов
				std::string types = trimmer(buffer_.substr(0, buffer_.find("|")));
				bool err = false;
				while (true)
				{
					// Проверка на дурака с запятой в конце перечисления типов
					if (types[types.find_last_of(",") + 1] == '\0')
					{
						*logger << std::string("Can't read config.File type error. Have \"" + buffer_.substr(0, buffer_.find("|")) + "\". In line " + std::to_string(line)) << " IGNORED" << std::endl;
						err = true;
						break;
					}
					// Пока не пусто и пока есть запятые
					if (types.substr(0, types.find(",")) != "" && types.find(",") != std::string::npos)
					{
						ptnode.types.push_back("." + types.substr(0, types.find(",")));
					}
					else
					{
						// если нет запятых
						if (types.find(",") == std::string::npos)
						{
							ptnode.types.push_back("." + types);
							break;
						}
						// если получили пустую строку выбрасываем ошибку
						*logger << std::string("Can't read config.File type error. Have \"" + buffer_.substr(0, buffer_.find("|")) + "\". In line " + std::to_string(line)) << " IGNORED" << std::endl;
						err = true;
						break;
					}
					types = types.substr(types.find(",") + 1, types.length());
				}
				if (err)
					continue;
				// Парсим путь до папки с файлами
				if (trimmer(buffer_.substr(buffer_.find("|") + 1, buffer_.length())) != "")
				{
					if (!boost::filesystem::exists(buffer_.substr(buffer_.find("/"), buffer_.length())))
					{

						*logger << std::string("Specified directory not found. Have \"" + buffer_.substr(buffer_.find("/"), buffer_.length()) + "\". In line " + std::to_string(line)) << " IGNORED" << std::endl;
						continue;
					}
					ptnode.path = buffer_.substr(buffer_.find("/"), buffer_.length());
				}
				else
				{
					*logger << std::string("Can't read config.File type error. Have \"" + buffer_ + "\". In line " + std::to_string(line)) << " IGNORED" << std::endl;
					continue;
				}
				// Добавляем наш путь в список
				backuped_paths_.push_back(ptnode);
			}
		}
	}

	if (!is_patch_for_save_)//Check if the save path was found
	{
		fatal_error("Can't find \"PatchForSave:\". Check config!");
	}
}

std::string config_reader::trimmer(std::string str)
{
	boost::erase_all(str, " ");
	return str;
}

std::string* config_reader::check_last_slash(std::string* str)
{
	if (str->find_last_of("/") == str->length()-1)
		*str = str->substr(0, str->find_last_of("/"));
	return str;
}