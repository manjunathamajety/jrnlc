#include <iostream>
#include <journal.h>
#include <jrnlmanager.h>
#include <config.h>
#include <parse.h>
#include <help.h>

int main(int argc,char** argv){
        
        if(argc < 2){global_help(); return 0;}
        std::string arg=argv[1];
         
        if(arg=="add") {add_handle(argc-2,argv+2);}
        else if(arg=="show") {display_handle(argc-2,argv+2);}
        else {global_help();}
        return 0;
}
