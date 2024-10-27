#include <filesystem>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <regex>
#include <map>

namespace fs = std::filesystem;
std::map<fs::path, int> directorySum;
std::map<fs::path, std::string> directoryPeople;

void TraverseDirectory(const fs::path& directoryPath, const std::string& fileExtension,
    const std::string& resultFilePath)
{
    for (const auto& entry : fs::directory_iterator(directoryPath))
    {
        if (entry.is_directory())
        {
            TraverseDirectory(entry.path(), fileExtension, resultFilePath);
        }
        else if (entry.is_regular_file() && entry.path().extension() == fileExtension && \
            entry.path() != resultFilePath)
        {
            std::ifstream findFile;
            findFile.open(entry.path());

            if (findFile.is_open())
            {
                std::string line;
                std::getline(findFile, line);

                int price = std::stoi(line);
                if (entry.path().filename().string().find("_") != std::string::npos)
                {
                    directorySum[directoryPath] -= price;
                }
                else
                {
                    directorySum[directoryPath] += price;
                    directoryPeople[directoryPath] += ", " + entry.path().filename().string() + \
                        ": " + std::to_string(price);
                }
            }
            findFile.close();
        }
    }

    
}

int main()
{
    std::string currentPath = fs::current_path().string() + "\\dir";
    std::string fileExtension = ".txt";
    std::string resultFilePath = currentPath + "\\result.txt";

    TraverseDirectory(currentPath, fileExtension, resultFilePath);

    std::ofstream resultFile;
    resultFile.open(resultFilePath);
    for (const auto& pair : directorySum) {
        resultFile << "Path: " << pair.first << ", Total: " << pair.second;
        for (const auto& subPair : directoryPeople)
        {
            if (pair.first == subPair.first)
            {
                resultFile << subPair.second;
            }
        }
        resultFile << std::endl;
    }
    resultFile.close();

    return 0;
}
