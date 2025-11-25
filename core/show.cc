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
            start = 0;
            end = jrnl_manager.size();
        }
        else{
            int num;
            num = std::stoi(range);
            if(num > jrnl_manager.size() || num <= 0){
                throw std::runtime_error("You have got to ask me to show for an entry IN the journal, not beyond it -_-");
            }
            else{
                start = num-1;
                end = start+1;
            }
        }
    }
    else if(range.length() >= 2){
        if(range[0] == '*'){
            int num = std::stoi(range.substr(1,range.size()));
            if(num > jrnl_manager.size() || num < 0){
                throw std::runtime_error("jrnl: Yeah mate, you can't expect me to show beyond what's present in your jrnl -_-, whatever... range-error");
            }
            else{
                start = 0;
                end = num;
            }
        }
        else if(range[range.length()-1] =='*'){
            int num = std::stoi(range.substr(0,range.size()-1));
            if( num > jrnl_manager.size() || num < 0){
                throw std::runtime_error("jrnl: I can't show journal entires you haven't created yourself. whatever... range-error");
            }
            else{
                start=jrnl_manager.size()-num;
                end=jrnl_manager.size();
            }
        }
    }
    int width;
    if(jrnl_manager.empty()){
        width = 1;
    }else{
        width = lastid_digits(jrnl_manager.back().getid());
    }

    for(int i=start;i<end;i++){
        //each iteration of loop loads the corresponding jrnl_manager element into a temporary variable for display
        int id=jrnl_manager[i].getid();
        std::string tag=jrnl_manager[i].gettag();
        time_t timestamp=jrnl_manager[i].getstamp();
        std::string txt=jrnl_manager[i].getentry();
        //Printing each entry with formatting
        std::cout<<CYAN<<std::setw(width)<<std::setfill('0')<<id<<RESET<<" ";
        std::cout<<tag<<" "<<RED<<timeconvert(timestamp)<<RESET<<" "<<txt<<"\n";
    }

}
