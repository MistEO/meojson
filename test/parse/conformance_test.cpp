#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "json.h"

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        std::cout << "param error, please input filename" << std::endl;
    }

    std::vector<std::string> success_filenames;
    std::vector<std::string> failed_filenames;

    for (int idx = 1; idx < argc; ++idx)
    {
        std::string filename = argv[idx];
        std::ifstream ifs(filename);
        std::stringstream ibuf;
        ibuf << ifs.rdbuf();
        std::string content(ibuf.str());
        //std::cout << content << std::endl;
        std::cout << "filename:\t" << filename << "\tsize:\t" << content.size();

        auto &&[ret, val] = json::parser::parse(content);

        if (ret)
        {
            std::cout << "\tsuccess😁" << std::endl;
            success_filenames.push_back(filename);
        }
        else
        {
            std::cout << "\t**failed***🥵" << std::endl;
            failed_filenames.push_back(filename);
        }
    }

    std::cout << "\nparse success file😁" << std::endl;
    for (auto &&str : success_filenames)
    {
        std::cout << str << " ";
    }
    std::cout << std::endl;

    std::cout << "\nparse failed file🥵" << std::endl;
    for (auto &&str : failed_filenames)
    {
        std::cout << str << " ";
    }
    std::cout << std::endl;

    return 0;
}
