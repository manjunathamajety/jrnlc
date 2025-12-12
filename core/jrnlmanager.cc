#include <jrnlmanager.h>

static bool check_dir(const std::string& path){
    std::filesystem::path parent_dir = std::filesystem::path(path).parent_path();
    if(!std::filesystem::exists(parent_dir)){
        std::cout<<"Creating parent directory at: "<<parent_dir<<std::endl;
        bool created = std::filesystem::create_directories(parent_dir);
        if(created == false){
            std::cerr<<"Couldn't create parent directories at: "<<parent_dir<<std::endl;
            return false;
        }
        return true;
    }
    return true;
}

static bool check_file(std::string& path){
    std::ofstream out(path, std::ios::app);
    if(!out){
        std::cerr<<"Couldn't create file at path: "<<path<<std::endl;
        return false;
    }
    return true;
}

Manager::Manager(std::string PATH, std::string BACKUP_PATH){
    bool path_dir_check = check_dir(PATH);
    if(path_dir_check == false){
        throw std::runtime_error("Aborted");
    }
    bool path_file = check_file(PATH);
    if(path_file == false){
        throw std::runtime_error("Aborted");
    }
    
    bool backup_check = check_dir(BACKUP_PATH);
}

void Manager::loadentry(std::string PATH){    
    std::ifstream file(PATH);
    //checking if file is open
    if(!file.is_open()){
        std::cerr<<"Couldn't open the jrnl file: "<<PATH<<" for reading, dude lets start empty";
        id_count=0;
        return;
    }
    //Initializes jrnl-objects in the jrnl_manager vector.
    int line_no=0;
    size_t tagsize=0;
    std::string id,tag,stamp,txt;
    //while loops checks for if the entry can be compleltely scanned
    while (
        std::getline(file,id,';')&&
        std::getline(file,tag,';')&&
        std::getline(file,stamp,';')&&
        std::getline(file,txt)){
        ++line_no;
        if(tag.length() > tagsize){
            tagsize = tag.length();
        }
        if(id.empty() || tag.empty() || stamp.empty() || txt.empty()){
            std::cerr<<"Corrupted entry: "<<PATH<<":"<<line_no<<": skipping this entry \n";
            continue;
        }
        
        try{
            size_t entry_id=std::stoi(id);
            long long times=std::stoll(stamp);

            if(entry_id <= 0 || times <= 0){
                std::cerr<<"Jrnl: "<<PATH<<":"<<line_no<<"Invalid ID/Timestamp,skipping \n"<<
                    "Don't tell me you edited the jrnl yourself -_-";
                continue;
            }

            //pushes to the vector
            time_t t = static_cast<time_t>(times);
            jrnl_manager.emplace_back(entry_id,tag,t,txt);

        }   
        catch(const std::exception& e){
            std::cerr<<"Jrnl: "<<PATH<<":"<<line_no<<"corrupted entry ("<<e.what()<<"), skipping\n";
            continue;

        }
        
    }
    tag_size = tagsize;
    file.close();
    
    if(jrnl_manager.empty()){
        id_count=0;
    }
    else{
        id_count=jrnl_manager.back().getid();
    }
}


void Manager::addentry(std::string txt,std::string tag){
    jrnl_manager.emplace_back(id_count+1,tag,timestamp(),txt);
    id_count++;
}

//passing jrnl_manager by reference 'const &'
bool write_all(int fd, std::vector<jrnl>& jrnl_manager){
    for(int i=0; i<jrnl_manager.size();i++){
        jrnl& entry=jrnl_manager[i]; 
        std::string entry_string=std::to_string(entry.getid())+";"+entry.gettag()+";"+std::to_string(entry.getstamp())+";"+entry.getentry()+"\n";
        
        ssize_t bytes_written = 0; 
        std::size_t entry_size = entry_string.size();

        while(bytes_written < entry_size){
            ssize_t n = ::write(fd,entry_string.data()+bytes_written,entry_string.size()-bytes_written);
            if(n == -1){
                int e = errno;
                std::cerr<<"jrnl: failed to write the entry "<<entry.getid()<<" "<<std::strerror(e)<<"(errno:"<<e<<")\n";
                return false;
            }   
            if(n == 0){
                int e = errno;
                std::cerr<<"jrnl: bytes written 0, there's something wrong with the file system"<<entry.getid()<<" "<<std::strerror(e)<<"(errno:"<<e<<")\n";
                return false;
            }
            else{
                bytes_written += static_cast<std::size_t>(n);
            }


        }
    }
    return true;

}
void Manager::save(std::string PATH){
    
    bool write_ok;
    //Using POSIX functions to implement atomic saves. 
    std::string temp_path=PATH+".tmp";
    int file = open(temp_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC,0644);
    if(file==-1){
        int e=errno;
        std::cerr<<"Failed to open tmp file "<<std::strerror(e)<<"(errorno: "<<e<<")";
        return;
    }    
    write_ok = write_all(file, jrnl_manager);

    //fsync on the temporary file to write the data onto the disk
    int sy = ::fsync(file);
    if(sy == -1){
        int e=errno;
        std::cerr<<"Failed to fsync the tmp file "<<std::strerror(e)<<"(errorno: "<<e<<")";
        return;
    }
    //closing the file 
    int cl = ::close(file);
    if(cl == -1){
        int e=errno;
        std::cerr<<"Failed to close tmp file "<<std::strerror(e)<<"(errorno: "<<e<<")";
        return;
    }   
    //renaming and effectively replacing journal.txt with the comletely written .tmp file
    if(write_ok == true){
        int rn = ::rename(temp_path.c_str(),PATH.c_str());
        if(rn ==-1){
            int e=errno;
            std::cerr<<"Failed to rename tmp file "<<std::strerror(e)<<"(errorno: "<<e<<")";
            return;
        }
    }
    else{
        return;
    }
    //fsync the parent directory for extra verification;
    std::filesystem::path parent_dir=std::filesystem::path(PATH).parent_path();
    int dir=open(parent_dir.c_str(), O_RDONLY | O_DIRECTORY);
    if(dir == -1){
        int e=errno;
        std::cerr<<"Failed to open the parent directory "<<std::strerror(e)<<"(errorno: "<<e<<")";
        return;
    } 
    int dir_sy = ::fsync(dir);
    //directory fsync is not a huge criteria, so failure in doing so will only log the error.
    if(dir_sy==-1){
        int e=errno;
        std::cerr<<"Failed to fsync the parent directory "<<std::strerror(e)<<"(errorno: "<<e<<")";
    }
    int dir_cl = ::close(dir);
    if(dir_cl == -1){
        int e=errno;
        std::cerr<<"Failed to close the parent directory "<<std::strerror(e)<<"(errorno: "<<e<<")";
        return;
    }
}
