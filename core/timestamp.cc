#include <timestamp.h>

std::int64_t timestamp(){

    time_t now;
    std::time(&now);
    return now;

}

std::string timeconvert(std::string timestamp){

    time_t time=std::stoll(timestamp);
    std::tm* localtime=std::localtime(&time);
    
    std::ostringstream oss;
    oss<<std::put_time(localtime, "%y-%m-%d %H:%M");
    return oss.str();

}
