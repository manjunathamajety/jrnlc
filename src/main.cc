#include <iostream>
#include <journal.h>
#include <jrnlmanager.h>
#include <config.h>
#include <parse.h>
#include <help.h>
#define VERSION "0.3.0"
int main(int argc,char** argv){
    try{        
        if(argc < 2){global_help(); return 0;}
        std::string arg=argv[1];
         
        if(arg == "add") {add_handle(argc-2,argv+2);}
        else if(arg == "show") {show_handle(argc-2,argv+2);}
        else if(arg == "backup") {backup_handle(argc-2,argv+2);}
        else if(arg == "init") {init_handle(argc-2,argv+2);}
        else if(arg == "--version"){std::cout<<"jrnlc - "<<VERSION<<std::endl;}
        else if(arg == "--help"){global_help();}
        else{throw std::runtime_error("invalid usage \n Use jrnlc --help for info \n");}
        return 0;
    }
    catch(const std::runtime_error& e){
        std::cerr<<"jrnlc: "<<e.what()<<"\n";
        return 1;
    }
    catch(const std::invalid_argument& e){
        std::cerr<<"jrnlc: "<<e.what()<<" Invalid argument passed \n For help, use jrnlc [function] --help"<<"\n";
    }
    catch(const std::out_of_range& e){
        std::cerr<<"jrnlc: "<<e.what()<<" Invalid argument passed \n For help, use jrnlc [function] --help"<<"\n";
    }
}
