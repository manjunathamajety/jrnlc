#include <iostream>
#include <journal.h>
#include <jrnlmanager.h>
#include <config.h>
#include <parse.h>
#include <help.h>
#define VERSION "0.2.1"
int main(int argc,char** argv){
    try{        
        if(argc < 2){global_help(); return 0;}
        std::string arg=argv[1];
         
        if(arg == "add") {add_handle(argc-2,argv+2);}
        else if(arg == "show") {display_handle(argc-2,argv+2);}
        else if(arg == "backup") {backup_handle(argc-2,argv+2);}
        else if(arg == "init") {init_handle(argc-2,argv+2);}
        else if(arg == "--version"){std::cout<<"jrnlc - "<<VERSION<<std::endl;}
        else {global_help();}
        return 0;
    }
    catch(const std::runtime_error& e){
        std::cerr<<"jrnl: "<<e.what()<<"\n";
        return 1;
    }
}
