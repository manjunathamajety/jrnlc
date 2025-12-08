#include <config.h>

static void check_create_parent_dir(const std::string path){
    std::filesystem::path parent_dir = std::filesystem::path(path).parent_path();
    if(!std::filesystem::exists(parent_dir)){
        std::cout<<"Creating parent directory at: "<<parent_dir<<std::endl;
        std::filesystem::create_directories(parent_dir);
    }
}

static void check_create_file(const std::string path){
    std::ofstream out(path);
    if(!out){
        throw std::runtime_error("Couldn't create file at" + " "+ path);
    }
}

void config::read_config_file(std::string config_path){
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
        if (eq == std::string::npos){
            //do nothing
            continue;
        }
        std::string key = line.substr(0,eq);
        std::string value = line.substr(eq+1);
        config_parse[key] = value;
    }
    
    //assinging the values in class, from the unordered map
    struct ColorConfig colors; 
    for(const auto& [key,value] : config_parse){
        if(key == "PATH"){
            this->PATH = value;
        }
        else if(key == "BACKUP_PATH"){
            this->BACKUP_PATH = value;
        }
        else if(key == "id_color"){
            colors.id_color = value;
        }
        else if(key == "tag_color"){
            colors.tag_color = value;
        }
        else if(key == "timestamp_color"){
            colors.timestamp_color = value;
        }
        else if(key == "Text_color"){
            colors.text_color = value;
        }
    }
}


void create_default_config(std::string config_path, const char* xdg_data){
    //creating the file with default values, since it doesn't exist
        std::cout<<"Your journal couldn't find where to store your secrets. Due to the lack of choice, storing it in a default location -_-"
            <<std::endl;
        std::ofstream outfile(config_path);
        
        std::string data_path;
        if(!outfile.is_open()){
            throw std::runtime_error("And.... yeah, your journals config file coudn't be opened at "+std::string(config_path));
        }
        if(xdg_data == NULL){
            data_path=std::string(gethome())+"/.local/share/jrnl/journal.txt";
        }
        else{
            data_path=std::string(xdg_data)+"/jrnl/journal.txt";
        }
        outfile<<"#Default path for storing journal.txt file"<<std::endl;
        outfile<<"PATH$"+data_path<<std::endl;

        std::string backup_path;
        if(xdg_data == NULL){
            backup_path = std::string(gethome())+"/.local/share/jrnl/backup";
        }
        else{
            backup_path = std::string(xdg_data)+"/jrnl/backup";
        }
        outfile<<"#Default directory path for storing backups"<<std::endl;
        outfile<<"BACKUP_PATH$" + backup_path<<std::endl;
}
//constructor initalizing the variables with default paths
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
    check_create_parent_dir(config_path);
    check_create_file(config_path);

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
    check_create_parent_dir(PATH);
    check_create_parent_dir(BACKUP_PATH);
    check_create_file(PATH);
}

//method to check 

