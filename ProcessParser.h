/**
 * @file: ProcessParser.h
 *
 * @brief:
 * 	CppND-System-Monitor: Header file for all the process functions.
 *     
 * @ingroup:
 * 	CppND-System-Monitor
 *
 * @author:
 * 	Eva Liu - evaliu2046@gmail.com
 * 
 * @date:
 * 	2019/Jun/22
 *
 */

#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"


using namespace std;

class ProcessParser{
    private:
        std::ifstream stream;
    public:
        static std::string getCmd(std::string pid);
        static std::vector<std::string> getPidList();
        static std::string getVmSize(std::string pid);
        static std::string getCpuPercent(std::string pid);
        static long int getSysUpTime();
        static std::string getProcUpTime(std::string pid);
        static std::string getProcUser(std::string pid);
        static std::vector<std::string> getSysCpuPercent(std::string coreNumber = "");
        static float getSysRamPercent();
        static std::string getSysKernelVersion();
        static int getNumberOfCores();
        static int getTotalThreads();
        static int getTotalNumberOfProcesses();
        static int getNumberOfRunningProcesses();
        static std::string getOSName();
        static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2);
        static bool isPidExisting(std::string pid);
};


float getSysActiveCpuTime(vector<string> values){
    return (stof(values[S_USER]) +
            stof(values[S_NICE]) +
            stof(values[S_SYSTEM]) +
            stof(values[S_IRQ]) +
            stof(values[S_SOFTIRQ]) +
            stof(values[S_STEAL]) +
            stof(values[S_GUEST]) +
            stof(values[S_GUEST_NICE]));
}

float getSysIdleCpuTime(vector<string>values){
    return (stof(values[S_IDLE]) + stof(values[S_IOWAIT]));
}

/**
 * @function:
 *  string ProcessParser::getCmd(string pid);
 *  This function command that executed the process.
 *
 * @param: a unique process ID (PID)
 * @return: command of the current process.
 */
std::string ProcessParser::getCmd(std::string pid){
    std::string line;
    ifstream stream = Util::getStream(Path::basePath()+pid+"/"+Path::cmdPath());
    getline(stream, line);
    return line;
}


/**
 * @function:
 *   vector<string> ProcessParser::getPidList();
 *  This function returns all the PID process currently running.
 *
 * @param: NULL
 * @return: all the running process ID.
 */
std::vector<std::string> ProcessParser::getPidList(){
    DIR* dir;
    std::vector<std::string> container;
    if (!(dir = opendir("/proc")))
        throw std::runtime_error(std::strerror(errno));

    // Iterate the /proc dir for all the directories with number
    while (dirent* dirp = readdir(dir)) {
        if (dirp->d_type != DT_DIR)
            continue;

        if (all_of(dirp->d_name, dirp->d_name+std::strlen(dirp->d_name),[](char c){return std::isdigit(c);})) {
            container.push_back(dirp->d_name);
        }
    }
    if (closedir(dir))
        throw std::runtime_error(std::strerror(errno));
    return container;
}


/**
 * @function:
 *  std::string ProcessParser::getVmSize(string pid);
 *  This function retrieves data for a specific running process
 *
 * @param: a unique process ID (PID)
 * @return: memory usage data.
 */
std::string ProcessParser::getVmSize(std::string pid){
    std::string line;
    //Declaring search attribute for file
    std::string name = "VmData";
    std::string value;
    float result;
    // Opening stream for specific file
    ifstream stream = Util::getStream(Path::basePath() + pid + Path::statusPath());
    while(std::getline(stream, line)){
        // Searching line by line
        if (line.compare(0, name.size(),name) == 0) {
            // slicing string line on ws for values using sstream
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            //conversion kB -> GB
            result = (stof(values[1])/float(1024*1024));
            break;
        }
    }
    return to_string(result);
} 


/**
 * @function:
 *  std::string ProcessParser::getCpuPercent(string pid);
 *  This function return CPU usage percent from a process.
 *
 * @param: a unique process ID (PID)
 * @return: CPU usage.
 */
