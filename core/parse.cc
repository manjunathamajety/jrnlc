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
    else if(isatty(STDIN_FILENO)){
        std::getline(std::cin,entry);
        m1.addentry(entry);
        m1.save(PATH);
        return 0;
    }
    
    //input is given via pipe
    else{
        std::getline(std::cin,entry);
        m1.addentry(entry);
        m1.save(PATH);
        return 0;
    }
    return 1;
}

int display_handle(int argc, char** argv){
    config c1;
    std::string PATH=c1.getpath();
    manager m1(PATH);
    std::string display_specifier;
    if(argc==1){
        display_specifier=argv[0];
        m1.display(display_specifier);
        m1.save(PATH);
        return 0;
    }
    else if(isatty(STDOUT_FILENO)){
        display_specifier="*";
        m1.display(display_specifier);
        m1.save(PATH);
        return 0;

    }
    else{
        std::getline(std::cin,display_specifier);
        m1.display(display_specifier);
        m1.save(PATH);
        return 0;
    }
     

return 1;

}



