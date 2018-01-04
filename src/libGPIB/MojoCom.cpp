#include "MojoCom.h"

MojoCom::MojoCom(std::string dev) {
  m_com = new SerialCom(dev, B115200);
  // Reset

  char obuf[] = "\r\n";
  char ibuf[2];
  for (unsigned n=0; n<10; n++) {
    m_com->write(obuf, 2);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    for (unsigned i=0; i<2; i+=m_com->read(&ibuf[i], 2-i))
      std::this_thread::sleep_for(std::chrono::milliseconds(1));

    if (ibuf[1] != '\n') {
      log(logERROR) << __PRETTY_FUNCTION__ << " : Reset failed!";
    } else {
      break;
    }
  }
}

MojoCom::~MojoCom() {
    delete m_com;
}

int MojoCom::writeReg(unsigned reg, char val) {
  log(logDEBUG1) << __PRETTY_FUNCTION__ << " : reg(" << reg << ") val(" << val << ")";
  char outbuf[7];

  // Prepare buffer
  outbuf[0] = 0x80 | ((reg >> 8) & 0xFF);
  outbuf[1] = reg & 0xFF;
  outbuf[2] = (val >> 24 ) & 0xFF;
  outbuf[3] = (val >> 16 ) & 0xFF;
  outbuf[4] = (val >> 8 ) & 0xFF;
  outbuf[5] = val & 0xFF;
  outbuf[6] = '\n';

  // Write buffer
  if (m_com->write(outbuf, 7) < 7) {
    log(logERROR) << __PRETTY_FUNCTION__ << " : Failed writing all bytes!";
    return -1;
  }

  return 0;
}

int MojoCom::readReg(unsigned reg, char &val) {
  log(logDEBUG1) << __PRETTY_FUNCTION__ << " : reg(" << reg << ")";
  char outbuf[3], inbuf[6];

  // Prepare output
  outbuf[0] = 0x00 | ((reg >> 8) & 0xFF);
  outbuf[1] = reg & 0xFF;
  outbuf[2] = '\n';

  // Write output
  if (m_com->write(outbuf, 3) < 3) 
    {
      log(logERROR) << __PRETTY_FUNCTION__ << " : Failed writing all bytes!";
      return -1;
    }

  // Read data
  for (unsigned i=0; i<6; i+=m_com->read(&inbuf[i], 6-i))
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

  // Check status
  if (inbuf[5] != '\n')
    {
      log(logERROR) << __PRETTY_FUNCTION__ << " : Wrong read response!";
      return -1;
    }

  // Trasnfer data
  for(int i = 1; i < 5; i++) val = (val<<8) + (unsigned char)inbuf[i];
  log(logDEBUG1) << __PRETTY_FUNCTION__ << " : val(" << val << ")";
  return 0;
}

int MojoCom::enableI2C() {
    log(logINFO) << __PRETTY_FUNCTION__ << " : Enabling I2C!";
    // Reset I2C
    if (this->writeReg(0x4c, 0x00))  return -1;
    if (this->writeReg(0x4c, 0x01)) return -1; // unreset I2C + drive SCL active

    // Set the I2C clock prescale
    if (this->writeReg(0x100, 0x00)) return -1;
    if (this->writeReg(0x101, 0x01)) return -1;

    // Enable the I2C core
    if (this->writeReg(0x102, 0x80)) return -1;
    return 0;
}

int MojoCom::readI2C(unsigned id, unsigned addr, char *data, unsigned bytes) {
    unsigned i(0);
    char reg(0);

    // Write Slave Address(RW = W) + Start Condition
    if(writeReg(0x103, id<<1)) return -1;
    if(writeReg(0x104,0x90)) return -1;
    do{
      usleep(1000);
      if(readReg(0x104, reg)) return -1;
    }while(reg & 0x02);
    if(reg != 0x41)
      return -1;

    // Write Register Address
    if(writeReg(0x103, addr)) return -1;
    if(writeReg(0x104,0x10)) return -1;
    do{
      usleep(1000);
      if(readReg(0x104, reg)) return -1;
    }while(reg & 0x02);
    if(reg != 0x41)
      return -1;

    // Write Slave Address(RW = R) + Repeated Start Condition
    if(writeReg(0x103, id*2+1)) return -1;
    if(writeReg(0x104,0x90)) return -1;
    do{
      usleep(1000);
      if(readReg(0x104, reg)) return -1;
    }while(reg & 0x02);
    if(reg != 0x41)
      return -1;

    // Read Bytes
    for(i = 0; i < (bytes - 1); i++){;
      if(writeReg(0x104,0x20)) return -1;
      do{
	usleep(1000);
	if(readReg(0x104, reg)) return -1;
      }while(reg & 0x02);
      if(reg != 0x41)
	return -1;
      if(readReg(0x103, data[i])) return -1;
    }

    // Read last byte + Stop Condition & NACK
    if(writeReg(0x104,0x68)) return -1;
    do{
      usleep(1000);
      if(readReg(0x104, reg)) return -1;
    }while(reg & 0x02);
    if(reg != 0x81)
      return -1;
    if(readReg(0x103, data[i])) return -1;

    return 0;
}

int MojoCom::writeI2C(unsigned id, unsigned addr, char *data, unsigned bytes) {
  unsigned i(0);
  char reg(0);

  // Write id + Start Condition
  if(writeReg(0x103, id<<1)) return -1;
  if(writeReg(0x104,0x90)) return -1;
  do{
    usleep(1000);
    if(readReg(0x104, reg)) return -1;
  }while(reg & 0x02);
  if(reg != 0x41)
    return -1;

  // Write reg addr
  if(writeReg(0x103, addr)) return -1;
  if(writeReg(0x104,0x10)) return -1;
  do{
    usleep(1000);
    if(readReg(0x104, reg)) return -1;
  }while(reg & 0x02);
  if(reg != 0x41)
    return -1;

  // Write Bytes
  for(i = 0; i < (bytes-1); i++){
    if(writeReg(0x103, data[i])) return -1;
    if(writeReg(0x104,0x10)) return -1;
    do{
      usleep(1000);
      if(readReg(0x104, reg)) return -1;
    }while(reg & 0x02);
    if(reg != 0x41)
      return -1;
  }
	
  // Write Byte + Stop Condition
  if(writeReg(0x103, data[i])) return -1;
  if(writeReg(0x104,0x50)) return -1;
  do{
    usleep(1000);
    if(readReg(0x104, reg)) return -1;
  }while(reg & 0x02);
  if(reg != 0x01)
    return -1;

  return 0;
}
