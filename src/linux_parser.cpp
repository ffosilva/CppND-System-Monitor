#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#if __has_include(<filesystem>)
#include <filesystem>
#define USE_FILESYSTEM
#endif

#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// An example of how to read data from the filesystem
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

// List the Pids (using std::filesystem)
vector<int> LinuxParser::Pids() {
  vector<int> pids;

#ifdef USE_FILESYSTEM
  std::filesystem::directory_iterator dir_iter(kProcDirectory);
  for (auto file : dir_iter) {
    string filename(file.path().filename());
    if (std::all_of(filename.begin(), filename.end(), isdigit)) {
      int pid = stoi(filename);
      pids.push_back(pid);
    }
  }
#else
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
#endif /* !USE_FILESYSTEM */

  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line, key, value;
  float memtotal, memavailable;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      std::istringstream valuestream;
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          std::replace(value.begin(), value.end(), '_', ' ');
          valuestream.str(value);
          valuestream >> memtotal;
        }
        if (key == "MemAvailable") {
          std::replace(value.begin(), value.end(), '_', ' ');
          valuestream.str(value);
          valuestream >> memavailable;
        }
      }
    }
  }

  return 1 - (memavailable / memtotal);
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  float uptime, coreidle;
  string line;

  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open() && std::getline(filestream, line)) {
    std::istringstream linestream(line);
    if (linestream >> uptime >> coreidle) {
      return long(uptime);
    }
  }

  return 0;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long totalJiffies = 0;

  for (auto timer : CpuUtilization()) {
    totalJiffies += stol(timer);
  }

  return totalJiffies;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  vector<string> stats;
  string pidStr = to_string(pid);

  std::ifstream filestream(kProcDirectory + pidStr + kStatFilename);
  if (filestream.is_open()) {
    string value;

    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      stats.push_back(value);
    }

    return std::stol(stats[13]) + std::stol(stats[14]) + std::stol(stats[15]) +
           std::stol(stats[16]);
  }

  return 0;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return Jiffies() - IdleJiffies(); }

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpuUtilization = CpuUtilization();
  return std::stol(cpuUtilization[CPUStates::kIdle_]) +
         std::stol(cpuUtilization[CPUStates::kIOwait_]);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> ret;
  string value, line;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream.ignore(256, ' ');
    while (linestream >> value) ret.push_back(value);
  }
  return ret;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  string pidStr = to_string(pid);

  std::ifstream filestream(kProcDirectory + pidStr + kCmdlineFilename);

  if (filestream.is_open()) {
    std::getline(filestream, line);
  }

  return line;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string key, value;
  string pidStr = to_string(pid);

  std::ifstream stream(kProcDirectory + pidStr + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") {
        return to_string(std::stol(value) / 1024);
      }
    }
  }
  return string();
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string value;
  string pidStr = to_string(pid);

  std::ifstream filestream(kProcDirectory + pidStr + kStatusFilename);
  if (filestream.is_open()) {
    string key;
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  int _uid = stoi(Uid(pid));
  string line;
  string name;

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    string junk;
    string uid;

    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> name >> junk >> uid) {
        if (std::to_string(_uid) == uid) {
          return name;
        }
      }
    }
  }
  return name;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line, values;
  vector<string> stat_values;
  string pidStr = to_string(pid);

  std::ifstream stream(kProcDirectory + pidStr + kStatFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);

    // see stat manpages
    while (linestream >> values) stat_values.push_back(values);

    return UpTime() - (stol(stat_values[21]) / sysconf(_SC_CLK_TCK));
  }

  return 0;
}
