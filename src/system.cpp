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

#include "process.h"
#include "processor.h"

using std::ifstream;
using std::set;
using std::size_t;
using std::string;
using std::unordered_map;
using std::vector;

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { return processes_; }

// Return the system's kernel identifier (string)
std::string System::Kernel() {
  if (!this->Kernel_.empty()) {
    return this->Kernel_;
  }

  this->Kernel_ = "<NOT DETECTED>";

  struct utsname name;
  uname(&name);

  this->Kernel_ = string(name.sysname) + " " + string(name.release);

  return this->Kernel_;
}

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return 0.0; }

// Return the operating system name
std::string System::OperatingSystem() {
  if (!this->OperatingSystem_.empty()) {
    return this->OperatingSystem_;
  }

  this->OperatingSystem_ = "<NOT DETECTED>";

  unordered_map<string, string> probeLocations = {
      {"/etc/os-release", "PRETTY_NAME"},
      {"/etc/lsb-release", "DISTRIB_DESCRIPTION"}};

  ifstream infile;
  string OSNameKey, line;
  for (auto& it : probeLocations) {
    infile.open(it.first);
    if (infile.good()) {
      OSNameKey = it.second;
      break;
    }
  }

  while (getline(infile, line)) {
    if (line.length() > 0 && line[0] != '#') {
      size_t found = line.find("=");
      if (found == string::npos) {
        continue;
      }
      string key, value;
      key = line.substr(0, found);
      if (key == OSNameKey) {
        value = line.substr(found + 1);
        if (value.front() == '"' && value.back() == '"') {
          value = value.substr(1, value.find_last_of('"') - 1);
        }
        OperatingSystem_ = value;
      }
    }
  }

  return OperatingSystem_;
}

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return 0; }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return 0; }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return 0; }