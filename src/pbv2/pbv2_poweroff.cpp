#include <iostream>
#include <cmath>

#ifdef FTDI
#include "FTDICom.h"
#endif

#include "Logger.h"
#include "MojoCom.h"
#include "I2CCom.h"
#include "AMAC.h"
#include "Bk85xx.h"
#include "AgilentPs.h"
#include "Keithley24XX.h"

loglevel_e loglevel = logINFO;

int main(int argc, char* argv[]) {
  //
  // Get settings from the command line
  if (argc < 4) {
    log(logERROR) << "Not enough parameters!";
    log(logERROR) << "Useage: " << argv[0] << " <Mojo/FTDI> <BK85XX> <GPIB>";
    return -1;
  }

  std::string mojoDev = argv[1];
  std::string bkDev = argv[2];
  std::string gpibDev = argv[3];

#ifndef FTDI
  if(mojoDev=="FTDI")
    {
      log(logERROR) << "FTDI support not enabled.";
      return -1;
    }
#endif

  log(logINFO) << "Initialising ...";

  log(logINFO) << " ... Agilent PS:";
  AgilentPs ps(gpibDev, 10);
  ps.init();
  ps.setRange(20);
  ps.setVoltage(11.0);
  ps.setCurrent(2.00);
  ps.turnOn();

  log(logINFO) << " ... Keithley 2410:";
  Keithley24XX sm(gpibDev, 9);
  sm.init();
  sm.setSource(KeithleyMode::CURRENT, 1e-6, 1e-6);
  sm.setSense(KeithleyMode::VOLTAGE, 500, 500);

  log(logINFO) << " ... DC Load:";
  Bk85xx dc(bkDev);
  dc.setRemote();
  dc.setRemoteSense();
  dc.setModeCC();
  dc.setCurrent(0);
  dc.turnOn();

  std::unique_ptr<I2CCom> i2c;
#ifdef FTDI
  if(mojoDev=="FTDI")
    i2c.reset(new FTDICom());
  else
    i2c.reset(new MojoCom(mojoDev));
#else
  i2c.reset(new MojoCom(mojoDev));
#endif
  AMAC amac(0, i2c);

  // Turn off
  sm.turnOff();
  ps.turnOff();
  dc.turnOff();

  return 0;
}
