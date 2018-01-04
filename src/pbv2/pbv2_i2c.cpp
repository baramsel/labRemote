#include <iostream>
#include <fstream>
#include <cmath>

#include <sys/stat.h>
#include <dirent.h>

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
  if (argc < 5) {
    log(logERROR) << "Not enough parameters!";
    log(logERROR) << "Usage: " << argv[0] << " TESTNAME <Mojo/FTDI> <BK85XX> <GPIB>";
    return -1;
  }

  std::string TestName = argv[1];
  std::string mojoDev = argv[2];
  std::string bkDev = argv[3];
  std::string gpibDev = argv[4];

#ifndef FTDI
  if(mojoDev=="FTDI")
    {
      log(logERROR) << "FTDI support not enabled.";
      return -1;
    }
#endif

  //
  // Create log directory if it does not exist
  DIR* logdir=opendir("log");
  if(!logdir)
    {
      const int dir_err = mkdir("log", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
      if (-1 == dir_err)
	{
	  std::cerr << "Error creating log directory!" << std::endl;
	  return 1;
	}
    }
  else
    closedir(logdir);

  //
  // Run tests
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

  log(logINFO) << "  ++Init";
  amac.write(AMACreg::BANDGAP_CONTROL, 10); //1.2V LDO output
  amac.write(AMACreg::RT_CH3_GAIN_SEL, 0); // no attenuation
  amac.write(AMACreg::LT_CH3_GAIN_SEL, 0); // no attentuation
  amac.write(AMACreg::RT_CH0_SEL, 1); //a
  amac.write(AMACreg::LT_CH0_SEL, 1); //a 
  amac.write(AMACreg::LEFT_RAMP_GAIN, 3); // best range
  amac.write(AMACreg::RIGHT_RAMP_GAIN, 3); // best range
  amac.write(AMACreg::OPAMP_GAIN_RIGHT, 0); // highest gain
  amac.write(AMACreg::OPAMP_GAIN_LEFT, 0); // highest gain
  amac.write(AMACreg::HV_FREQ, 0x1);

  log(logINFO) << "  ++Enable LV";
  amac.write(AMACreg::LV_ENABLE, 0x1);
  log(logINFO) << "  ++Disable HV";
  amac.write(AMACreg::HV_ENABLE, 0x0);

  dc.setCurrent(3);


  log(logINFO) << "  ++ Const ADC values:";
  unsigned ota_l, ota_r, bgo, dvdd2;
  amac.read(AMACreg::VALUE_LEFT_CH5, ota_l);
  std::cout << "OTA LEFT : \t" << ota_l << std::endl;
  amac.read(AMACreg::VALUE_RIGHT_CH5, ota_r);
  std::cout << "OTA_RIGHT : \t" << ota_r << std::endl;
  amac.read(AMACreg::VALUE_LEFT_CH1, dvdd2);
  std::cout << "DVDD/2 : \t" << dvdd2 << std::endl;
  amac.read(AMACreg::VALUE_LEFT_CH2, bgo);
  std::cout << "BGO : \t" << bgo << std::endl;


  //
  // Tun the reliability test
  log(logINFO) << "Testing I2C...";

  for(int i=0;i<1000;i++)
    {
      double result;
      int error_ctr = 0, num_attempts_per_val=100;
      for(int j = 0; j < num_attempts_per_val; j++)
  	{
  	  unsigned val_to_write = rand()%256;
  	  unsigned val_received = 0;
  	  //Write to any RW register
  	  if(amac.write(AMACreg::ILOCK_HV_THRESH_HI_L_CH0,val_to_write))
  	    {
  	      std::cout << "Write error" << std::endl;
  	      error_ctr++;
  	    }
  	  else
  	    {
  	      if(amac.read(AMACreg::ILOCK_HV_THRESH_HI_L_CH0,val_received))
  		{
  		  std::cout << "Read error" << std::endl;
  		  error_ctr++;
  		}
  	      else if(val_to_write != val_received)
  		{
  		  std::cout << "Data error: 0x" << std::hex << val_to_write << " != 0x" << val_received << std::dec << std::endl;
  		  error_ctr++;
  		}
  	    }
  	}
      result = 1.0 - (double)error_ctr / num_attempts_per_val;
      std::cout << "reliability = " << result << std::endl;
    }

  //
  // Poweroff
  sm.turnOff();
  ps.turnOff();
  dc.turnOff();

  return 0;
}
