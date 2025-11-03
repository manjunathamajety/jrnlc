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
                
        char c=tag[0];
        long long times=std::stoll(stamp);
        time_t t=static_cast<time_t>(times);
        //pushes the entry to the last of the vector jrnl_manager
        jrnl_manager.emplace_back(std::stoi(id),c,t,txt);
        
    }
    file.close();
    //clearing the file of all entries, so as to prevent duplication while saving
    std::ofstream clearfile(PATH,std::ios::out|std::ios::trunc); 
    clearfile.close();

}

void manager::addentry(std::string txt,char tag){
    id_count=jrnl_manager.back().getid(); 
    jrnl_manager.emplace_back(id_count+1,tag,timestamp(),txt);

}

void manager::display(std::string range){
    
    int start, end;
    //manual parsing logic to set start and end based upon the request
    if(range.length()>2){
        
    }
    else if(range.length()==1){
            //function to print the entire journal 
        if(range=="*"){
            start=0;
            end=jrnl_manager.size();
        }
        else{
            start=std::stoi(range)-1;
            end=start+1;
        }
    }
    else if(range.length()==2){
        if(range[0]=='*'){
            start=0;
            end=std::stoi(range.substr(1,range.size()));
        }
        else if(range[1]=='*'){
            start=jrnl_manager.size()-std::stoi(range.substr(0,range.size()-1));
            end=jrnl_manager.size();
        }
    }
    
    for(int i=start;i<end;i++){
        //each iteration of loop loads the corresponding jrnl_manager element into a temporary variable for display 
        int id=jrnl_manager[i].getid();
        char tag=jrnl_manager[i].gettag();
        time_t timestamp=jrnl_manager[i].getstamp();
        std::string txt=jrnl_manager[i].getentry();
        //Printing each entry with formatting 
        std::cout<<GREEN<<id<<RESET<<" "<<tag<<" "<<GREEN<<timestamp<<RESET<<" "<<txt<<"\n";
    }

}

void manager::save(std::string PATH){
    
    std::ofstream savefile(PATH,std::ios::app);
        //Loop uses jrnl_manager size as a limiting condition    
        for(int i=0;i<jrnl_manager.size();i++){
        savefile<<jrnl_manager[i].getid()<<";"<<jrnl_manager[i].gettag()<<";"<<jrnl_manager[i].getstamp()<<";"<<jrnl_manager[i].getentry()<<"\n";

    }    
    
    savefile.close();

}
