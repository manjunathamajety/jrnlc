#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <stdexcept>
#include <unordered_map>

#ifndef CONFIG_H
#define CONFIG_H

struct ColorTemplate {
    std::string id_color;
    std::string tag_color;
    std::string timestamp_color;
    std::string text_color;
};

class config{
    
    private:
        std::string config_path;
        std::string PATH;
        std::string BACKUP_PATH;
        struct ColorTemplate colors;

    public:
        config();
        std::string getconfigpath(return config_path;)
        std::string getpath(){return PATH;}
        std::string getbackup(){return BACKUP_PATH;} 
        void initialization();
        const ColorTemplate& getcolors();
};

#endif 
