//This is the implementation file for adding an entry into the journal.
#include <entry.h>

inline std::int64_t timestamp(){

    time_t now;
    std::time(&now);
    return now;   

}

void entry(std:: string txt){

    std::ofstream file("jrnl.txt",std::ios::app);
    file<<"\n"<<timestamp()<<"::"<<txt;
    file.close();
   


}
