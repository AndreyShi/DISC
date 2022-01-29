п»ї/*
 * enum_struct.h
 *
 * Created: 14.06.2017 16:47:01
 *  Author: РђРЅРґСЂРµР№
 */ 


#ifndef ENUM_STRUCT_H_
#define ENUM_STRUCT_H_

enum  CMD_STATE{
	//	FILL    = (char)0,
	//    BEGIN   = (char)1,
	//    END     = (char)2,
	//	EMPTY   = (char)0xff
	FILL    = (unsigned char)0,
	BEGIN   = (unsigned char)1,
	END     = (unsigned char)2,
	EMPTY   = (unsigned char)0xff
};

enum FIND_RES{
	NOT_FOUND_NEXT_AXIS = 0,
	FOUND_NEXT_AXIS     = (1 << 0),
	FOUND_G90           = (1 << 1),
	FOUND_G91           = (1 << 2),
	FOUND_CUR_AXIS      = (1 << 3),
};


enum CMD_WORD{
	AXIS_X,
	AXIS_Y,
	AXIS_Z,
	AXIS_U,
	AXIS_V,
	AXIS_W,
	AXIS_P,
	AXIS_Q,
	AXIS_R,
	AXIS_A,
	AXIS_B,
	AXIS_C,
	AXIS_D,
	//	AXIS_E,

	OFFSET,
	SET_ABS,
	SET_REL,
	INSTR,

	JUMP,
	FUNC_CALL,
	BEGIN_CYCLE,
	END_CYCLE,
	M_FUNC,                   // for enum size = 16bit

	PAUSE,

	ACCEL_FEED,               // G0
	NORMAL_FEED,              // G1

	DIS_LENGTH_INSTR,         //G49
	ENA_LENGTH_INSTR_MINUS,   //G44
	ENA_LENGTH_INSTR_PLUS,    //G43
	ENA_INSTR_LEFT,           //G42
	ENA_INSTR_RIGHT,          //G41
	DIS_INSTR,                //G40
	PLANE_XY,				  // XY
	PLANE_XZ,				  // XZ
	PLANE_YZ,				  // YZ
	ACCEL_NORMAL_INPUT_SEL,    // Select Accel/Normal Feed from input
	SET_OFFSET,
	SET_OFFSET_X,
	SET_OFFSET_Y,
	SET_OFFSET_Z,
	SET_OFFSET_U,
	SET_OFFSET_V,
	SET_OFFSET_W,
	SET_OFFSET_P,
	SET_OFFSET_Q,
	SET_OFFSET_R,
	SET_OFFSET_A,
	SET_OFFSET_B,
	SET_OFFSET_C,
	SET_OFFSET_D,
	SET_FREQ,
	FIRST_SPINDEL_STEP,
	TWO_SPINDEL_STEP,
	//3 AND 4 GEAR WERE ADDED 03.02.2011
	TH_GEAR,
	FOUR_GEAR,
	AXIS_S,
	//try to implement g92x, and so on
	//21.06.2011
	SET_ZERO_X,
	SET_ZERO_Y,
	SET_ZERO_Z,
	SET_ZERO_U,
	SET_ZERO_V,
	SET_ZERO_W,
	SET_ZERO_P,
	SET_ZERO_Q,
	SET_ZERO_R,
	SET_ZERO_A,
	SET_ZERO_B,
	SET_ZERO_C,
	SET_ZERO_D,
	ENA_RAD_INSTR_MINUS_ARSENAL,
	ENA_RAD_INSTR_PLUS_ARSENAL,


	//SET_OFFSET_E,
	EMPTY_CMD = (char) 0xff
};

enum EXT_CMD_STATE{
	COMMENT,
	NO_COMMENT = (char)0xff
};

enum SEL_FUN_PRG{
	PROGRAMM=(char)0x00,
	FUNCTION=(char)0x01,
	SEARCH=(unsigned char)0x02,
	NO_SEARCH=(unsigned char)0x03,
};
enum MODE_WRITE{
	REPLACE=(char)0x00,
	INSERT=(char)0x01
};


typedef struct CMD_FORMAT{
	//01.06.2016 must be 33 bytes *8=264 bytes
	//it is buffer's size inside at45DB081B
	//8 frames totally for one buffer(one page)
	SEL_FUN_PRG type;
	unsigned int prog_number;
	unsigned int frame_number;
	CMD_STATE state_cmd;          // empty, full, comment...
	EXT_CMD_STATE ext_state_cmd;  // comment...
	CMD_WORD cmd;                 // cmd unsigned int
	long freq;
	long position;
	CMD_WORD cmd1;                 // cmd unsigned int
	long freq1;
	long position1;
	//13.10.2016 if chip is new  empty0 is empty (0xff)
	// if frame will be written empty0 will be full (0x00)
	// and ones will be counted and written to crc_one and crc_two
	// ones are counted only in first 26 bytes.
	// it makes when low level programm write(frame...) is working
	// later,when frame will be read,if empty0 is full (0x00)
	// ones will be checked... See file programm.cpp
	char empty0;
	//------------------------
	char empty1;
	char empty2;
	char empty3;
	char empty4;
	char empty5;
	char crc_one;
	char crc_two;
} FLASH_CMD;







#endif /* ENUM_STRUCT_H_ */