#include <parse.h>
#include <time.h>
#include <jrnlmanager.h>
#include <config.h>

void init_handle(int argc, char** argv){
    if(argc == 0){
        throw std::runtime_error("init - invalid usage \n Usage jrnlc init [--global/--local]");
    }    
    else if(argc > 1){ 
        throw std::runtime_error("init - too many arguments \n Usage jrnlc init [--global/--local]");
    }
    else{
        std::string flag = argv[0];
        if(flag == "--global"){
            config c1;
            c1.global_init();
        }
        else if(flag == "--local"){
            config c1;
            c1.local_init();
        }
        else{
            throw std::runtime_error("jrnlc: init - invalid flag \n Usage jrnlc init [--global/--local]");
        }
    }


}

void add_handle(int argc, char** argv){
    //reading the config file for path
    bool is_local = true;
    config c1;
    c1.parseconfig();
    std::string entry;
    std::string tag_buf;
    std::optional<std::string> tag;
    bool got_entry = false;
    if (argc == 0){
        std::string entry;
        std::getline(std::cin, entry);
        c1.resolve_local_or_global(is_local);
        std::string PATH = c1.get_path();
        std::string BACKUP_PATH = c1.get_backup();
        Manager m1(PATH,BACKUP_PATH);
        m1.loadentry(PATH);
        m1.addentry(entry);
        m1.save(PATH);
    } 
    else {
        for(int i = 0; i < argc; i++){
            std::string arg = argv[i];
            //checking if any of the flags match with the argv vector
            if(arg == "--local"){
                is_local = true;
            }
            else if(arg == "--global"){
                is_local = false;
            }
            else if(arg == "--tag"){
                if(i+1 >= argc){
                    throw std::runtime_error("Unspecified tag: usage --tag [tag]");                }
                else{
                    tag_buf = argv[i+1];
                    i++;
                }
            }
            else {
                if(got_entry){
                    throw std::runtime_error("jrnlc: too many positional arguments for add \n Usage jrnlc add [entry] --tag [tag] [--local/--global]\n");
                }
                else{
                    entry = arg;
                    got_entry = true;
                }
            }
        }
    }

    if(!got_entry){
        if(!std::getline(std::cin,entry) || entry.empty()){
            throw std::runtime_error("jrnlc: no entry provided");
        }
    }
    c1.resolve_local_or_global(is_local);
    std::string PATH = c1.get_path();
    std::string BACKUP_PATH = c1.get_backup();
    Manager m1(PATH,BACKUP_PATH);
    m1.loadentry(PATH);
    if(tag_buf.empty()){
        m1.addentry(entry);
    }
    else{
        m1.addentry(entry, tag_buf);
    }
    m1.save(PATH);
}

void display_handle(int argc, char** argv){
    //reading the config file for path
    config c1;
    c1.parseconfig();
    std::string display_specifier;
    struct ShowFlag flags;
    bool is_local = true;
    //to find the machine state, tty or argv
    bool tty = isatty(STDOUT_FILENO);
    //buffer to find what the user preference is 
    std::optional<bool> color_buf;
    if (argc == 0){
        flags.range = "*";
    } 
    else {
        for(int i = 0; i < argc; i++){
            std::string arg = argv[i];
            //checking if any of the flags match with the argv vector
            //--before flag
            if(arg == "--before"){
                if(i+1 >= argc){
                    throw std::runtime_error("Unspecified time range; usage - jrnlc --before YYYY-MM-DD HH:MM");
                }
                else{
                    std::string time = argv[i+1];
                    flags.before = time_parse(time);
                    i++;
                }
            }
            //--after flag
            else if(arg == "--after"){
                if(i+1 >= argc){
                    throw std::runtime_error("Unspecified time range; usage - jrnlc --before YYYY-MM-DD HH:MM");
                }
                else{
                    std::string time = argv[i+1];
                    flags.after = time_parse(time);
                    i++;
                } 
            }
            //--color flag -> to force color in show fuction
            else if(arg == "--color"){
                color_buf = true;
            }
            //--no-color flag -> to force color-free output in show function
            else if(arg == "--no-color"){
                color_buf = false;
            }
            else if(arg == "--global"){
                is_local = false;
            }
            //check for range, which doesn't follow the flag format
            //so range SHOULD be the only term accepted which doesn't start with '--'
            else if(arg.size() < 2 || arg[0] != '-' || arg[1] != '-'){
                if(flags.range){
                    throw std::runtime_error("jrnlc: too many arguments for range \n");
                }
                else{
                    flags.range = argv[i];
                }
            }
        }
    }
    //by default the user's preference is given the highest priority
    if(color_buf.has_value()){
        flags.color = color_buf;
    }
    //else using the machine state;
    else{
        flags.color = tty;
    }
    const ColorTemplate& colors = c1.get_colors(); 
    c1.resolve_local_or_global(is_local);
    std::string PATH = c1.get_path();
    std::string BACKUP_PATH = c1.get_backup();
    //initializing the vector of journal entries 
    Manager m1(PATH, BACKUP_PATH);
    m1.loadentry(PATH); 
    m1.show(flags, colors);
}

void backup_handle(int argc, char** argv){
    std::string backup_name;
    //reading the config file for path
    config c1;
    c1.parseconfig();
    bool is_local = true;
    bool got_name = false;
    if(argc == 0){
        c1.resolve_local_or_global(is_local);
        std::string PATH = c1.get_path();
        std::string BACKUP_PATH = c1.get_backup();
        Manager m1(PATH,BACKUP_PATH);
        m1.loadentry(PATH);
        time_t time_now = timestamp();
        std::string time = timeconvert(time_now);
        BACKUP_PATH = BACKUP_PATH+"/backup"+time+".txt";
        m1.backup(BACKUP_PATH);
    }
    else{
        for(int i = 0; i < argc; i++){
            std::string arg = argv[i];
            if(arg == "--local"){
                is_local = true;
            }
            else if(arg == "--global"){
                is_local = false;
            }
            else{
                if(got_name == false){
                    backup_name = argv[i];
                    got_name = true;
                }
                else{
                    throw std::runtime_error("jrnlc: backup - too many positional arguments \n Usage jrnlc backup [name-optional] [--local/--global]\n");
                }
            }
        }    
    }
    if(backup_name.empty()){
        c1.resolve_local_or_global(is_local);
        std::string PATH = c1.get_path();
        std::string BACKUP_PATH = c1.get_backup();
        Manager m1(PATH,BACKUP_PATH);
        m1.loadentry(PATH);
        time_t time_now = timestamp();
        std::string time = timeconvert(time_now);
        BACKUP_PATH = BACKUP_PATH+"/backup"+time+".txt";
        m1.backup(BACKUP_PATH);
    }
    else{
        c1.resolve_local_or_global(is_local);
        std::string PATH = c1.get_path();
        std::string BACKUP_PATH = c1.get_backup();
        Manager m1(PATH,BACKUP_PATH);
        m1.loadentry(PATH);
        BACKUP_PATH = BACKUP_PATH+"/"+backup_name+".txt";
        m1.backup(BACKUP_PATH);
    }

}
