#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <stdexcept>
#include <unordered_map>
#include <optional>

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
        std::optional<std::string> local_path;
        std::optional<std::string> local_backup;
        ColorTemplate colors;

    public:
        config();
        std::string get_path(){ return local_path.value_or(PATH);}
        std::string get_backup(){ return local_backup.value_or(BACKUP_PATH);} 

        void global_init();
        void parseconfig();
        void local_init();
        const ColorTemplate& get_colors(){return colors;}
        void resolve_local_or_global(bool is_local);        
};

#endif 
