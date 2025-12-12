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
        ColorTemplate colors;

    public:
        config();
        std::string get_path(){return PATH;}
        std::string get_backup(){return BACKUP_PATH;} 
        void initialization();
        void parseconfig();
        const ColorTemplate& get_colors(){return colors;}
};

#endif 
