#include "processor.h"

#include <string>
#include <vector>

#include "linux_parser.h"

using LinuxParser::CPUStates;
using std::stoi;
using std::string;
using std::vector;

// Return the aggregate CPU utilization
float Processor::Utilization() {
  vector<string> cpuUtilization = LinuxParser::CpuUtilization();

  long idleSinceBoot = LinuxParser::IdleJiffies();
  long totalJiffies = LinuxParser::Jiffies();

  long deltaTotal = totalJiffies - lastTotalJiffies_;
  long deltaIdle = idleSinceBoot - lastIdleJiffies_;

  lastTotalJiffies_ = totalJiffies;
  lastIdleJiffies_ = idleSinceBoot;

  return (deltaTotal - deltaIdle) / float(deltaTotal);
}