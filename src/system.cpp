#include "system.h"

#include <sys/utsname.h>
#include <unistd.h>

#include <cstddef>
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::ifstream;
using std::set;
using std::size_t;
using std::string;
using std::unordered_map;
using std::vector;

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() {
  processes_.clear();
  for (auto pid : LinuxParser::Pids()) {
    processes_.push_back(pid);
  }

  std::sort(this->processes_.begin(), this->processes_.end());

  return processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() {
  if (!this->Kernel_.empty()) {
    return this->Kernel_;
  }

  this->Kernel_ = LinuxParser::Kernel();

  return this->Kernel_;
}

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() {
  if (!this->OperatingSystem_.empty()) {
    return this->OperatingSystem_;
  }

  this->OperatingSystem_ = LinuxParser::OperatingSystem();

  return OperatingSystem_;
}

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }