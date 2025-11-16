#include <iostream>
#include <ctime>
#include <string>
#include <fstream>
#include <memory>
#include <vector>
#include <journal.h>
#include <timestamp.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#include <filesystem>

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
    int id_count;

    public:
    manager(std::string PATH);
    void addentry(std::string txt,std::string tag="jrnl");
    void save(std::string PATH);
    void show(std::string range);
    int getid_count(){return id_count;}
};

#endif
