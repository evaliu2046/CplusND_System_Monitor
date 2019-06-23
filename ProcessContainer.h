/**
 * @file: ProcessContainer.h
 *
 * @brief:
 * 	CppND-System-Monitor: Header file for containing all of the processes.
 *     
 * @ingroup:
 * 	CppND-System-Monitor
 *
 * @author:
 * 	Eva Liu - evaliu2046@gmail.com
 * 
 * @date:
 * 	2019/Jun/23
 *
 */

#include "Process.h"
#include <vector>
class ProcessContainer{

private:
    std::vector<Process>_list;
public:
    ProcessContainer(){
        this->refreshList();
    }
    void refreshList();
    std::string printList();
    std::vector<std::vector<std::string>> getList();
};


/**
 * @function:
 *  std::string ProcessContainer::refreshList();
 *  This function updates current process list.
 *
 * @param: NULL
 * @return: NULL
 */
void ProcessContainer::refreshList(){
    std::vector<std::string> pidList = ProcessParser::getPidList();
    this->_list.clear();
    for(int i=0;i<pidList.size();i++){
        Process proc(pidList[i]);
        this->_list.push_back(proc);
    }
}


/**
 * @function:
 *  std::string ProcessContainer::printList();
 *  This function prints all the processes.
 *
 * @param: NULL
 * @return: string of processes.
 */
std::string ProcessContainer::printList(){
    std::string result="";
    for(int i=0;i<this->_list.size();i++){
        result += this->_list[i].getProcess();
    }
    return result;
}


/**
 * @function:
 *  std::vector<std::vector<std::string> > ProcessContainer::getList();
 *  The getter function returns the list of processes.
 *
 * @param: NULL
 * @return: List of the running process.
 */
std::vector<std::vector<std::string> > ProcessContainer::getList(){
    std::vector<std::vector<std::string>> values;
    std::vector<std::string> stringifiedList;
    for(int i=0; i<ProcessContainer::_list.size(); i++){
        stringifiedList.push_back(ProcessContainer::_list[i].getProcess());
    }
    int lastIndex = 0;
    for (int i=0; i<stringifiedList.size();i++){
        if(i %10 == 0 && i > 0){
          std::vector<std::string>  sub(&stringifiedList[i-10], &stringifiedList[i]);
          values.push_back(sub);
          lastIndex = i;
        }
        if(i == (ProcessContainer::_list.size() - 1) && (i-lastIndex)<10){
            std::vector<std::string> sub(&stringifiedList[lastIndex],&stringifiedList[i+1]);
            values.push_back(sub);
        }
   }
    return values;
}
