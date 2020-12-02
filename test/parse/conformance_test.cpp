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
        ifs.close();
        std::string content(ibuf.str());
        //std::cout << content << std::endl;
        std::cout << "filename: " << filename << "\t size: " << content.size();

        auto value_opt = json::parser::parse(content);

        if (value_opt)
        {
            std::cout << "\tsuccess😁" << std::endl;
            success_filenames.push_back(filename);
        }
        else
        {
            std::cout << "\tfailed🥵" << std::endl;
            failed_filenames.push_back(filename);
        }
    }

    return 0;
}
