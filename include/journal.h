#include <iostream>
#include <ctime>
#include <string>
#include <fstream>
#include <memory>
#include <vector>

#ifndef JOURNAL_H
#define JOURNAL_H

//class representing a journal entry
class jrnl{
    
    private:
        
        size_t id;
        std::string tag;
        time_t timestamp;
        std::string text;

    public:
        
        //The constructor uses initializer list, to avoid overheads in string assignment. 
        //Move is used to avoid the overheads involved in copying, instead transfers the string.
        jrnl (size_t i, std::string t, time_t stamp,std::string txt) : id(i), tag(std::move(t)), timestamp(stamp), text(std::move(txt)){};
        
        size_t getid()const{return id;}
        std::string gettag(){return tag;}
        time_t getstamp()const{return timestamp;}
        std::string getentry(){return text;}
        

};


#endif 
