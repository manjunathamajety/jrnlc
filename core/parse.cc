#include <parse.h>
#include <time.h>
#include <jrnlmanager.h>
#include <config.h>

void init_handle(int argc, char** argv){
    if(argc == 0){
        throw std::runtime_error("jrnlc: init - invalid usage \n Usage jrnlc init [--global/--local]");
    }    
    else if(argc > 1){ 
        throw std::runtime_error("jrnlc: init - too many arguments \n Usage jrnlc init [--global/--local]");
    }
    else{
        std::string flag = argv[0];
        if(flag == "--global"){
            config c1;
            c1.global_init();
        }
        if(flag == "--local"){
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
    config c1;
    c1.parseconfig();
    std::string PATH = c1.get_path();
    std::string BACKUP_PATH = c1.get_backup();
    //initializing the vector for jrnl entries
    Manager m1(PATH,BACKUP_PATH);
    m1.loadentry(PATH);
    std::string entry;
    std::string tag;
    //condition when there's no input in argv
    if(argc == 0){
        //input from stream
        if(isatty(STDIN_FILENO)){
            std::getline(std::cin,entry);
            m1.addentry(entry);
            m1.save(PATH);
        }
        //input being piped
        else{
            std::getline(std::cin,entry);
            m1.addentry(entry);
            m1.save(PATH);
        }
    }
    //getting input from argv itself
    else{
        //tag is not specified
        if(argc==1){
            entry=argv[0];
            m1.addentry(entry);
            m1.save(PATH);
        }
        //tag is explicitly specified
        else if(argc==2){
            entry=argv[0];
            tag=argv[1];
            m1.addentry(entry,tag);
            m1.save(PATH);
        }
        else {
            throw std::runtime_error("jrnlc: too many arguments to 'add'\n Usage: jrnl add [entry] tag [tag-optional]\n");
        }
    }
    throw std::runtime_error("jrnlc: add - invalid use");
}

void display_handle(int argc, char** argv){
    //reading the config file for path
    config c1;
    c1.parseconfig();
    const ColorTemplate& colors = c1.get_colors(); 
    std::string PATH = c1.get_path();
    std::string BACKUP_PATH = c1.get_backup();
    //initializing the vector of journal entries 
    Manager m1(PATH, BACKUP_PATH);
    m1.loadentry(PATH);
    std::string display_specifier;
    struct ShowFlag flags;

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
                flags.color = true;
            }
            //--no-color flag -> to force color-free output in show function
            else if(arg == "--no-color"){
                flags.color = false;
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
    
    m1.show(flags, colors);
}

void backup_handle(int argc, char** argv){
    //reading the config file for path
    config c1;
    c1.parseconfig();
    std::string PATH = c1.get_path();
    std::string BACKUP_PATH = c1.get_backup();
    //initializing the vector for jrnl entries
    Manager m1(PATH,BACKUP_PATH);
    m1.loadentry(PATH);

    if(argc == 1){
        std::string name = argv[0]; 
        BACKUP_PATH = BACKUP_PATH+"/"+name+".txt";
        m1.backup(BACKUP_PATH);
    }
    else if(argc == 0){
        time_t time_now = timestamp();
        std::string time = timeconvert(time_now);
        BACKUP_PATH = BACKUP_PATH+"/backup"+time+".txt";
        m1.backup(BACKUP_PATH);
    }
    else{
        throw std::runtime_error("jrnlc: backup - too many arguments");
    }


}
