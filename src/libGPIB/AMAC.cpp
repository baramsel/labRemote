#include "AMAC.h"

AMAC::AMAC(unsigned id, std::unique_ptr<I2CCom>& i2c)
  : m_i2c(std::move(i2c))
{
    m_id = id;
    m_i2c->enableI2C();
}

AMAC::~AMAC() {

}

//Reads the according bits from the according AMAC register
int AMAC::read(AMACreg reg, unsigned &val){
	switch(reg){
		case AMACreg::STATUS_HV_ILOCK: 	return this->readBits(0,1,0, val);
		case AMACreg::STATUS_LV_ILOCK: 	return this->readBits(0,1,1, val);
		case AMACreg::STATUS_HV_WARN: 	return this->readBits(0,1,2, val);
		case AMACreg::STATUS_LV_WARN: 	return this->readBits(0,1,3, val);
		case AMACreg::STATUS_ID: 	return this->readBits(0,4,4, val);
		case AMACreg::VALUE_LEFT_CH0: 	return this->readBits(20,10,0, val);
		case AMACreg::VALUE_LEFT_CH1: 	return this->readBits(21,10,2, val);
		case AMACreg::VALUE_LEFT_CH2: 	return this->readBits(22,10,4, val);
		case AMACreg::VALUE_LEFT_CH3: 	return this->readBits(23,10,6, val);
		case AMACreg::VALUE_LEFT_CH4: 	return this->readBits(25,10,0, val);
		case AMACreg::VALUE_LEFT_CH5: 	return this->readBits(26,10,2, val);
		case AMACreg::VALUE_LEFT_CH6: 	return this->readBits(27,10,4, val);
		case AMACreg::VALUE_RIGHT_CH0: 	return this->readBits(29,10,0, val);
		case AMACreg::VALUE_RIGHT_CH1: 	return this->readBits(30,10,2, val);
		case AMACreg::VALUE_RIGHT_CH2: 	return this->readBits(31,10,4, val);
		case AMACreg::VALUE_RIGHT_CH3: 	return this->readBits(32,10,6, val);
		case AMACreg::VALUE_RIGHT_CH4: 	return this->readBits(34,10,0, val);
		case AMACreg::VALUE_RIGHT_CH5: 	return this->readBits(35,10,2, val);
		case AMACreg::VALUE_RIGHT_CH6: 	return this->readBits(36,10,4, val);
		case AMACreg::LEFT_RAMP_GAIN: 	return this->readBits(45,2,0, val);
		case AMACreg::RIGHT_RAMP_GAIN: 	return this->readBits(45,2,4, val);
		case AMACreg::OPAMP_GAIN_RIGHT:	return this->readBits(42,4,4, val);
		case AMACreg::OPAMP_GAIN_LEFT:	return this->readBits(42,4,0, val);
		case AMACreg::BANDGAP_CONTROL: 	return this->readBits(40,5,0, val);
		case AMACreg::RT_CH3_GAIN_SEL: 	return this->readBits(45,1,7, val);
		case AMACreg::LT_CH3_GAIN_SEL: 	return this->readBits(45,1,3, val);
		case AMACreg::HV_ENABLE:	return this->readBits(43,1,0, val);	
		case AMACreg::LV_ENABLE:	return this->readBits(44,1,0, val);
		case AMACreg::HV_FREQ:		return this->readBits(43,2,1, val);
		case AMACreg::ILOCK_FLAG_HV_LEFT_HI:	return this->readBits(1,7,0,val);
		case AMACreg::ILOCK_FLAG_HV_LEFT_LO:	return this->readBits(2,7,0,val);
		case AMACreg::ILOCK_FLAG_HV_RIGHT_HI:	return this->readBits(3,7,0,val);
		case AMACreg::ILOCK_FLAG_HV_RIGHT_LO:	return this->readBits(4,7,0,val);
		case AMACreg::ILOCK_FLAG_LV_LEFT_HI:	return this->readBits(5,7,0,val);
		case AMACreg::ILOCK_FLAG_LV_LEFT_LO:	return this->readBits(6,7,0,val);
		case AMACreg::ILOCK_FLAG_LV_RIGHT_HI:	return this->readBits(7,7,0,val);
		case AMACreg::ILOCK_FLAG_LV_RIGHT_LO:	return this->readBits(8,7,0,val);
		case AMACreg::WARN_FLAG_HV_LEFT_HI:	return this->readBits(9,7,0,val);
		case AMACreg::WARN_FLAG_HV_LEFT_LO:	return this->readBits(10,7,0,val);
		case AMACreg::WARN_FLAG_HV_RIGHT_HI:	return this->readBits(11,7,0,val);
		case AMACreg::WARN_FLAG_HV_RIGHT_LO:	return this->readBits(12,7,0,val);
		case AMACreg::WARN_FLAG_LV_LEFT_HI:	return this->readBits(13,7,0,val);
		case AMACreg::WARN_FLAG_LV_LEFT_LO:	return this->readBits(14,7,0,val);
		case AMACreg::WARN_FLAG_LV_RIGHT_HI:	return this->readBits(15,7,0,val);
		case AMACreg::WARN_FLAG_LV_RIGHT_LO:	return this->readBits(16,7,0,val);
		case AMACreg::ILOCK_EN_HV_LEFT_HI:	return this->readBits(50,7,0,val);
		case AMACreg::ILOCK_EN_HV_LEFT_LO:	return this->readBits(51,7,0,val);
		case AMACreg::ILOCK_EN_HV_RIGHT_HI:	return this->readBits(52,7,0,val);
		case AMACreg::ILOCK_EN_HV_RIGHT_LO:	return this->readBits(53,7,0,val);
		case AMACreg::ILOCK_EN_LV_LEFT_HI:	return this->readBits(54,7,0,val);
		case AMACreg::ILOCK_EN_LV_LEFT_LO:	return this->readBits(55,7,0,val);
		case AMACreg::ILOCK_EN_LV_RIGHT_HI:	return this->readBits(56,7,0,val);
		case AMACreg::ILOCK_EN_LV_RIGHT_LO:	return this->readBits(57,7,0,val);
		case AMACreg::WARN_EN_HV_LEFT_HI:	return this->readBits(58,7,0,val);
		case AMACreg::WARN_EN_HV_LEFT_LO:	return this->readBits(59,7,0,val);
		case AMACreg::WARN_EN_HV_RIGHT_HI:	return this->readBits(60,7,0,val);
		case AMACreg::WARN_EN_HV_RIGHT_LO:	return this->readBits(61,7,0,val);
		case AMACreg::WARN_EN_LV_LEFT_HI:	return this->readBits(62,7,0,val);
		case AMACreg::WARN_EN_LV_LEFT_LO:	return this->readBits(63,7,0,val);
		case AMACreg::WARN_EN_LV_RIGHT_HI:	return this->readBits(64,7,0,val);
		case AMACreg::WARN_EN_LV_RIGHT_LO:	return this->readBits(65,7,0,val);
		case AMACreg::ILOCK_HV_THRESH_HI_L_CH0:	return this->readBits(66,10,0,val);
		default:
			return -2; break;
	}
}

