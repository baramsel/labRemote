#ifndef FTDICOM_H
#define FTDICOM_H

#include <iostream>
#include <thread>
#include <chrono>
#include <string>

#include "I2CCom.h"
#include "Logger.h"

struct mpsse_context;

class FTDICom : public I2CCom {
public:
  FTDICom();
  ~FTDICom();

  int enableI2C();
  int writeI2C(unsigned id, unsigned addr, char *data, unsigned bytes);
  int readI2C(unsigned id, unsigned addr, char *data, unsigned bytes);

private:
  struct mpsse_context *m_i2cdev;
};

#endif
