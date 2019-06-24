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
#include <string>
#include <vector>
using std::string;
using std::vector;

class ProcessContainer {
    public:
        ProcessContainer()
        {
            this->refreshList();
        }
        void refreshList();
        string printList();
        vector<string> getList();

    private:
        vector<Process> _list;
};

/**
 * @function:
 *  std::string ProcessContainer::refreshList();
 *  This function updates current process list.
 *
 * @param: NULL
 * @return: NULL
 */
void ProcessContainer::refreshList()
{
    vector<string> pids = ProcessParser::getPidList();
    this->_list.clear();
    for (auto pid : pids) {
        Process proc(pid);
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
string ProcessContainer::printList()
{
    std::string result="";
    for (auto i : _list) {
        result += i.getProcess();
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
vector<string> ProcessContainer::getList() 
{
    vector<string> values;
    for (int i = (this->_list.size()-10); i < this->_list.size(); i++){
        values.push_back(this->_list[i].getProcess());
    }
    return values;
}
