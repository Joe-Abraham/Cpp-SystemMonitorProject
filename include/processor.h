#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>
#include <string>
#include "linux_parser.h"

class Processor {
 public:
  float Utilization();
 private:
  long cached_active_ticks_{0};
  long cached_idle_ticks_{0};
};

#endif