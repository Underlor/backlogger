/*
 * Программа принимает следующие ключи:
 * -ignore (-i) пропуск строки конфига с ошибкой
 * -noanswer(-na) ключь для выполнения программы по таймеру(Убедитесь, убедитесь в существовании конфига!)
 * -newcfg(-nc) пересоздает конфиг файл
 */

#include <boost/algorithm/string.hpp>
#include <string>
#include "config_reader.h"
#include "boost/date_time/posix_time/posix_time.hpp" //include all types plus i/o

using namespace boost::algorithm;

int main(int argc, char* argv[])
{
	const std::string path_ = "/etc/backloger/config.conf";
	
	config_reader cfg(path_, argc, argv);

	auto backuped_paths_ = cfg.backuped_paths();
	for (auto path_ : backuped_paths_)
	{
		std::cout << path_.path << std::endl;
		boost::filesystem::path dir(path_.path);
		for (boost::filesystem::directory_iterator file(dir), end; file != end; ++file)
		{

			for (auto type_ : path_.types)
			{
				if (file->path().extension() == type_)
				{
					std::string file_name = file->path().string();
					file_name = file_name.substr(file_name.find_last_of("/")+1, file_name.length());
					std::string archive_name = boost::posix_time::to_simple_string(boost::posix_time::second_clock::local_time()) + "_" + file_name + ".gz";
					archive_name.replace(archive_name.find(" "), 1, "_");
					archive_name.replace(archive_name.find(":"), 1, "");
					std::string command = "tar -zcvf "+ archive_name + " " + file->path().string();
					std::cout << command << std::endl;
					//system(command.c_str());
				}
			}
		}
	}
	return 0;
}