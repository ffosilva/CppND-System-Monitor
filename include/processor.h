#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
  long lastTotalJiffies_;
  long lastIdleJiffies_;
};

#endif