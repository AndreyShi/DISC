#include "CPrgSTM.h"

unsigned char CPrgSTM::read_one_byte_eeprom_address_int(unsigned int addr,bool *error){
	uint8_t tmp;
	tmp = *((uint8_t*)(start_eeprom + addr));
	return tmp;
}
