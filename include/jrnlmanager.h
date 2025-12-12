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
#include <iomanip>
#include <optional>
#include <algorithm>
#include <config.h>

#ifndef JRNLMANAGER_H
#define JRNLMANAGER_H

enum class InputMode{
    argv,
    pipe,
    std_in
};


//struct to store the flags specified for show method
struct ShowFlag{
    std::optional<std::string> range;
    std::optional<bool> color;
    std::optional<time_t> before;
    std::optional<time_t> after;

    InputMode mode;
};


class Manager{

    private: 
    std::vector<jrnl> jrnl_manager;
    size_t id_count;
    size_t tag_size;

    public:
    Manager(std::string PATH, std::string BACKUP_PATH);
    void loadentry(std::string PATH);
    void addentry(std::string txt,std::string tag="jrnl");
    void save(std::string PATH);
    void show(const ShowFlag& flags,const ColorTemplate& colors);
    void backup(std::string PATH);
    int getid_count(){return id_count;}

};

#endif
