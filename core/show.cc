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

void Manager::show(const ShowFlag& flags){
    size_t start = 0, end = jrnl_manager.size();
    
    //filter for range flag
    if(flags.range.has_value()){
        const std::string st = flags.range.value();
        size_t filter_start = 0, filter_end = jrnl_manager.size(); 
        //manual parsing logic to set start and end based upon the request
        if(st.length()==1){
            //function to print the entire journal
            if(st == "*"){
                //do nothing :)
            }
            else{
                int num;
                num = std::stoi(st);
                if(num > jrnl_manager.size() || num <= 0){
                    throw std::runtime_error("You've got to ask me to show an entry IN the journal, not beyond it -_-");
                }
                else{
                    start = num-1;
                    end = start+1;
                }
            }
        }
        else if(st.length() >= 2){
            if(st[0] == '*'){
                int num = std::stoi(st.substr(1,st.size()));
                if(num > jrnl_manager.size() || num < 0){
                    throw std::runtime_error("Yeah mate, you can't expect me to show beyond what's present in your jrnl -_-, whatever... range-error");
                }
                else{
                    filter_end = num;
                }
            }
            else if(st[st.length()-1] == '*'){
                int num = std::stoi(st.substr(0,st.size()-1));
                if( num > jrnl_manager.size() || num < 0){
                    throw std::runtime_error("I can't show journal entires you haven't created yourself. whatever... range-error");
                }
                else{
                    filter_start = jrnl_manager.size() - num;
                }
            }
        }
        start = std::max(start, filter_start);
        end = std::min(end, filter_end);
    }

    //filter for after flag
    if(flags.after){
        //after just affect the start variable
        auto& vec = jrnl_manager;
        auto it = std::lower_bound(vec.begin(), vec.end(), flags.after.value(),[](jrnl& j, time_t aft){return j.getstamp() < aft;});
        size_t afterid = it - vec.begin();
            start = std::max(start, afterid);
    }

    //filter for before flag
    if(flags.before){
        //before just affects the end variable 
        auto& vec = jrnl_manager;
        auto it = std::upper_bound(vec.begin(), vec.end(), flags.before.value(), [](time_t bfr, jrnl& j){return j.getstamp() > bfr;});
        size_t beforeid = it - vec.begin();
            end = std::min(end, beforeid);
    }

    //jrnl_manager id width acquisition
    int width;
    if(jrnl_manager.empty()){
        width = 1;
    }
    else{
        width = lastid_digits(jrnl_manager.back().getid());
    }
        
    for(int i = start; i < end; i++){
            //each iteration of loop loads the corresponding jrnl_manager element into a temporary variable for display
                int id = jrnl_manager[i].getid();
                std::string tag = jrnl_manager[i].gettag();    
                time_t timestamp = jrnl_manager[i].getstamp();
                std::string txt = jrnl_manager[i].getentry();            
            //Printing each entry with formatting
                std::cout<<GREEN<<std::setw(width)<<std::setfill('0')<<id<<RESET<<" ";
                std::cout<<tag<<" "<<GREEN<<timeconvert(timestamp)<<RESET<<" "<<txt<<"\n";
    }
}
