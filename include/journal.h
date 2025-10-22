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
        
        int id;
        char tag;
        time_t timestamp;
        std::string text;

    public:
        
        //The constructor uses initializer list, to avoid overheads in string assignment. 
        //Move is used to avoid the overheads involved in copying, instead transfers the string.
        jrnl (int i, char t, time_t stamp,std::string txt) : id(i), tag(t), timestamp(stamp), text(std::move(txt)){};
        
        int getid(){return id;}
        char gettag(){return tag;}
        time_t getstamp(){return timestamp;}
        std::string getentry(){return text;}
        

};


#endif 
