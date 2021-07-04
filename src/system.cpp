#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"

#include "process.h"
#include "processor.h"

using std::map;
using std::set;
using std::size_t;
using std::string;
using std::vector;

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() 
{ 
    vector<int> pids{LinuxParser::Pids()}; 
    set<int> extant_pids;
    for (Process const& process: processes_)
    {
        extant_pids.insert(process.Pid());
    }

    for (int pid : pids)
    {
        if (extant_pids.find(pid) == extant_pids.end())
        processes_.emplace_back(pid);
    }
    
    for (auto& process : processes)
    {
      process.CpuUtilization(LinuxParser::ActiveJiffies(process.Pid()),
                             LinuxParser::ActiveJiffies());
    }

    std::sort(processes_.begin(), processes_.end, std::greater<process>());
    return processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::kernel(); }

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcess(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return Linux_parser::TotalProcess(); }

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }