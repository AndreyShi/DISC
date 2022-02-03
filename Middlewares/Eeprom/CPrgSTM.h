#include "../../Core/code/programm.h"


#ifndef CPRGSTM
#define CPRGSTM
class CPrgSTM :public CPrg{
private:
	const uint32_t start_eeprom = 0x81FF000;
public:
	unsigned char read_one_byte_eeprom_address_int(unsigned int addr,bool *error);
};
extern CPrgSTM flash_prg;

#endif