std::string ProcessParser::getCpuPercent(std::string pid){
    string line;
    string value;
    float result;
    ifstream stream = Util::getStream((Path::basePath()+ pid +"/"+ Path::statPath()));
    getline(stream, line);
    string str = line;
    istringstream buf(str);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg, end);

    // acquiring relevant times for calculation of active occupation of CPU for selected process
    float utime = stof(ProcessParser::getProcUpTime(pid));
    float stime = stof(values[14]);
    float cutime = stof(values[15]);
    float cstime = stof(values[16]);
    float starttime = stof(values[21]);
    float uptime = ProcessParser::getSysUpTime();
    float freq = sysconf(_SC_CLK_TCK);
    float total_time = utime + stime + cutime + cstime;
    float seconds = uptime - (starttime/freq);
    result = 100.0*((total_time/freq)/seconds);
    return to_string(result);
}


/**
* @function:
*  static long int ProcessParser::getSysUpTime();
*  This function gets the system up time.
*
* @param: NULL
* @return: System up time value.
*/
long int ProcessParser::getSysUpTime(){
    string line;
    ifstream stream = Util::getStream((Path::basePath() + Path::upTimePath()));
    getline(stream,line);
    istringstream buf(line);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg, end);
    return stoi(values[0]);
}


/**
* @function:
*  std::string ProcessParser::getProcUpTime(string pid);
*  This function gets the system frequency for calculating the process up time.
*
* @param: a unique process ID (PID)
* @return: Process up time value.
*/
std::string ProcessParser::getProcUpTime(std::string pid){
    string line;
    ifstream stream = Util::getStream((Path::basePath() + pid +"/"+ Path::statPath()));
    getline(stream, line);
    string str = line;
    istringstream buf(str);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg, end);
    float num = stof(values[13])/sysconf(_SC_CLK_TCK);
    return to_string(num);
}


/**
* @function:
*  string ProcessParser::getProcUser(string pid);
*  This function gets the process user.
*
* @param: a unique process ID (PID)
* @return: Process user.
*/
std::string ProcessParser::getProcUser(std::string pid){
    string line;
    string name = "Uid:";
    string result = "";
    ifstream stream = Util::getStream((Path::basePath() + pid +"/"+ Path::statusPath()));
    // Get user ID based on the pid value
    while (std::getline(stream, line)){
        if (line.compare(0,name.size(),name)==0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            result = values[1];
            break;
        }
    }
    // Get name of the user with UID
    stream = Util::getStream("/etc/passwd/");
    name = ("x:" + result);
    while (std::getline(stream, line)) {
        if (line.find(name) != std::string::npos){
            result = line.substr(0, line.find(":"));
            return result;
        }
    }
    return "";
}


/**
* @function:
*  vector<string> ProcessParser::getSysCpuPercent(string coreNumber = "");
*  This function contains information on overall cpu usage, as well stats for indivi*  dual cores.
*
* @param: string coreNumber
* @return: system CPU information.
*/
std::vector<std::string> ProcessParser::getSysCpuPercent(std::string coreNumber){
    string line;
    string name = "cpu" + coreNumber;
    ifstream stream = Util::getStream((Path::basePath()+Path::statPath()));
    while (std::getline(stream, line)){
        if (line.compare(0, name.size(), name) == 0){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            return values;
        }
    }
    return (vector<string>());
}


/**
* @function:
*  float ProcessParser::getSysRamPercent();
*  This function calculates RAM usage in percentage.
*
* @param: NULL
* @return: RAM usage in percentage.
*/
float ProcessParser::getSysRamPercent(){
    string line;
    string name1 = "MemAvailable:";
    string name2 = "MemFree:";
    string name3 = "Buffers:";
    string value;
    int result;
    ifstream stream = Util::getStream((Path::basePath()+Path::memInfoPath()));
    float total_mem = 0;
    float free_mem = 0;
    float buffers = 0;
    while (std::getline(stream,line)){
        if ((total_mem != 0) && (free_mem != 0))
            break;
        if (line.compare(0, name1.size(),name1) == 0){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            total_mem = stof(values[1]);
        }
        if (line.compare(0, name2.size(),name2) == 0){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            free_mem = stof(values[1]);
        }
        if (line.compare(0, name3.size(),name3) == 0){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            buffers = stof(values[1]);
        }
    }
    return float(100.0*(1-(free_mem/(total_mem-buffers))));
}


