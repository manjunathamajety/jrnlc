#include <timestamp.h>

std::time_t timestamp(){
    time_t now;
    std::time(&now);
    return now;
}

std::time_t time_parse(std::string input_string){
    //struct of type tm
    std::tm tm{};
    //converting the input_string to a stream
    std::istringstream ss(input_string);
    //using get_time on the input_stream(string)
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
    //if get_time has failed
    if(ss.fail()){
        throw std::runtime_error("Invalid time format, Use: YYYY-MM-DD HH-MM");
    }
    //converting the struct tm to time_t for further o/p
    time_t t = std::mktime(&tm);
    //if converting it to epoch time has failed
    if(t == -1){
        throw std::runtime_error("Invalid or unrepresentable time value");
    }
    return t;
}

std::string timeconvert(time_t timestamp){
    std::tm* localtime=std::localtime(&timestamp);
    std::ostringstream oss;
    oss<<std::put_time(localtime, "%Y-%m-%d %H:%M");
    return oss.str();
}
