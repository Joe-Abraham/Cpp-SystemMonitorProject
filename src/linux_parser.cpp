#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <sstream>
#include <string>
#include <vector>

using std::getline;
using std::ifstream;
using std::istringstream;
using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float mem_total{1};
  float mem_free{0};
  float buffer{0};
  string token;
  ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (stream >> token) {
      if (token == "MemTotal:") {
        if (stream >> token) {
          mem_total = stof(token);
        }
      } else if (token == "MemFree:") {
        if (stream >> token) {
          mem_free = stof(token);
        }
      } else if (token == "Buffers:") {
        if (stream >> token) {
          buffer = stof(token);
        }
      }
    }
  }
  return 1 - mem_free / (mem_total - buffer);
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  string token;
  ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    getline(stream, line);
    std::istringstream stream(line);
    if (stream >> token) {
      return stoi(token);
    }
  }
  return 0;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return UpTime() * sysconf(_SC_CLK_TCK); }

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line, token;
  vector<string> values;
  ifstream stream(LinuxParser::kProcDirectory + to_string(pid) +
                  LinuxParser::kStatFilename);

  if (stream.is_open()) {
    getline(stream, line);
    istringstream linestream(line);
    while (linestream >> token) {
      values.push_back(token);
    }
  }
  long jiffles{0};
  if (values.size() > 21) {
    long user = stol(values[13]);
    long kernel = stol(values[14]);
    long children_user = stol(values[15]);
    long children_kernel = stol(values[16]);
    jiffles = user + kernel + children_kernel + children_user;
  }
  return jiffles;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> time = CpuUtilization();
  return (stol(time[CPUStates::kUser_]) + stol(time[CPUStates::kNice_]) +
          stol(time[CPUStates::kSystem_]) + stol(time[CPUStates::kIRQ_]) +
          stol(time[CPUStates::kSoftIRQ_]) + stol(time[CPUStates::kSteal_]) +
          stol(time[CPUStates::kGuest_]) + stol(time[CPUStates::kGuestNice_]));
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> time = CpuUtilization();
  return (stol(time[CPUStates::kIOwait_]) + stol(time[CPUStates::kIdle_]));
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line, token;
  vector<string> values;
  ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (getline(stream, line)) {
      istringstream stream(line);
      while (stream >> token) {
        if (token == "cpu") {
          while (stream >> token) {
            values.push_back(token);
          }
        }
      }
    }
  }
  return values;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() 
{
  string line,key,value;
  ifstream stream(kProcDirectory+kStatFilename);
  if(stream.is_open())
  {
    while(getline(stream,line))
    {
      istringstream stream(line);
      while(stream >> key >> value)
      {
        if (key == "processes")
        {
          return stoi(value);
        }
      }
    }
  }
  return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses()  
{
  string line,key,value;
  ifstream stream(kProcDirectory+kStatFilename);
  if(stream.is_open())
  {
    while(getline(stream,line))
    {
      istringstream stream(line);
      while(stream >> key >> value)
      {
        if (key == "procs_running")
        {
          return stoi(value);
        }
      }
    }
  }
  return 0;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) 
{
  string line;
  ifstream stream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kCmdlineFilename);

  if(stream.is_open())
  {
    getline(stream,line);
    return line;
  }
  return string();
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) 
{ 
  string token;
  ifstream stream(LinuxParser::kProcDirectory + to_string(pid) +
                  LinuxParser::kStatFilename);
  if(stream.is_open())
  {
    while(stream >> token)
    {
      if (token == "VmSize:")
      {
        if (stream >> token)
        {
          return to_string(stoi(token)/1024);
        }
      }
    }
  }

  return string(); 
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string token;
  ifstream stream(LinuxParser::kProcDirectory + to_string(pid) +
                  LinuxParser::kStatFilename);

  if (stream.is_open()) {
    while (stream >> token) {
      if ((token == "Uid:") and (stream >> token)) {
        return token;
      }
    }
  }
  return string("");
}
// Read and return the user associated with a process
string LinuxParser::User(int pid) 
{
  ifstream stream(LinuxParser::kPasswordPath);
  if (stream.is_open()) 
  {
    string line;
    string token = "x:" + Uid(pid);
    while (getline(stream, line)) 
    {
      auto marker = line.find(token);
      if (marker != string::npos) 
      {
        return line.substr(0, marker - 1);
      }
    }
  }
  return string();
}

// Read and return the uptime of a process
long int LinuxParser::UpTime(int pid) {
  long int time{0};
  string token;
  ifstream stream(LinuxParser::kProcDirectory + to_string(pid) +
                  LinuxParser::kStatFilename);

  if (stream.is_open()) {
    for (int i = 0; stream >> token; ++i) {
      if (i == 13) {
        long int time{stol(token)};
        time /= sysconf(_SC_CLK_TCK);
        return time;
      }
    }
  }
  return time;
}