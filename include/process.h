#ifndef PROCESS_H
#define PROCESS_H

#include <string>

#include "linux_parser.h"

using std::string;
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid) : pid_(pid), cpu_(CpuUtilization()) {}
  int Pid();
  std::string User();
  std::string Command();
  float CpuUtilization();
  std::string Ram();
  long int UpTime();
  bool operator<(Process const& a) const;

 private:
  int pid_;
  float cpu_;
};

#endif