/**
* @function:
*  string ProcessParser::getSysKernelVersion();
*  This function gets data about the kernel version.
*
* @param: NULL
* @return: Kernel Version.
*/
std::string ProcessParser::getSysKernelVersion(){
    string line;
    string name = "Linux version ";
    ifstream stream = Util::getStream((Path::basePath()+Path::versionPath()));
    while (std::getline(stream, line)){
        if (line.compare(0, name.size(), name) == 0){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            return values[2];
        }
    }
    return "";
}

/**
* @function:
*  int ProcessParser::getNumberOfCores();
*  This function gets the number of Cores of this computer.
*
* @param: NULL
* @return: Number of cores.
*/
int ProcessParser::getNumberOfCores(){
    string line;
    string name = "cpu cores";
    ifstream stream = Util::getStream((Path::basePath()+"cpuinfo"));
    while (std::getline(stream, line)){
        if (line.compare(0, name.size(), name) == 0){
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            return stoi(values[3]);
        }
    }
    return 0;
}


/**
* @function:
*  int ProcessParser::getTotalThreads();
*  This function gets the total threads count.
*
* @param: NULL
* @return: Total threads count.
*/
int ProcessParser::getTotalThreads(){
    string line;
    int result = 0;
    string name = "Threads:";
    vector<string> _list = ProcessParser::getPidList();
    for (int i=0; i<_list.size();i++) {
        string pid = _list[i];
        ifstream stream = Util::getStream((Path::basePath()+pid+Path::statusPath()));
        while (std::getline(stream, line)){
            if (line.compare(0, name.size(), name) == 0){
                istringstream buf(line);
                istream_iterator<string> beg(buf), end;
                vector<string> values(beg, end);
                result += stoi(values[1]);
                break;
            }
        }
    }
    return result;
}


/**
* @function:
*  int ProcessParser::getTotalNumberOfProcesses();
*  This function gets the total number of processes.
*
* @param: NULL
* @return: Total process count.
*/
int ProcessParser::getTotalNumberOfProcesses(){
    string line;
    int result = 0;
    string name = "processes";
    ifstream stream = Util::getStream(Path::basePath() + Path::statPath());
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(), name) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            result += stoi(values[1]);
            break;
        }
    }
    return result;
}


/**
* @function:
*  int ProcessParser::getTotalNumberOfRunningProcesses();
*  This function gets the total number of running processes.
*
* @param: NULL
* @return: Total running process count.
*/   
int ProcessParser::getNumberOfRunningProcesses(){
    string line;
    int result = 0;
    string name = "procs_running";
    ifstream stream = Util::getStream(Path::basePath() + Path::statPath());
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(), name) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            result += stoi(values[1]);
            break;
        }
    }
    return result;
}


/**
* @function:
*  string ProcessParser::getOSName();
*  This function gets the name of operating system.
*
* @param: NULL
* @return: Name of Operating System.
*/
std::string ProcessParser::getOSName(){
    string line;
    string name = "PRETTY_NAME=";
    ifstream stream = Util::getStream("/etc/os-release/");
    while (std::getline(stream, line)){
        if (line.compare(0, name.size(), name) == 0){
            std::size_t found = line.find("=");
            found ++;
            string result = line.substr(found);
            result.erase(std::remove(result.begin(),result.end(),'"'),result.end());
            return result;
        }
    }
    return "";
}

/**
* @function:
*  std::string ProcessParser::PrintCpuStats(std::vector<std::string> values1, std::v*  ector<std::string>values2);
*  This function prints the CPU status;
*
* @param: previous time and current time;
* @return: CPU usage;
*/
std::string ProcessParser::PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2){
    float activeTime = getSysActiveCpuTime(values2)-getSysActiveCpuTime(values1);
    float idleTime = getSysIdleCpuTime(values2)-getSysIdleCpuTime(values1);
    float totalTime = activeTime + idleTime;
    float result = 100.0*(activeTime/totalTime);
    return to_string(result);
}


/**
* @function:
*  bool ProcessParser::isPidExisting(string pid);
*  This function checks if a pid is in the current process list.
*
* @param: process ID
* @return: True or False.
*/
bool ProcessParser::isPidExisting(std::string pid){
    bool result = false;
    vector<string> _list = ProcessParser::getPidList();
    for (int i=0; i<_list.size();i++) {
        if (_list[i] == pid)
            result = true;
    }
    return result;
}


