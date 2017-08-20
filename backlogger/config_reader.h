#pragma once
#include <boost/filesystem.hpp>
#include <vector>
#include <fstream>



class config_reader
{
private:
	std::ofstream* logger;

	/**
	* \Структура для хранения путей
	*/
	struct patch_info
	{
		/**
		* \Типы файлов
		*/
		std::vector<std::string> types;
		/**
		* \Путь до папки
		*/
		std::string path;
	};

	/**
	* \Путь до папки с бэкапами
	*/
	std::string patch_for_save_;

	/**
	* \Переменная с путем до конфига.
	*/
	std::string config_path_;

	/**
	* \Переменная с путями до папок которые пакуем и типов файлов
	*/
	std::vector<patch_info> backuped_paths_;

	bool config_creator() const;

	/**
	* \Функция проверяет наличие конфига
	*/
	void check_config() const;	
	/**
	 * \Функция проверяет наличие конфига, для флага -noanswer
	 */
	void check_config_na() const;

	/**
	* \Функция фатальных ошибок, выдает ошибку и закрывает программу
	* \input: Error text in string
	*/
	static void fatal_error(std::string);

	/**
	* \Функция читает данные из конфига
	*/
	void parce_config();

	void parce_config_na();

	/**
	* \Функция удаляет все пробелы из строки
	* \param str
	* \return string //without spaces
	*/
	static std::string trimmer(std::string);

	/**
	* \Функция проверяет наличие "/" в конце пути к папкам и убирает его
	* \param str
	* \return string*
	*/
	static std::string* check_last_slash(std::string*);

	bool ignore_;

	bool noanswer_;

	bool newcfg_;

	void KeyChecker(int argc, char* argv[]);
public:
	std::ofstream* getlogger() const
	{
		return logger;
	}

	bool noanswer() const
	{
		return noanswer_;
	}
	/**
	 * \Конфструктор для чтения конфига. Принимает путь до файла с конфигом
	 * \param config_path 
	 */
	config_reader(const std::string&, int argc, char* argv[]);
//	config_reader(const std::string&);

	/**
	 * \Getter to backuped_paths
	 * \return backuped_paths
	 */
	std::vector<patch_info> backuped_paths() const;
};
