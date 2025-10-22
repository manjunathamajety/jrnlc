#include <iostream>
#include <ctime>
#include <string>
#include <fstream>
#include <memory>
#include <vector>
#include <journal.h>
#include <timestamp.h>
#ifndef JRNLMANAGER_H
#define JRNLMANAGER_H

#define BLACK   "\x1b[30m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define WHITE   "\x1b[37m"
#define RESET   "\x1b[0m"

class manager{

    private: 
    std::vector<jrnl> jrnl_manager;

    public:
    manager();
    void addentry(std::string txt);
    void save();
    void display();
};



#endif
