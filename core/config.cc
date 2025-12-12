#include <config.h>

static bool check_ansi(const std::string& ansi_code){
    int color = std::stoi(ansi_code);
        if(color > 255 || color < 0){
            return false;
        }
        else {
            return true;
        }
}

static bool check_create_parent_dir(std::string path){
    std::filesystem::path parent_dir = std::filesystem::path(path).parent_path();
    if(!std::filesystem::exists(parent_dir)){
        std::cout<<"Create directory at: "<<parent_dir<<std::endl;
        bool dir_create = std::filesystem::create_directories(parent_dir);
        if(dir_create == false){
            std::cerr<<"Couldn't create directory at: "<<parent_dir<<std::endl;
            return false;
        }
    }
    std::ofstream out(path, std::ios::app);
    if(!out){
        std::cout<<"Couldn't create file at path: "<<path<<std::endl;
        return false;
    }
    return true;

}

config::config(){
    //getting the default environments
    const char* home = std::getenv("HOME");
    const char* xdg_config = std::getenv("XDG_CONFIG_HOME");
    const char* xdg_data = std::getenv("XDG_DATA_HOME");

    if(xdg_config == nullptr && home == nullptr){
        //bailing out since there's no damned home
        throw std::runtime_error("Mate, your system is so weird, it's HOME-less");
    }
    else if(xdg_config != nullptr){
        //xdg_config is set, using it
        config_path = std::string(xdg_config)+"/jrnl/jrnl.txt";
    }
    else {
        //using home if xdg_config isn't set
        //and it actually has got home
        config_path = std::string(home)+"/.config/jrnl/jrnl.txt";
    }
    //chekcing for if the directories exist and creating the file if it doesn't exist
    bool file_check = check_create_parent_dir(config_path);
    if(file_check == false){
        throw std::runtime_error("Mate, coudln't even access your config directory, something is wrong with your filesystems bruh");

    }
    if(xdg_data == nullptr && home == nullptr){
        //bailing out since there's no damned xdg_data_home and home
        throw std::runtime_error("No way your system has no HOME AND XDG_DATA_HOME set man, I ain't god to work under such work conditions");
    }
    else if(xdg_data != nullptr){
        //using xdg_data for default path location for jrnl file
        PATH = std::string(xdg_data)+"/jrnl/journal.txt";
        BACKUP_PATH = std::string(xdg_data)+"/jrnl/backup";
    }
    else{
        //working with home
        PATH = std::string(home)+"/.local/share/jrnl/journal.txt";
        BACKUP_PATH = std::string(home)+"/.local/share/jrnl/backup";
    }
}

//method to check 
void config::initialization(){
    if (!std::filesystem::exists(config_path)){
        std::ofstream outfile(config_path);
        if(!outfile){
            throw std::runtime_error(std::string("Coudn't create file at location ") + config_path);
        }
        else{
            outfile<<"#Default location to store journal.txt"<<"\n";
            outfile<<"PATH$"<<PATH<<"\n";
            outfile<<"#Default location to store backups"<<"\n";
            outfile<<"BACKUP_PATH$"<<BACKUP_PATH<<"\n";
            outfile<<"#Color Configs[256bit ANSI Color Codes]"<<"\n";
            outfile<<"Id$"<<"32"<<"\n";
            outfile<<"Tag$"<<"34"<<"\n";
            outfile<<"Time$"<<"32"<<"\n";
            outfile<<"Text$"<<"37"<<"\n";
            outfile.close();
        }
    }
}


void config::parseconfig(){
    
    //reading the config file
    std::string line;
    std::ifstream file(config_path); 
    std::unordered_map<std::string, std::string> config_parse;
    while(std::getline(file,line)){
        if(line.empty() || line[0] == '#'){
            //do nothing
            continue;
        }
        
        auto eq = line.find('$');
        //when the search has failed
        if (eq == std::string::npos){
            //do nothing
            continue;
        }
        std::string key = line.substr(0,eq);
        std::string value = line.substr(eq+1);
        config_parse[key] = value;
    }
    
    //assinging the values in class, from the unordered map
    for(const auto& [key,value] : config_parse){
        if(key == "PATH"){
            PATH = value;
        }
        else if(key == "BACKUP_PATH"){
            BACKUP_PATH = value;
        }
        else if(key == "Id"){
            if(check_ansi(value)){
                colors.id_color = value;
            }
        }
        else if(key == "Tag"){
            if(check_ansi(value)){
                colors.tag_color = value;
            }
        }
        else if(key == "Time"){
            if(check_ansi(value)){    
                colors.timestamp_color = value;
            }
        }
        else if(key == "Text"){
            if(check_ansi(value)){
                colors.text_color = value;
            }
        }
    }
}