//Writes the according bits in the according AMAC register
int AMAC::write(AMACreg reg, unsigned val){
	switch(reg){
		case AMACreg::LEFT_RAMP_GAIN: 		return this->writeBits(45,2,0,val);	
		case AMACreg::RIGHT_RAMP_GAIN: 		return this->writeBits(45,2,4,val);
		case AMACreg::OPAMP_GAIN_RIGHT:		return this->writeBits(42,4,4,val);
		case AMACreg::OPAMP_GAIN_LEFT:		return this->writeBits(42,4,0,val);
	        case AMACreg::BANDGAP_CONTROL: 		return this->writeBits(40,5,0,val);
		case AMACreg::RT_CH3_GAIN_SEL: 		return this->writeBits(45,1,7,val);
		case AMACreg::LT_CH3_GAIN_SEL: 		return this->writeBits(45,1,3,val);
		case AMACreg::RT_CH0_SEL: 		return this->writeBits(45,1,6,val);
		case AMACreg::LT_CH0_SEL: 		return this->writeBits(45,1,2,val);	
		case AMACreg::CLK_OUT_ENABLE : 		return this->writeBits(41,1,0,val);	
		case AMACreg::CLK_SELECT:		return this->writeBits(49,1,0,val);	
		case AMACreg::HV_FREQ:			return this->writeBits(43,2,1,val);	
		case AMACreg::HV_ENABLE:		return this->writeBits(43,1,0,val);	
		case AMACreg::LV_ENABLE:		return this->writeBits(44,1,0,val);	
		case AMACreg::HV_ILOCK:			return this->writeBits(43,1,7,val);
		case AMACreg::HV_ILOCK_LAM:		return this->writeBits(43,1,5,val);	
		case AMACreg::LV_ILOCK:			return this->writeBits(44,1,7,val);
		case AMACreg::LV_ILOCK_LAM:		return this->writeBits(44,1,5,val);
		case AMACreg::RESET_HV_ILOCK:		return this->writeBits(48,1,0,val);
		case AMACreg::RESET_HV_WARN:		return this->writeBits(48,1,1,val);
		case AMACreg::RESET_LV_ILOCK:		return this->writeBits(48,1,2,val);
		case AMACreg::RESET_LV_WARN:		return this->writeBits(48,1,3,val);
		case AMACreg::ILOCK_EN_HV_LEFT_HI:	return this->writeBits(50,7,0,val);
		case AMACreg::ILOCK_EN_HV_LEFT_LO:	return this->writeBits(51,7,0,val);
		case AMACreg::ILOCK_EN_HV_RIGHT_HI:	return this->writeBits(52,7,0,val);
		case AMACreg::ILOCK_EN_HV_RIGHT_LO:	return this->writeBits(53,7,0,val);
		case AMACreg::ILOCK_EN_LV_LEFT_HI:	return this->writeBits(54,7,0,val);
		case AMACreg::ILOCK_EN_LV_LEFT_LO:	return this->writeBits(55,7,0,val);
		case AMACreg::ILOCK_EN_LV_RIGHT_HI:	return this->writeBits(56,7,0,val);
		case AMACreg::ILOCK_EN_LV_RIGHT_LO:	return this->writeBits(57,7,0,val);
		case AMACreg::WARN_EN_HV_LEFT_HI:	return this->writeBits(58,7,0,val);
		case AMACreg::WARN_EN_HV_LEFT_LO:	return this->writeBits(59,7,0,val);
		case AMACreg::WARN_EN_HV_RIGHT_HI:	return this->writeBits(60,7,0,val);
		case AMACreg::WARN_EN_HV_RIGHT_LO:	return this->writeBits(61,7,0,val);
		case AMACreg::WARN_EN_LV_LEFT_HI:	return this->writeBits(62,7,0,val);
		case AMACreg::WARN_EN_LV_LEFT_LO:	return this->writeBits(63,7,0,val);
		case AMACreg::WARN_EN_LV_RIGHT_HI:	return this->writeBits(64,7,0,val);
		case AMACreg::WARN_EN_LV_RIGHT_LO:	return this->writeBits(65,7,0,val);
		case AMACreg::ILOCK_HV_THRESH_HI_L_CH0:	return this->writeBits(66,10,0,val);
		case AMACreg::ILOCK_HV_THRESH_HI_L_CH1:	return this->writeBits(67,10,2,val);
		case AMACreg::ILOCK_HV_THRESH_HI_L_CH2:	return this->writeBits(68,10,4,val);
		case AMACreg::ILOCK_HV_THRESH_HI_L_CH3:	return this->writeBits(69,10,6,val);
		case AMACreg::ILOCK_HV_THRESH_HI_L_CH4:	return this->writeBits(71,10,0,val);
		case AMACreg::ILOCK_HV_THRESH_HI_L_CH5:	return this->writeBits(72,10,2,val);
		case AMACreg::ILOCK_HV_THRESH_HI_L_CH6:	return this->writeBits(73,10,4,val);
		case AMACreg::ILOCK_HV_THRESH_LO_L_CH0:	return this->writeBits(75,10,0,val);
		case AMACreg::ILOCK_HV_THRESH_LO_L_CH1:	return this->writeBits(76,10,2,val);
		case AMACreg::ILOCK_HV_THRESH_LO_L_CH2:	return this->writeBits(77,10,4,val);
		case AMACreg::ILOCK_HV_THRESH_LO_L_CH3:	return this->writeBits(78,10,6,val);
		case AMACreg::ILOCK_HV_THRESH_LO_L_CH4:	return this->writeBits(80,10,0,val);
		case AMACreg::ILOCK_HV_THRESH_LO_L_CH5:	return this->writeBits(81,10,2,val);
		case AMACreg::ILOCK_HV_THRESH_LO_L_CH6:	return this->writeBits(82,10,4,val);
		case AMACreg::ILOCK_HV_THRESH_HI_R_CH0:	return this->writeBits(84,10,0,val);
		case AMACreg::ILOCK_HV_THRESH_HI_R_CH1:	return this->writeBits(85,10,2,val);
		case AMACreg::ILOCK_HV_THRESH_HI_R_CH2:	return this->writeBits(86,10,4,val);
		case AMACreg::ILOCK_HV_THRESH_HI_R_CH3:	return this->writeBits(87,10,6,val);
		case AMACreg::ILOCK_HV_THRESH_HI_R_CH4:	return this->writeBits(89,10,0,val);
		case AMACreg::ILOCK_HV_THRESH_HI_R_CH5:	return this->writeBits(90,10,2,val);
		case AMACreg::ILOCK_HV_THRESH_HI_R_CH6:	return this->writeBits(91,10,4,val);
		case AMACreg::ILOCK_HV_THRESH_LO_R_CH0:	return this->writeBits(93,10,0,val);
		case AMACreg::ILOCK_HV_THRESH_LO_R_CH1:	return this->writeBits(94,10,2,val);
		case AMACreg::ILOCK_HV_THRESH_LO_R_CH2:	return this->writeBits(95,10,4,val);
		case AMACreg::ILOCK_HV_THRESH_LO_R_CH3:	return this->writeBits(96,10,6,val);
		case AMACreg::ILOCK_HV_THRESH_LO_R_CH4:	return this->writeBits(98,10,0,val);
		case AMACreg::ILOCK_HV_THRESH_LO_R_CH5:	return this->writeBits(99,10,2,val);
		case AMACreg::ILOCK_HV_THRESH_LO_R_CH6:	return this->writeBits(100,10,4,val);
		case AMACreg::ILOCK_LV_THRESH_HI_L_CH0:	return this->writeBits(102,10,0,val);
		case AMACreg::ILOCK_LV_THRESH_HI_L_CH1:	return this->writeBits(103,10,2,val);
		case AMACreg::ILOCK_LV_THRESH_HI_L_CH2:	return this->writeBits(104,10,4,val);
		case AMACreg::ILOCK_LV_THRESH_HI_L_CH3:	return this->writeBits(105,10,6,val);
		case AMACreg::ILOCK_LV_THRESH_HI_L_CH4:	return this->writeBits(107,10,0,val);
		case AMACreg::ILOCK_LV_THRESH_HI_L_CH5:	return this->writeBits(108,10,2,val);
		case AMACreg::ILOCK_LV_THRESH_HI_L_CH6:	return this->writeBits(109,10,4,val);
		case AMACreg::ILOCK_LV_THRESH_LO_L_CH0:	return this->writeBits(111,10,0,val);
		case AMACreg::ILOCK_LV_THRESH_LO_L_CH1:	return this->writeBits(112,10,2,val);
		case AMACreg::ILOCK_LV_THRESH_LO_L_CH2:	return this->writeBits(113,10,4,val);
		case AMACreg::ILOCK_LV_THRESH_LO_L_CH3:	return this->writeBits(114,10,6,val);
		case AMACreg::ILOCK_LV_THRESH_LO_L_CH4:	return this->writeBits(116,10,0,val);
		case AMACreg::ILOCK_LV_THRESH_LO_L_CH5:	return this->writeBits(117,10,2,val);
		case AMACreg::ILOCK_LV_THRESH_LO_L_CH6:	return this->writeBits(118,10,4,val);
		case AMACreg::ILOCK_LV_THRESH_HI_R_CH0:	return this->writeBits(120,10,0,val);
		case AMACreg::ILOCK_LV_THRESH_HI_R_CH1:	return this->writeBits(121,10,2,val);
		case AMACreg::ILOCK_LV_THRESH_HI_R_CH2:	return this->writeBits(122,10,4,val);
		case AMACreg::ILOCK_LV_THRESH_HI_R_CH3:	return this->writeBits(123,10,6,val);
		case AMACreg::ILOCK_LV_THRESH_HI_R_CH4:	return this->writeBits(125,10,0,val);
		case AMACreg::ILOCK_LV_THRESH_HI_R_CH5:	return this->writeBits(126,10,2,val);
		case AMACreg::ILOCK_LV_THRESH_HI_R_CH6:	return this->writeBits(127,10,4,val);
		case AMACreg::ILOCK_LV_THRESH_LO_R_CH0:	return this->writeBits(129,10,0,val);
		case AMACreg::ILOCK_LV_THRESH_LO_R_CH1:	return this->writeBits(130,10,2,val);
		case AMACreg::ILOCK_LV_THRESH_LO_R_CH2:	return this->writeBits(131,10,4,val);
		case AMACreg::ILOCK_LV_THRESH_LO_R_CH3:	return this->writeBits(132,10,6,val);
		case AMACreg::ILOCK_LV_THRESH_LO_R_CH4:	return this->writeBits(134,10,0,val);
		case AMACreg::ILOCK_LV_THRESH_LO_R_CH5:	return this->writeBits(135,10,2,val);
		case AMACreg::ILOCK_LV_THRESH_LO_R_CH6:	return this->writeBits(136,10,4,val);
		case AMACreg::WARN_HV_THRESH_HI_L_CH0:	return this->writeBits(138,10,0,val);
		case AMACreg::WARN_HV_THRESH_HI_L_CH1:	return this->writeBits(139,10,2,val);
		case AMACreg::WARN_HV_THRESH_HI_L_CH2:	return this->writeBits(140,10,4,val);
		case AMACreg::WARN_HV_THRESH_HI_L_CH3:	return this->writeBits(141,10,6,val);
		case AMACreg::WARN_HV_THRESH_HI_L_CH4:	return this->writeBits(143,10,0,val);
		case AMACreg::WARN_HV_THRESH_HI_L_CH5:	return this->writeBits(144,10,2,val);
		case AMACreg::WARN_HV_THRESH_HI_L_CH6:	return this->writeBits(145,10,4,val);
		case AMACreg::WARN_HV_THRESH_LO_L_CH0:	return this->writeBits(147,10,0,val);
		case AMACreg::WARN_HV_THRESH_LO_L_CH1:	return this->writeBits(148,10,2,val);
		case AMACreg::WARN_HV_THRESH_LO_L_CH2:	return this->writeBits(149,10,4,val);
		case AMACreg::WARN_HV_THRESH_LO_L_CH3:	return this->writeBits(150,10,6,val);
		case AMACreg::WARN_HV_THRESH_LO_L_CH4:	return this->writeBits(152,10,0,val);
		case AMACreg::WARN_HV_THRESH_LO_L_CH5:	return this->writeBits(153,10,2,val);
		case AMACreg::WARN_HV_THRESH_LO_L_CH6:	return this->writeBits(154,10,4,val);
		case AMACreg::WARN_HV_THRESH_HI_R_CH0:	return this->writeBits(156,10,0,val);
		case AMACreg::WARN_HV_THRESH_HI_R_CH1:	return this->writeBits(157,10,2,val);
		case AMACreg::WARN_HV_THRESH_HI_R_CH2:	return this->writeBits(158,10,4,val);
		case AMACreg::WARN_HV_THRESH_HI_R_CH3:	return this->writeBits(159,10,6,val);
		case AMACreg::WARN_HV_THRESH_HI_R_CH4:	return this->writeBits(161,10,0,val);
		case AMACreg::WARN_HV_THRESH_HI_R_CH5:	return this->writeBits(162,10,2,val);
		case AMACreg::WARN_HV_THRESH_HI_R_CH6:	return this->writeBits(163,10,4,val);
		case AMACreg::WARN_HV_THRESH_LO_R_CH0:	return this->writeBits(165,10,0,val);
		case AMACreg::WARN_HV_THRESH_LO_R_CH1:	return this->writeBits(166,10,2,val);
		case AMACreg::WARN_HV_THRESH_LO_R_CH2:	return this->writeBits(167,10,4,val);
		case AMACreg::WARN_HV_THRESH_LO_R_CH3:	return this->writeBits(168,10,6,val);
		case AMACreg::WARN_HV_THRESH_LO_R_CH4:	return this->writeBits(170,10,0,val);
		case AMACreg::WARN_HV_THRESH_LO_R_CH5:	return this->writeBits(171,10,2,val);
		case AMACreg::WARN_HV_THRESH_LO_R_CH6:	return this->writeBits(172,10,4,val);
		case AMACreg::WARN_LV_THRESH_HI_L_CH0:	return this->writeBits(174,10,0,val);
		case AMACreg::WARN_LV_THRESH_HI_L_CH1:	return this->writeBits(175,10,2,val);
		case AMACreg::WARN_LV_THRESH_HI_L_CH2:	return this->writeBits(176,10,4,val);
		case AMACreg::WARN_LV_THRESH_HI_L_CH3:	return this->writeBits(177,10,6,val);
		case AMACreg::WARN_LV_THRESH_HI_L_CH4:	return this->writeBits(179,10,0,val);
		case AMACreg::WARN_LV_THRESH_HI_L_CH5:	return this->writeBits(180,10,2,val);
		case AMACreg::WARN_LV_THRESH_HI_L_CH6:	return this->writeBits(181,10,4,val);
		case AMACreg::WARN_LV_THRESH_LO_L_CH0:	return this->writeBits(183,10,0,val);
		case AMACreg::WARN_LV_THRESH_LO_L_CH1:	return this->writeBits(184,10,2,val);
		case AMACreg::WARN_LV_THRESH_LO_L_CH2:	return this->writeBits(185,10,4,val);
		case AMACreg::WARN_LV_THRESH_LO_L_CH3:	return this->writeBits(186,10,6,val);
		case AMACreg::WARN_LV_THRESH_LO_L_CH4:	return this->writeBits(188,10,0,val);
		case AMACreg::WARN_LV_THRESH_LO_L_CH5:	return this->writeBits(189,10,2,val);
		case AMACreg::WARN_LV_THRESH_LO_L_CH6:	return this->writeBits(190,10,4,val);
		case AMACreg::WARN_LV_THRESH_HI_R_CH0:	return this->writeBits(192,10,0,val);
		case AMACreg::WARN_LV_THRESH_HI_R_CH1:	return this->writeBits(193,10,2,val);
		case AMACreg::WARN_LV_THRESH_HI_R_CH2:	return this->writeBits(194,10,4,val);
		case AMACreg::WARN_LV_THRESH_HI_R_CH3:	return this->writeBits(195,10,6,val);
		case AMACreg::WARN_LV_THRESH_HI_R_CH4:	return this->writeBits(197,10,0,val);
		case AMACreg::WARN_LV_THRESH_HI_R_CH5:	return this->writeBits(198,10,2,val);
		case AMACreg::WARN_LV_THRESH_HI_R_CH6:	return this->writeBits(199,10,4,val);
		case AMACreg::WARN_LV_THRESH_LO_R_CH0:	return this->writeBits(201,10,0,val);
		case AMACreg::WARN_LV_THRESH_LO_R_CH1:	return this->writeBits(202,10,2,val);
		case AMACreg::WARN_LV_THRESH_LO_R_CH2:	return this->writeBits(203,10,4,val);
		case AMACreg::WARN_LV_THRESH_LO_R_CH3:	return this->writeBits(204,10,6,val);
		case AMACreg::WARN_LV_THRESH_LO_R_CH4:	return this->writeBits(206,10,0,val);
		case AMACreg::WARN_LV_THRESH_LO_R_CH5:	return this->writeBits(207,10,2,val);
		case AMACreg::WARN_LV_THRESH_LO_R_CH6:	return this->writeBits(208,10,4,val);
		default:
			return -2; break;
	}
}


