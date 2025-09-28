#include <iostream>
#include <entry.h>
#include <display.h>

int main(int argc,char** argv){
  
    
    if (argc>3){
        std::cout<<"Error! Too many arguments!";
    }
    
        
    if (argc<3){
        std::cout<<"Error! Too few arguments!";
    }
    
    if (argc==3){
        
        std::string arg=argv[1];
    
        if (arg=="-a"){
            std::string str=argv[2];
            entry(str);
        }

        if (arg=="-l"){
            std::string str=argv[2];
            display(str);

        }

    }
    
}
