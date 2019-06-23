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
    static string getCmd(string pid);
    static vector<string> getPidList();
    static std::string getVmSize(string pid);
    static std::string getCpuPercent(string pid);
    static long int getSysUpTime();
    static std::string getProcUpTime(string pid);
    static string getProcUser(string pid);
    static vector<string> getSysCpuPercent(string coreNumber = "");
    static float getSysRamPercent();
    static string getSysKernelVersion();
    static int getNumberOfCores();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfRunningProcesses();
    static string getOSName();
    static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2);
    static bool isPidExisting(string pid);
};


/**
 * @function:
 *  string ProcessParser::getCmd(string pid);
 *  This function command that executed the process.
 *
 * @param: a unique process ID (PID)
 * @return: command of the current process.
 */
static string ProcessParser::getCmd(string pid){
    string line;
    ifstream stream = Util::getStream((Path::basePath()+pid+Path::cmdPath()));
    getline(stream, line);
    return line;
}


/**
 * @function:
 *  string ProcessParser::getCmd(string pid);
 *  This function command that executed the process.
 *
 * @param: a unique process ID (PID)
 * @return: command of the current process.
 */
static vector<string> ProcessParser::getPidList(){


}


/**
 * @function:
 *  std::string ProcessParser::getVmSize(string pid);
 *  This function retrieves data for a specific running process
 *
 * @param: a unique process ID (PID)
 * @return: memory usage data.
 */
static std::string ProcessParser::getVmSize(string pid){
    string line;
    //Declaring search attribute for file
    string name = "VmData";
    string value;
    float result;
    // Opening stream for specific file
    ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));
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
static std::string ProcessParser::getCpuPercent(string pid){
    string line;
    string value;
    float result;
    ifstream strean = Util::getStream((Path::basePath()+ pid +"/"+ Path::statPath()));
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
static long int ProcessParser::getSysUpTime(){
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
static std::string ProcessParser::getProcUpTime(string pid){
    string line;
    ifstream stream = Util::getStream((Path::basePath() + pid +"/"+ Path::statPath()));
    getline(stream, line);
    string str = line;
    istringstream buf(str);
    istream_iterator<string> beg(bug), end;
    vector<string> values(beg, end);
    return to_string(float(stof(values[13]/sysconf(_SC_CLK_TCK))));
}


/**
* @function:
*  string ProcessParser::getProcUser(string pid);
*  This function gets the process user.
*
* @param: a unique process ID (PID)
* @return: Process user.
*/
static string ProcessParser::getProcUser(string pid){
    string line;
    string name = "Uid:";
    string result = "";
    ifstream stream = Util::getStream((Path::basePath() + pid + + Path::statusPath()));
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
    stream = Util::getStream("/etc/passwd");
    name = ("x:" + result);
    while (std::getline(stream, line)) {
        if (line.find(name) != std::string::npos){
            result = line.substr(0, line.find(":"));
            return result;
        }
    }
    return "";
}

