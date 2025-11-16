#include <jrnlmanager.h>


void manager::show(std::string range){

    int start, end;
    //manual parsing logic to set start and end based upon the request
    if(range.length()>2){

    }
    else if(range.length()==1){
            //function to print the entire journal
        if(range=="*"){
            start=0;
            end=jrnl_manager.size();
        }
        else{
            start=std::stoi(range)-1;
            end=start+1;
        }
    }
    else if(range.length()==2){
        if(range[0]=='*'){
            start=0;
            end=std::stoi(range.substr(1,range.size()));
        }
        else if(range[1]=='*'){
            start=jrnl_manager.size()-std::stoi(range.substr(0,range.size()-1));
            end=jrnl_manager.size();
        }
    }

    for(int i=start;i<end;i++){
        //each iteration of loop loads the corresponding jrnl_manager element into a temporary variable for display
        int id=jrnl_manager[i].getid();
        std::string tag=jrnl_manager[i].gettag();
        time_t timestamp=jrnl_manager[i].getstamp();
        std::string txt=jrnl_manager[i].getentry();
        //Printing each entry with formatting
        std::cout<<GREEN<<id<<RESET<<" "<<tag<<" "<<GREEN<<timeconvert(timestamp)<<RESET<<" "<<txt<<"\n";
    }

}
