#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <stdexcept>
#include <unordered_map>

#ifndef CONFIG_H
#define CONFIG_H

class config{
    
    private:
        std::string PATH;

    public:
        config();
        std::string getpath(){return PATH;}
};

#endif 
