#include <parse.h>

int add_handle(int argc, char** argv){
    //reading the config file for path
    config c1;
    std::string PATH=c1.getpath();
    //initializing the vector for jrnl entries
    manager m1(PATH);

    std::string entry;
    std::string tag;
    //input is given through argv
    if(argc>0){
        if(argc==1){
            entry=argv[0];
            m1.addentry(entry);
            m1.save(PATH);
            return 0;
        }
        else if(argc==2){
            entry=argv[0];
            tag=argv[1];
            m1.addentry(entry,tag);
            m1.save(PATH);
            return 0;
        }
    }
    //input is being piped to the program
    else if(isatty(STDIN_FILENO)){
        std::getline(std::cin,entry);
        m1.addentry(entry);
        m1.save(PATH);
        return 0;
    }
    
    else{
        std::getline(std::cin,entry);
        m1.addentry(entry);
        m1.save(PATH);
        return 0;
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
    // display_specifier is passed with the program call itself
    if(argc==1){
        display_specifier=argv[0];
        m1.show(display_specifier);
        m1.save(PATH);
        return 0;
    }
    //output is being piped
    else if(isatty(STDOUT_FILENO)){
        display_specifier="*";
        m1.show(display_specifier);
        m1.save(PATH);
        return 0;

    }
    else{
        std::getline(std::cin,display_specifier);
        m1.show(display_specifier);
        m1.save(PATH);
        return 0;
    }
     
return 1;

}



