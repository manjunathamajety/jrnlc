#include <timestamp.h>

std::time_t timestamp(){

    time_t now;
    std::time(&now);
    return now;

}

std::string timeconvert(time_t timestamp){

    std::tm* localtime=std::localtime(&timestamp);
    
    std::ostringstream oss;
    oss<<std::put_time(localtime, "%d-%m-%y %H:%M");
    return oss.str();

}
