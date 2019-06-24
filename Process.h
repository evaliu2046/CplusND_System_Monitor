/**
 * @file: Process.h
 *
 * @brief:
 * 	CppND-System-Monitor: Header file for all the process representation.
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
#include <string>

using namespace std;
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
private:
    string pid;
    string user;
    string cmd;
    string cpu;
    string mem;
    string upTime;

public:
    Process(string pid){
        this->pid = pid;
        this->user = ProcessParser::getProcUser(pid);
        this->mem = ProcessParser::getVmSize(pid);
        this->cpu = ProcessParser::getCpuPercent(pid);
        this->upTime = ProcessParser::getProcUpTime(pid);
        this->cmd = getCmd();
    }
    void setPid(string pid);
    string getPid()const;
    string getUser()const;
    string getCmd()const;
    string getCpu()const;
    string getMem()const;
    string getUpTime()const;
    string getProcess();
};
void Process::setPid(string pid){
    this->pid = pid;
}
string Process::getPid()const {
    return this->pid;
}
string Process::getUser()const {
    return this->user;
}
string Process::getCmd()const {
    return this->cmd;
}
string Process::getCpu()const {
    return this->cpu;
}
string Process::getMem()const {
    return this->mem;
}
string Process::getUpTime()const {
    return this->upTime;
}

/**
 * @function:
 *  string Process::getProcess();
 *  This function gets the process information.
 *
 * @param: NULL
 * @return: process information.
 */
string Process::getProcess(){
    if(!ProcessParser::isPidExisting(this->pid))
        return "";
    this->mem = ProcessParser::getVmSize(this->pid);
    this->upTime = ProcessParser::getProcUpTime(this->pid);
    this->cpu = ProcessParser::getCpuPercent(this->pid);

    return (this->pid + "   " + this->user 
            + "   " + this->mem.substr(0,5) 
            + "   " + this->cpu.substr(0,5)
            + "   " + this->upTime.substr(0,5)
            + "   " + this->cmd.substr(0,30));
}
