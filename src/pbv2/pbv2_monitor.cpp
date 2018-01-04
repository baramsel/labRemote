#include <memory>
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
  //dc.turnOn();

  log(logINFO) << " ... AMAC:";
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
  amac.write(AMACreg::LV_ENABLE, 0x0);
  log(logINFO) << "  ++Disable HV";
  amac.write(AMACreg::HV_ENABLE, 0x0);

  //dc.setCurrent(1);


  log(logINFO) << "  ++ Const ADC values:";
  unsigned ota_l=0, ota_r=0, bgo=0, dvdd2=0;
  amac.read(AMACreg::VALUE_LEFT_CH5, ota_l);
  std::cout << "OTA_LEFT : \t" << ota_l << std::endl;
  amac.read(AMACreg::VALUE_RIGHT_CH5, ota_r);
  std::cout << "OTA_RIGHT : \t" << ota_r << std::endl;
  amac.read(AMACreg::VALUE_LEFT_CH1, dvdd2);
  std::cout << "DVDD/2 : \t" << dvdd2 << std::endl;
  amac.read(AMACreg::VALUE_LEFT_CH2, bgo);
  std::cout << "BGO : \t" << bgo << std::endl;


  //
  // Run monitoring
  std::string logpath = "log/" + TestName + "_monitor.log";
  std::fstream logfile(logpath, std::fstream::out);
  logfile << "time bgo rtch3gain ltch3gain rtrg ltrg rtog ltog hvfreq ch0la ch0lb ch1l ch2l ch3l ch4l ch5l ch6l ch0ra ch0rb ch1r ch2r ch3r ch4r ch5r ch6r" << std::endl;

  log(logINFO) << "Start monitoring...";

  unsigned ch0la,ch0lb,ch1l,ch2l,ch3l,ch4l,ch5l,ch6l;
  unsigned ch0ra,ch0rb,ch1r,ch2r,ch3r,ch4r,ch5r,ch6r;
  unsigned ctl_bgo, ctl_rtch3gain, ctl_ltch3gain, ctl_rtrg, ctl_ltrg, ctl_rtog, ctl_ltog, ctl_hvfreq;
  for(uint i=0;i<100000;i++)
    {
      auto time = std::chrono::system_clock::now();

      logfile << std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();

      // Check settings
      ctl_bgo=ctl_rtch3gain=ctl_ltch3gain=ctl_rtrg=ctl_ltrg=ctl_rtog=ctl_ltog=ctl_hvfreq=0;
      amac.read(AMACreg::BANDGAP_CONTROL, ctl_bgo);
      amac.read(AMACreg::RT_CH3_GAIN_SEL, ctl_rtch3gain);
      amac.read(AMACreg::LT_CH3_GAIN_SEL, ctl_ltch3gain);
      amac.read(AMACreg::LEFT_RAMP_GAIN,  ctl_rtrg);
      amac.read(AMACreg::RIGHT_RAMP_GAIN, ctl_ltrg);
      amac.read(AMACreg::OPAMP_GAIN_RIGHT,ctl_rtog);
      amac.read(AMACreg::OPAMP_GAIN_LEFT, ctl_ltog);
      amac.read(AMACreg::HV_FREQ,         ctl_hvfreq);

      logfile << " " << ctl_bgo << " " << ctl_rtch3gain << " " << ctl_ltch3gain << " " << ctl_rtrg << " " << ctl_ltrg << " " << ctl_rtog << " " << ctl_ltog << " " << ctl_hvfreq;

      // check ADCs
      ch0la=ch0lb=ch1l=ch2l=ch3l=ch4l=ch5l=ch6l=0;
      ch0ra=ch0rb=ch1r=ch2r=ch3r=ch4r=ch5r=ch6r=0;

      amac.write(AMACreg::LT_CH0_SEL, 0);
      amac.read(AMACreg::VALUE_LEFT_CH0, ch0la);
      amac.write(AMACreg::LT_CH0_SEL, 1);
      amac.read(AMACreg::VALUE_LEFT_CH0, ch0lb);
      amac.read(AMACreg::VALUE_LEFT_CH1, ch1l);
      amac.read(AMACreg::VALUE_LEFT_CH2, ch2l);
      amac.read(AMACreg::VALUE_LEFT_CH3, ch3l);
      amac.read(AMACreg::VALUE_LEFT_CH4, ch4l);
      amac.read(AMACreg::VALUE_LEFT_CH5, ch5l);
      amac.read(AMACreg::VALUE_LEFT_CH6, ch6l);

      amac.write(AMACreg::RT_CH0_SEL, 0);
      amac.read(AMACreg::VALUE_RIGHT_CH0, ch0ra);
      amac.write(AMACreg::RT_CH0_SEL, 1);
      amac.read(AMACreg::VALUE_RIGHT_CH0, ch0rb);
      amac.read(AMACreg::VALUE_RIGHT_CH1, ch1r);
      amac.read(AMACreg::VALUE_RIGHT_CH2, ch2r);
      amac.read(AMACreg::VALUE_RIGHT_CH3, ch3r);
      amac.read(AMACreg::VALUE_RIGHT_CH4, ch4r);
      amac.read(AMACreg::VALUE_RIGHT_CH5, ch5r);
      amac.read(AMACreg::VALUE_RIGHT_CH6, ch6r);

      logfile << " " << ch0la << " " << ch0lb << " " << ch1l << " " << ch2l << " " << ch3l << " " << ch4l << " " << ch5l << " " << ch6l
	      << " " << ch0ra << " " << ch0rb << " " << ch1r << " " << ch2r << " " << ch3r << " " << ch4r << " " << ch5r << " " << ch6r
	      << std::endl;

      std::this_thread::sleep_for(std::chrono::seconds(1));
    }


  //
  // Poweroff
  sm.turnOff();
  ps.turnOff();
  dc.turnOff();

  return 0;
}
