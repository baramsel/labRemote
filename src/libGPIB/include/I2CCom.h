#ifndef I2CCOM_H
#define I2CCOM_H

class I2CCom {
    public:
        virtual int enableI2C() = 0;
        virtual int writeI2C(unsigned id, unsigned addr, char *data, unsigned bytes) = 0;
        virtual int readI2C(unsigned id, unsigned addr, char *data, unsigned bytes) = 0;
};

#endif
