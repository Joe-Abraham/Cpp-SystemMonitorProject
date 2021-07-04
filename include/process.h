#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class Process {
 public:
  int Pid() const;                               
  std::string User() const;                      
  std::string Command() const;                   
  float CpuUtilization() const;                  
  std::string Ram() const;                       
  long int UpTime() const;                       
  bool operator<( const Process& a) const;
  bool operator>(const Process& a) const;
  Process(int pid);
  void CpuUtilization(long active_ticks, long system_ticks);

 private:
  int pid_;
  float cpu_{0};
  long cached_active_ticks_{0};
  long cached_idle_ticks_{0};
  long cached_system_ticks_{0};
};

#endif