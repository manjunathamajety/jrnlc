#include <parse.h>
#include <time.h>
#include <jrnlmanager.h>
#include <config.h>

int add_handle(int argc, char** argv){
    //reading the config file for path
    config c1;
    c1.initialization();
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
            return 0;
        }
        //input being piped
        else{
            std::getline(std::cin,entry);
            m1.addentry(entry);
            m1.save(PATH);
            return 0;
        }
    }
    //getting input from argv itself
    else{
        //tag is not specified
        if(argc==1){
            entry=argv[0];
            m1.addentry(entry);
            m1.save(PATH);
            return 0;
        }
        //tag is explicitly specified
        else if(argc==2){
            entry=argv[0];
            tag=argv[1];
            m1.addentry(entry,tag);
            m1.save(PATH);
            return 0;
        }
        else {
            std::cerr<<"jrnl: too many arguments to 'add'\n"
                <<"Usage: jrnl add [entry] tag [tag-optional]\n";
            return 1;
        }
    }
    return 1;
}

int display_handle(int argc, char** argv){
    //reading the config file for path
    config c1;
    c1.initialization();
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
        //flags.mode = !isatty(STDIN_FILENO);
        if(isatty(STDIN_FILENO)){
            flags.mode = InputMode::std_in;
        }
    } 
    else {
        flags.mode = InputMode::argv;
        for(int i = 0; i < argc; i++){
            std::string arg = argv[i];
            //checking if any of the flags match with the argv vector
            //--before flag
            if(arg == "--before"){
                std::string time = argv[i+1];
                flags.before = time_parse(time); 
                i++;
            }
            //--after flag
            else if(arg == "--after"){
                std::string time = argv[i+1];
                flags.after = time_parse(time);
                i++;
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
                    std::cerr<<"jrnl: too many arguments for range \n";
                    return 1;
                }
                else{
                    flags.range = argv[i];
                }
            }
        }
    }
    if(!isatty(STDIN_FILENO)){
        flags.mode = InputMode::pipe;
    }
    else {
        flags.mode = InputMode::std_in;
    }
    
    m1.show(flags, colors);
    return 0;
}
