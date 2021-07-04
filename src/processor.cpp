#include "processor.h"

float Processor::Utilization() 
{
  const long active_ticks = LinuxParser::ActiveJiffies();
  const long idle_ticks = LinuxParser::IdleJiffies();
  const long duration_active{active_ticks - cached_active_ticks_};
  const long duration_idle{idle_ticks - cached_idle_ticks_};
  const long duration{duration_active - duration_idle};
  cached_idle_ticks_ = idle_ticks;
  cached_active_ticks_ = active_ticks;
  return (static_cast<float>(duration_active) / duration);
}