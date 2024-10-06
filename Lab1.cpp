#include <filesystem>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <regex>
#include <map>

namespace fs = std::filesystem;

typedef struct limits
{
	int low;
	int high;
};

std::map<std::string, int> acceptedDB; // Принятые товары

void ReadRequirements(const std::string& input, std::map<std::string, limits>& limitations)
{
	// Регулярное выражение для сопоставления строк и чисел
	std::regex pattern("([a-zA-Z]+):([0-9]*):([0-9]*)");

	std::smatch matches; // Для хранения результатов сопоставления

	// Поиск всех соответствий в строке
	auto words_begin = std::sregex_iterator(input.begin(), input.end(), pattern);
	auto words_end = std::sregex_iterator();

	for (std::sregex_iterator i = words_begin; i != words_end; ++i) 
	{
		std::smatch match = *i;
		std::string word = match[1]; // Получаем строку
		int num1 = match[2].str().empty() ? 0 : std::stoi(match[2].str()); // Получаем первое число или 0
		int num2 = match[3].str().empty() ? INT_MAX : std::stoi(match[3].str()); // Получаем второе число или INT_MAX

		limits limits = { num1, num2 };
		limitations[word] = limits;
	}
}

void Accept(const std::string& input, const std::map<std::string, limits>& limitations)
{
	// Регулярное выражение для сопоставления строк и чисел
	std::regex pattern("([a-zA-Z]+):([0-9]*)");

	std::smatch matches; // Для хранения результатов сопоставления

	// Поиск всех соответствий в строке
	auto words_begin = std::sregex_iterator(input.begin(), input.end(), pattern);
	auto words_end = std::sregex_iterator();

	for (std::sregex_iterator i = words_begin; i != words_end; ++i)
	{
		std::smatch match = *i;
		std::string word = match[1]; // Получаем строку
		int num = std::stoi(match[2].str()); // Получаем число

		auto itL = limitations.find(word);
		if (itL != limitations.end())
		{
			if (num < itL->second.low)
			{
				num = 0; // Если меньше требования - не берем
			}
			else if (num >= itL->second.high)
			{
				num = itL->second.high; // Больше или равно - берем разрешенный максимум
			}
			// Если ограничений нет - ничего не меняем
		}

		auto itA = acceptedDB.find(word);
		if (itA != acceptedDB.end()) 
		{
			// Элемент найден
			acceptedDB[word] += num;
		}
		else 
		{
			// Элемент не найден
			acceptedDB[word] = num;
		}
	}
}

void TraverseDirectory(const fs::path& directoryPath, const std::string& fileExtension,
	const std::string& resultFilePath)
{
	for (const auto& entry : fs::directory_iterator(directoryPath))
	{
		if (entry.is_directory())
		{
			TraverseDirectory(entry.path(), fileExtension, resultFilePath); // Рекурсивный вызов для обхода поддиректорий
		}
		else if (entry.is_regular_file() && entry.path().extension() == fileExtension && \
			entry.path() != resultFilePath)
		{
			std::map<std::string, limits> limitationsDB;

			std::ifstream findfile;
			findfile.open(entry.path());

			if (findfile.is_open())
			{
				std::string line;
				std::getline(findfile, line);

				ReadRequirements(line, limitationsDB); // Сохраняем ограничения

				while (std::getline(findfile, line))
				{
					Accept(line, limitationsDB); // Принимаем товары
				}

				std::cout << "File " << entry.path() << "is readed successfully" << std::endl;
			}
			else
			{
				std::cout << "Error openning files" << std::endl;
			}
		}
	}
}

int main()
{
	fs::path currentPath = fs::current_path(); // Текущая директория
	std::string fileExtension = ".txt"; // Интересующий формат файла
	std::string resultFilePath = currentPath.string() + "\\result.txt"; // Путь результирующего файла

	TraverseDirectory(currentPath, fileExtension, resultFilePath);

	std::ofstream outfile;
	outfile.open(resultFilePath, std::ios::app);

	// Запись результатов
	for (const auto& item : acceptedDB)
	{
		outfile << item.first << "=" << item.second << std::endl;
	}

	outfile.close();

	return 0;
}
