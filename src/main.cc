#include <iostream>
#include <entry.h>
#include <display.h>
#include <journal.h>
#include <jrnlmanager.h>

int main(int argc,char** argv){
      
    if (argc>3){
        std::cout<<"Error! Too many arguments!";
    }
    
        
    if (argc<3){
        std::cout<<"Error! Too few arguments!";
    }
    
    if (argc==3){
        
        manager m1;

        std::string arg=argv[1];
    
        if (arg=="-a"){
            std::string str=argv[2];
            //entry(str);
            m1.addentry(str);
            m1.save();
        }

        if (arg=="-l"){
            std::string str=argv[2];
            m1.display();
            //display(str);
            m1.save();
        }

    }
    
}
