#include "FTDICom.h"

extern "C"{
#include <mpsse.h>
}

#include <string.h>
#include <unistd.h>

FTDICom::FTDICom()
  : m_i2cdev(0)
{  }

FTDICom::~FTDICom()
{ }

int FTDICom::enableI2C()
{ 
  if(m_i2cdev!=0) return 0;
  if((m_i2cdev=MPSSE(I2C, ONE_HUNDRED_KHZ, MSB)) == NULL || !m_i2cdev->open) return 1;
  
  log(logDEBUG2) << __PRETTY_FUNCTION__ << "I2C device "<< GetDescription(m_i2cdev) << " initialized at " << GetClock(m_i2cdev) << "Hz";

  Tristate(m_i2cdev);
      
  return 0;
}

int FTDICom::readI2C(unsigned id, unsigned addr, char *data, unsigned bytes)
{
  char buf[1] = {0};
  char *outbuf=new char[bytes];

  if(Start(m_i2cdev)!=0) return -1;

  buf[0]=id<<1;
  if(Write(m_i2cdev, buf, 1)!=0) return -2;
  buf[0]=addr;
  if(Write(m_i2cdev, buf, 1)!=0) return -3;

  if(GetAck(m_i2cdev)==ACK)
    {
      if(Start(m_i2cdev)!=0) return -5;
      buf[0]=(id<<1)|1;
      if(Write(m_i2cdev, buf, 1)!=0) return -6;

      SendAcks(m_i2cdev);
      outbuf=Read(m_i2cdev, bytes);
      SendNacks(m_i2cdev);
      Read(m_i2cdev, 1); // dummy read to send the NACK
      strncpy(data,outbuf,bytes);
    }
  else return -4;

  if(Stop(m_i2cdev)!=0) return -7;

  return 0;
}

int FTDICom::writeI2C(unsigned id, unsigned addr, char *data, unsigned bytes)
{
  char buf[1];

  if(Start(m_i2cdev)!=0) return -1;

  buf[0]=id<<1;
  if(Write(m_i2cdev, buf, 1)!=0) return -2;
  buf[0]=addr;
  if(Write(m_i2cdev, buf, 1)!=0) return -3;
  if(Write(m_i2cdev, data, bytes)!=0) return -4;

  if(Stop(m_i2cdev)!=0) return -5;

  return 0;
}