//Works until up to 4 bytes
int AMAC::readBits(unsigned startreg, unsigned num_bits, unsigned offset, unsigned &value){
	int num_bytes = ((num_bits + offset -1) / 8) + 1;
	char* buf = new char[num_bytes];

	if(m_i2c->readI2C(m_id, startreg, buf, num_bytes)) return -1;	
	uint temp = 0;
	for(int i = num_bytes - 1; i >= 0; i--){
	  //std::cout << "a " << i << ": " << std::hex << +buf[i] << std::dec << std::endl;
		temp <<= 8;
		temp += (buf[i] & 0xFF);
	}
	//std::cout << "temp " << std::hex << temp << std::dec << std::endl;
	value = (temp >> offset) & ((1 << num_bits) -1);
	return 0;
}

//Works until up to 4 bytes
//Read register, modify bits, write register
int AMAC::writeBits(unsigned startreg, unsigned num_bits, unsigned offset, unsigned value){
	int num_bytes = ((num_bits + offset -1) / 8) + 1;
	char* buf = new char[num_bytes];
	int temp;
	int mask = ((1 << num_bits) -1) << offset;
	if(m_i2c->readI2C(m_id, startreg, buf, num_bytes)) return -1;	
	temp = 0;
	for(int i = num_bytes - 1; i >= 0; i--){
		temp <<= 8;
		temp += (buf[i] & 0xFF);
	}
	temp &= ~mask;
	temp |= ((value << offset) & mask);
	for(int i = 0; i < num_bytes; i++, temp >>= 8)
		buf[i] = temp & 0xFF;
	if(m_i2c->writeI2C(m_id, startreg, buf, num_bytes)) return -1;
	return 0;
}
