#include <parse.h>

int add_handle(int argc, char** argv){
    //reading the config file for path
    config c1;
    std::string PATH=c1.getpath();
    //initializing the vector for jrnl entries
    manager m1(PATH);

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
    std::string PATH=c1.getpath();
    //initializing the vector of journal entries 
    manager m1(PATH);
    std::string display_specifier;
    // when no argument is passed 
    if (argc == 0){
        //stream or pipe, we just print everything :)
        if(isatty(STDOUT_FILENO)){
            display_specifier="*";
            m1.show(display_specifier);
            m1.save(PATH);
            return 0;

        }
        //piped input 
        else{
            display_specifier="*";
            m1.show(display_specifier);
            m1.save(PATH);
            return 0;
        }
    }
    //passing argv
    else if(argc==1){
        display_specifier=argv[0];
        m1.show(display_specifier);
        m1.save(PATH);
        return 0;
    }
    else{
        std::cerr<<"jrnl: too many arugments to 'show'\n"
            <<"Usage: jrnl show [range]\n";
        return 1;
    } 
     
return 1;

}



