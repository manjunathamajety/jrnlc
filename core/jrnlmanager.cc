#include <jrnlmanager.h>

manager::manager(std::string PATH){
    
    std::ifstream file(PATH);
    //Initializes jrnl-objects in the jrnl_manager vector.
    std::string id,tag,stamp,txt;
    //while loops checks for if the entry can be compleltely scanned
    while (
        std::getline(file,id,';')&&
        std::getline(file,tag,';')&&
        std::getline(file,stamp,';')&&
        std::getline(file,txt)){
                
        long long times=std::stoll(stamp);
        time_t t=static_cast<time_t>(times);
        //pushes the entry to the last of the vector jrnl_manager
        jrnl_manager.emplace_back(std::stoi(id),tag,t,txt);
        
    }
    file.close();
    if(jrnl_manager.empty()){
        id_count=0;
    }
    else{
        id_count=jrnl_manager.back().getid();
    }
}

void manager::addentry(std::string txt,std::string tag){
    jrnl_manager.emplace_back(id_count+1,tag,timestamp(),txt);
}



void manager::save(std::string PATH){
    //Using POSIX functions to implement atomic saves. 
    std::string temp_path=PATH+".tmp";
    int file=open(temp_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC,0644);
    if(file==-1){
        int e=errno;
        std::cerr<<"Failed to open tmp file "<<std::strerror(e)<<"(errorno: "<<e<<")";
        return;
    }    
    for(int i=0;i<jrnl_manager.size();i++){
        jrnl& entry=jrnl_manager[i];
        std::string write_entry=std::to_string(entry.getid())+";"+entry.gettag()+";"+std::to_string(entry.getstamp())+";"+entry.getentry()+"\n";
        ::write(file,write_entry.data(),write_entry.size());
    }
    //perfo
    int sy = ::fsync(file);
    if(sy == -1){
        int e=errno;
        std::cerr<<"Failed to fsync the tmp file "<<std::strerror(e)<<"(errorno: "<<e<<")";
        return;
    }
    int cl = ::close(file);
    if(cl == -1){
        int e=errno;
        std::cerr<<"Failed to close tmp file "<<std::strerror(e)<<"(errorno: "<<e<<")";
        return;
    }   
    int rn = ::rename(temp_path.c_str(),PATH.c_str());
    if(rn ==-1){
        int e=errno;
        std::cerr<<"Failed to rename tmp file "<<std::strerror(e)<<"(errorno: "<<e<<")";
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
