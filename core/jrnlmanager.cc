#include <jrnlmanager.h>


manager::manager(){
    int i=0;
    std::ifstream file("jrnl.txt");
    //Initializes jrnl-objects in the jrnl_manager vector.
    std::string id,tag,stamp,txt;
    //while loops checks for if the entry can be compleltely scanned
    while (
        std::getline(file,id,';')&&
        std::getline(file,tag,';')&&
        std::getline(file,stamp,';')&&
        std::getline(file,txt)){
        
        char c=tag[0];
        long long times=std::stoll(stamp);
        time_t t=static_cast<time_t>(times);
        //pushes the entry to the last of the vector jrnl_manager
        jrnl_manager.emplace_back(std::stoi(id),c,t,txt);
        i++;
        
    }
    file.close();
    //clearing the file of all entries, so as to prevent duplication while saving
    std::ofstream clearfile("jrnl.txt",std::ios::out|std::ios::trunc); 
    clearfile.close();

}


void manager::addentry(std::string txt){
    jrnl_manager.emplace_back(67,'A',timestamp(),txt);
    std::cout<<"Entry added successfully";
}

void manager::display(){
    
    for(int i=0;i<jrnl_manager.size();i++){
        int id=jrnl_manager[i].getid();
        char tag=jrnl_manager[i].gettag();
        time_t timestamp=jrnl_manager[i].getstamp();
        std::string txt=jrnl_manager[i].getentry();

        std::cout<<BLUE<<id<<RESET<<" "<<tag<<" "<<GREEN<<timestamp<<RESET<<" "<<txt<<"\n";
    }

}

void manager::save(){
    
    std::ofstream savefile("jrnl.txt",std::ios::app);
    
        for(int i=0;i<jrnl_manager.size();i++){
        savefile<<jrnl_manager[i].getid()<<";"<<jrnl_manager[i].gettag()<<";"<<jrnl_manager[i].getstamp()<<";"<<jrnl_manager[i].getentry()<<"\n";

    }    
    
    savefile.close();

}
