#include "format.h"

#include <iomanip>
#include <sstream>
#include <string>

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  int outputSec = seconds % 60;
  int outputMin = (seconds / 60) % 60;
  int outputHour = (seconds / 3600);

  std::stringstream sstream;

  sstream << std::setfill('0') << std::setw(2) << outputHour << ":";
  sstream << std::setfill('0') << std::setw(2) << outputMin << ":";
  sstream << std::setfill('0') << std::setw(2) << outputSec;

  return sstream.str();
}