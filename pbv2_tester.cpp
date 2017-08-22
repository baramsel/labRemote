#include <iostream>
#include <cmath>

#include "Logger.h"
#include "MojoCom.h"
#include "I2CCom.h"
#include "AMAC.h"
#include "Bk85xx.h"
#include "AgilentPs.h"
#include "Keithley24XX.h"

loglevel_e loglevel = logINFO;

int main(int argc, char* argv[]) {

    if (argc < 4) {
        log(logERROR) << "Not enough parameters!";
        log(logERROR) << "Useage: " << argv[0] << " <Mojo> <BK85XX> <GPIB>";
        return -1;
    }

    std::string mojoDev = argv[1];
    std::string bkDev = argv[2];
    std::string gpibDev = argv[3];

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
    sm.setSense(KeithleyMode::VOLTAGE, 100, 100);

    log(logINFO) << " ... DC Load:";
    Bk85xx dc(bkDev);
    dc.setRemote();
    dc.setRemoteSense();
    dc.setModeCC();
    dc.setCurrent(0);
    dc.turnOn();
  
    log(logINFO) << " ... AMAC:";
    MojoCom mojo(mojoDev);
    AMAC amac(0, dynamic_cast<I2CCom*>(&mojo));
    
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

    log(logINFO) << "Test LV enable: ";
    dc.setCurrent(0);
    dc.turnOn();
    double lv_on = dc.getValues().vol;
    amac.write(AMACreg::LV_ENABLE, 0x0);
    double lv_off = dc.getValues().vol;
    amac.write(AMACreg::LV_ENABLE, 0x1);
    if (!(lv_on > 1.4 && lv_on < 1.6 && lv_off < 0.1)) {
        log(logERROR) << " ++ LV enable not working!";
        return -1;
    } else {
        log(logINFO) << " ++ LV enable good!";
    }

    log(logINFO) << "Test HV enable: ";
    sm.setSource(KeithleyMode::CURRENT, 1e-6, 1e-6);
    sm.setSense(KeithleyMode::VOLTAGE, 100, 100);
    sm.turnOn();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    double hv_off = std::stod(sm.sense().substr(0,13));
    amac.write(AMACreg::HV_ENABLE, 0x1);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    double hv_on = std::stod(sm.sense().substr(0,13));
    // If HVmux does not conduct, source meter will go into compliance
    if (!(hv_off > 90.0 && hv_on < 1.0)) {
        log(logERROR) << " ++ HV enable not working!";
        return -1;
    } else {
        log(logINFO) << " ++ HV enable good!";
    }
    sm.turnOff();

    log(logINFO) << "Testing Current Sense Amp & Efficiency...";
    double iout_min = 0;
    double iout_max = 4000;
    double iout_step = 100;

    for (double iout = iout_min; iout <= iout_max; iout+=iout_step) {
        dc.setCurrent(iout);
        std::this_thread::sleep_for(std::chrono::seconds(10));
        unsigned cur = 0;
        amac.read(AMACreg::VALUE_RIGHT_CH1, cur);
        unsigned ptat = 0;
        amac.read(AMACreg::VALUE_RIGHT_CH3, ptat);
        unsigned ntc = 0;
        amac.read(AMACreg::VALUE_LEFT_CH2, ntc);
        std::cout << iout << "\t" << dc.getValues().vol << "\t" << cur << "\t" << ps.getCurrent() << "\t" << ntc << "\t" << ptat << std::endl;
    }
    dc.setCurrent(1000);

    log(logINFO) << "Testing VIN measurement ...";
    double vin_min = 6.0;
    double vin_max = 12.0;
    double vin_step = 0.1;

    for (double vin=vin_min; vin<=vin_max; vin+=vin_step) {
        ps.setVoltage(vin);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        unsigned val = 0;
        amac.read(AMACreg::VALUE_RIGHT_CH0, val);
        std::cout << vin << "\t" << val << std::endl;
    }
    ps.setVoltage(11.0);
    ps.setCurrent(2.00);


    log(logINFO) << "Testing Ileak measurement ...";
    double ileak_min = 1e-6;
    double ileak_max = 5e-3;
    double ileak_step = 1e-6;
    sm.setSource(KeithleyMode::CURRENT, ileak_min, ileak_min);
    sm.turnOn();

    int counter = 0;
    for (double ileak=ileak_min; ileak<=ileak_max; ileak += ileak_step) {
        counter++;
        if (counter%10 == 0)
            ileak_step = ileak_step*10;
        sm.setSource(KeithleyMode::CURRENT, ileak, ileak);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        unsigned val[5];
        for (unsigned i=0; i<5; i++) {
            if (i == 0) {
                amac.write(AMACreg::OPAMP_GAIN_LEFT, 0);
            } else {
                amac.write(AMACreg::OPAMP_GAIN_LEFT, pow(2,i-1));
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            amac.read(AMACreg::VALUE_LEFT_CH6, *&val[i]);
        }
        std::cout << ileak << "\t" << val[0] << "\t" << val[1] << "\t" << val[2] << "\t" << val[3] << "\t" << val[4] << std::endl;
    }
    sm.turnOff();
    ps.turnOff();
    dc.turnOff();
    
    /*
    log(logINFO) << "  ++ADC Values";
    unsigned val = 0;
    amac.read(AMACreg::VALUE_RIGHT_CH0, val);
    log(logINFO) << "[VIN] : \t" << val ;
    amac.read(AMACreg::VALUE_RIGHT_CH1, val);
    log(logINFO) << "[IOUT] : \t" << val ;
    amac.read(AMACreg::VALUE_RIGHT_CH2, val);
    log(logINFO) << "[NTC_Temp] : \t" << val ;
    amac.read(AMACreg::VALUE_RIGHT_CH3, val);
    log(logINFO) << "[3R] : \t\t" << val ;
    amac.read(AMACreg::VALUE_RIGHT_CH4, val);
    log(logINFO) << "[VDD_H/4] : \t" << val ;
    amac.read(AMACreg::VALUE_RIGHT_CH5, val);
    log(logINFO) << "[OTA] : \t" << val ;
    amac.read(AMACreg::VALUE_RIGHT_CH6, val);
    log(logINFO) << "[ICHANR] : \t" << val ;
    
    amac.read(AMACreg::VALUE_LEFT_CH0, val);
    log(logINFO) << "[0L] : \t\t" << val ;
    amac.read(AMACreg::VALUE_LEFT_CH1, val);
    log(logINFO) << "[DVDD/2] : \t" << val ;
    amac.read(AMACreg::VALUE_LEFT_CH2, val);
    log(logINFO) << "[BGO] : \t" << val ;
    amac.read(AMACreg::VALUE_LEFT_CH3, val);
    log(logINFO) << "[3L] : \t\t" << val ;
    amac.read(AMACreg::VALUE_LEFT_CH4, val);
    log(logINFO) << "[TEMP] : \t" << val ;
    amac.read(AMACreg::VALUE_LEFT_CH5, val);
    log(logINFO) << "[OTA] : \t" << val ;
    amac.read(AMACreg::VALUE_LEFT_CH6, val);
    log(logINFO) << "[ICHANL] : \t" << val ;

    */

    return 0;
}
