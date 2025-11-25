#include <config.h>

config::config(){

    std::string config_path;
    //looking for the enviroment paths
    const char* home=std::getenv("HOME");
    const char* xdg_config=std::getenv("XDG_CONFIG_HOME");
    const char* xdg_data=std::getenv("XDG_DATA_HOME");

    if(xdg_config == nullptr && home == nullptr){
        //bailing out if there's no home
        throw std::runtime_error("jrnl: Mate, your system is so weird, it's HOME-less");
    }
    else if( xdg_config != nullptr){
        //if xdg_config is set, using it
        config_path=std::string(xdg_config)+"/jrnl/jrnl.txt";
    }
    else {    
        //using home if xdg_config isn't set up
        config_path = std::string(home)+"/.config/jrnl/jrnl.txt";
    }

    //checking if the config directory exists
    std::filesystem::path config_dir = std::filesystem::path(config_path).parent_path();
    if(!std::filesystem::exists(config_dir)){
        std::cout<<"jrnl: To tell your journal where you would wanna store your secrets...." <<config_dir<<std::endl;
        std::filesystem::create_directories(config_dir);
    }

    //creating the file with default values, if it doesn't exist
    if(!std::filesystem::exists(config_path)){
        std::cout<<"jrnl: Your journal couldn't find where to store your secrets. Due to the lack of choice, storing it in a default location -_-"
            <<std::endl;
        std::ofstream outfile(config_path);
        
        std::string data_path;
        if(!outfile.is_open()){
            throw std::runtime_error("jrnl: And.... yeah, your journals config file coudn't be opened at "+config_path);
        }
        if(xdg_data == NULL){
            data_path=std::string(home)+"/.local/share/jrnl/journal.txt";
        }
        else{
            data_path=std::string(xdg_data)+"/jrnl/journal.txt";
        }
        outfile<<"PATH$"+data_path;
    }

    //reading the config file from /.config/
    std::string identifier;
    std::ifstream file(config_path);
    std::getline(file,identifier,'$');
    std::getline(file,PATH);
    //PATH=std::string(home)+PATH;
    
    //checking if the journal target directory exists, creating it not
    std::filesystem::path jrnl_dir = std::filesystem::path(PATH).parent_path();
    if(!std::filesystem::exists(jrnl_dir)){
        std::cout<<"Storing your secrets at "<<jrnl_dir<<std::endl;
        std::filesystem::create_directories(jrnl_dir);
    }

    //creating journal file if it doesn't exist
    if(!std::filesystem::exists(PATH)){
        std::ofstream jrnlfile(PATH);
        if(!jrnlfile.is_open()){
            throw std::runtime_error("The vault for your secrets couldn't be created. Maybe today, the Universe doesn't want you journalling"+PATH);
        }
    }
}
