#include <jrnlmanager.h>

int lastid_digits(int num){
    if (num == 0){return 1;}
    int count = 0;
    while(num){
        num /= 10;
        count++;
    }
    return count;
}


void manager::show(std::string range){

    int start, end;
    //manual parsing logic to set start and end based upon the request
    if(range.length()==1){
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
    else if(range.length() >= 2){
        if(range[0]=='*'){
            start=0;
            end=std::stoi(range.substr(1,range.size()));
        }
        else if(range[1]=='*'){
            start=jrnl_manager.size()-std::stoi(range.substr(0,range.size()-1));
            end=jrnl_manager.size();
        }
    }
    int width = lastid_digits(jrnl_manager.back().getid());
    for(int i=start;i<end;i++){
        //each iteration of loop loads the corresponding jrnl_manager element into a temporary variable for display
        int id=jrnl_manager[i].getid();
        std::string tag=jrnl_manager[i].gettag();
        time_t timestamp=jrnl_manager[i].getstamp();
        std::string txt=jrnl_manager[i].getentry();
        //Printing each entry with formatting
        std::cout<<GREEN<<std::setw(width)<<std::setfill('0')<<id<<RESET<<" ";
        std::cout<<tag<<" "<<GREEN<<timeconvert(timestamp)<<RESET<<" "<<txt<<"\n";
    }

}
