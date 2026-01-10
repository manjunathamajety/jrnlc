#include <jrnlmanager.h>
#include <config.h>

size_t lastid_digits(size_t num){
    if (num == 0){return 1;}
    size_t count = 0;
    while(num){
        num /= 10;
        count++;
    }
    return count;
}

void Manager::show(const ShowFlag& flags, const ColorTemplate& colors){
    size_t start = 0, end = jrnl_manager.size();
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
        auto it = std::upper_bound(vec.begin(), vec.end(), flags.before.value(), [](time_t bfr, jrnl& j){return bfr < j.getstamp();});
        size_t beforeid = it - vec.begin();
            end = std::min(end, beforeid);
    }
     
    //filter for range flag
    if(flags.range.has_value()){
        const std::string st = flags.range.value();
        //manual parsing logic to set start and end based upon the request
            //function to print the entire journal
        if(st[0] == ':' && st.length() == 1){
            //do nothing as this condition means, to print the entire journal
        }
        else if(st[0] != ':' && st[st.length()-1] != ':'){
            int num;
            num = std::stoi(st);
            if(num > jrnl_manager.size() || num <= 0){
                throw std::runtime_error("You've got to ask me to show an entry IN the journal, not beyond it -_-");
            }
            else{
                start = num-1;
                end = num;
            }
        }
        else if(st.length() >= 2){
            if(st[0] == ':'){
                int num = std::stoi(st.substr(1,st.size()-1));
                if(num > jrnl_manager.size() || num <= 0){
                    throw std::runtime_error("Yeah mate, you can't expect me to show beyond what's present in your jrnl -_-, whatever... range-error");
                }
                else{
                    size_t fnum = num;
                    end = start + num;
                }
            }
            else if(st[st.length()-1] == ':'){
                int num = std::stoi(st.substr(0,st.size()-1));
                if( num > jrnl_manager.size() || num <= 0){
                    throw std::runtime_error("I can't show journal entires you haven't created yourself. whatever... range-error");
                }
                else{
                    size_t fnum = num;
                    start = end - num;
                }
            }
        }
        else{
            throw std::runtime_error("show - Invalid range specified \n Usage jrnlc show :n or jrnlc show n: ");
        }
    }
    std::string id_color,tag_color,time_color,text_color;
    std::string reset = "\x1b[0m";
    if(flags.color == true){
    
        id_color   = std::string("\x1b[") + colors.id_color + "m";
        tag_color  = std::string("\x1b[") + colors.tag_color + "m";
        time_color = std::string("\x1b[") + colors.timestamp_color + "m";
        text_color = std::string("\x1b[") + colors.text_color + "m";
    }
    else{
        id_color = "";
        tag_color = "";
        time_color = "";
        text_color = "";
    }

    //jrnl_manager id width acquisition
    size_t width;
    if(jrnl_manager.empty()){
        width = 1;
    }
    else{
        width = lastid_digits(jrnl_manager.back().getid());
    }
    for(size_t i = start; i < end; i++){
            //each iteration of loop loads the corresponding jrnl_manager element into a temporary variable for display
                size_t id = jrnl_manager[i].getid();
                std::string tag = jrnl_manager[i].gettag();    
                time_t timestamp = jrnl_manager[i].getstamp();
                std::string txt = jrnl_manager[i].getentry();            
            //Printing each entry with formatting
                std::cout<<id_color<<std::right<<std::setw(width)<<std::setfill('0')<<id<<reset<<" ";
                std::cout<<tag_color<<std::left<<std::setw(tag_size)<<std::setfill(' ')<<tag<<reset<<" ";
                std::cout<<time_color<<timeconvert(timestamp)<<reset<<" "<<text_color<<txt<<reset<<"\n";
    }
}
