#include <iostream>
#include <journal.h>
#include <jrnlmanager.h>
#include <config.h>
#include <parse.h>
#include <help.h>

int main(int argc,char** argv){
        std::string arg=argv[1];
    
        if(arg=="add") {add_handle(argc-2,argv+2);}
        if(arg=="show") {display_handle(argc-2,argv+2);}
        
    return 0;
}
