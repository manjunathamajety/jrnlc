#include <iostream>
//#include <entry.h>

int main(int argc,char** argv){
   
    std::string arg=argv[1];
        

    if (argc>3){
        std::cout<<"Error! Too many arguments!";
    }
    
        
    if (argc<3){
        std::cout<<"Error! Too few arguments!";
    }
    
    
    if (argc==3){

        if (arg=="-a"){
            std::string str=argv[2];
            std::cout<<"Journal Entry for the string:"<<str;

        }

        if (arg=="-l"){
            std::string ptr=argv[2];
            std::cout<<"\n"<<ptr;


        }

    }
    
}
