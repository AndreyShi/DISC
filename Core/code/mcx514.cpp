/*
 * mcx514.cpp
 *
 * Created: 24.06.2019 13:22:48
 *  Author: Andrew
 */ 
#include <avr/io.h>
#define F_CPU 32000000UL
#include <util/delay.h>
#include "mcx514.h"

//////////////////////////////////////////////////////////////////////////////////
// Common functions
//////////////////////////////////////////////////////////////////////////////////
// Common function of writing WR register (I/O port access. The following is the example of SH microcomputer.)
int WriteReg(char HAdr, char LAdr, unsigned short Data) {
          //reg_writeh(HAdr, Data);
	  //reg_writel(LAdr, Data);
          
          PORTD.OUTSET=HAdr; //WR0H address
          PORTE.OUT=Data>>8; //WR0H data (axis X)
          PORTR.OUTCLR=0x02; //WR=0 write enable		  
          _delay_loop_1(5);//__delay_cycles(5);
          PORTR.OUTSET=0x02; //WR=1 write disable
          _delay_loop_1(5);// __delay_cycles(5);
		  PORTD.OUTCLR=HAdr;
          
          PORTD.OUTSET=LAdr; //WR0L address
          PORTE.OUT=Data; //WR0L writing data (Real Position)
          PORTR.OUTCLR=0x02; //WR=0 write enable
         _delay_loop_1(5);// __delay_cycles(5);
          PORTR.OUTSET=0x02; //WR=1 write disable
          _delay_loop_1(5);//__delay_cycles(5);
          PORTD.OUTCLR=LAdr;
        return 0;
}

// Common function of reading RR register (I/O port access. The following is the example of SH microcomputer.)
int ReadReg(char HAdr, char LAdr, unsigned short *Data) {
	//*Data = reg_read(Adr);
	
          PORTD.OUTSET=HAdr;  //Address read
          PORTE.DIR=0x00;  
          PORTR.OUTCLR=0x01;  //RD=0 read enable 
          _delay_loop_1(5);//__delay_cycles(5);
          char HData=PORTE.IN; 
          PORTR.OUTSET=0x01;  //RD=1 read disable
          PORTE.DIR=0xFF;  
          _delay_loop_1(5);//__delay_cycles(5);  
		   PORTD.OUTCLR=HAdr;
          
          PORTD.OUTSET=LAdr;  //Address read
          PORTE.DIR=0x00;  
          PORTR.OUTCLR=0x01;  //RD=0 read enable 
          _delay_loop_1(5);//__delay_cycles(5);
          char LData=PORTE.IN; 
          PORTR.OUTSET=0x01;  //RD=1 read disable
          PORTE.DIR=0xFF;  
          _delay_loop_1(5);//__delay_cycles(5); 
		  PORTD.OUTCLR=LAdr;     
        
           *Data = (HData<<8) | LData;
       
        return 0;
}

// Common function of commands for writing data
// Data can be written by writing data into WR6, WR7, and then writing a command into WR0.
int SetData(unsigned short Cmd, int Axis, unsigned long Data) {

	unsigned long mask_data = 0x0000ffff;
	unsigned short write_data;

	// Writes the lower 16-bit of data into WR6
	write_data = (unsigned short )(Data & mask_data);
	WriteReg6(write_data);
	
	// Writes the upper 16-bit of data into WR7
	write_data = (unsigned short )(Data >> 16);
	WriteReg7(write_data);

	// Writes a command (into WR0)
	WriteReg0(((unsigned short) (Axis << 8) | Cmd));

	
	return 0;
}

// Common function of commands for writing mode
// Data can be written by writing data into WR6, and then writing a command into WR0.
int SetModeData(unsigned short Cmd, int Axis, unsigned short Data) {

	// Writes the lower 16-bit of data into WR6
	WriteReg6(Data);
	
	// Writes a command (into WR0)
	WriteReg0(((unsigned short) (Axis << 8) | Cmd));
	
	return 0;
}

// Common function of commands for reading data
// Data can be read by writing a command into WR0, and then read  RR6, RR7.
int GetData(unsigned short Cmd, int Axis, long *Data) {

	unsigned short rdata1,rdata2;
	long retdata = 0x00000000;
	
	if (Data == NULL) return 0;
	
	// Writes a command (into WR0)
	WriteReg0(((unsigned short) (Axis << 8) | Cmd));
	
	// Reads RR7
	ReadReg7(&rdata1);
        
	// Reads RR6
	ReadReg6(&rdata2);
	
	// Create data for reading
	retdata = (long )rdata1;	// Sets RR7 value to the upper 16-bit
	*Data = (retdata << 16);
	retdata = (long )rdata2;	// Sets RR6 value to the lower 16-bit
	*Data = *Data | retdata;

	return 0;
}

// Common function of command execution
int ExeCmd(unsigned short Cmd, int Axis) {

	// Writes a command (into WR0)
	WriteReg0(((unsigned short) (Axis << 8) | Cmd));
	
	return 0;
}
 
// Waiting for termination of driving
void waitdrive(int Axis) {

	unsigned short rrData;

	ReadReg0(&rrData);			// Reads RR0
	while ((rrData & Axis)) {		// If during the driving
		ReadReg0(&rrData);		// Reads RR0
	}
}
// Waiting for termination of driving in RTC interrupt
char waitdrive_rtc(int Axis) {

	unsigned short rrData;
	/*static char trigger;*/
	ReadReg0(&rrData);			// Reads RR0
	if((rrData & Axis))
		return rrData;		
	else
		return 0;
}	

// Waiting for termination of split pulse
void waitsplit(int Axis) {

	unsigned short rrData;
	
	ReadReg3(1, Axis, &rrData);		// Reads RR3 Page1
	while ((rrData & 0x0800)) {		// If split pulse is in operation
		ReadReg3(1, Axis, &rrData);	// Reads RR3 Page1
	}
}	


///////////////////////////////////////////////////////////////////////////////////////
// Write functions for WR register 
//////////////////////////////////////////////////////////////////////////////////////
int WriteReg0(unsigned short Data){ 				// Writes into WR0 register
	return ( WriteReg((REG_ADDR + MCX514_WR0H), (REG_ADDR + MCX514_WR0L), Data) );
}

int WriteReg1(int Axis, unsigned short Data){ 			// Writes into WR1 register
	WriteReg0(((Axis << 8) + MCX514_CMD1F_NOP));		// Axis assignment
	return(WriteReg((REG_ADDR + MCX514_WR1H), (REG_ADDR + MCX514_WR1L), Data));
}

int WriteReg2(int Axis, unsigned short Data){ 			// Writes into WR2 register
	WriteReg0(((Axis << 8) + MCX514_CMD1F_NOP));		// Axis assignment
	return(WriteReg((REG_ADDR + MCX514_WR2H), (REG_ADDR + MCX514_WR2L), Data));
}
int WriteReg3(int Axis, unsigned short Data){ 			// Writes into WR3 register
	WriteReg0(((Axis << 8) + MCX514_CMD1F_NOP));		// Axis assignment
	return(WriteReg((REG_ADDR + MCX514_WR3H), (REG_ADDR + MCX514_WR3L), Data));
}
int WriteReg4(unsigned short Data){ 				// Writes into WR4 register
	return(WriteReg((REG_ADDR + MCX514_WR4H), (REG_ADDR + MCX514_WR4L), Data));
}
int WriteReg5(unsigned short Data){ 				// Writes into WR5 register
        return(WriteReg((REG_ADDR + MCX514_WR5H), (REG_ADDR + MCX514_WR5L), Data));
}
int WriteReg6(unsigned short Data){ 				// Writes into WR6 register
	return(WriteReg((REG_ADDR + MCX514_WR6H), (REG_ADDR + MCX514_WR6L), Data));
}
int WriteReg7(unsigned short Data){ 				// Writes into WR7 register
	return(WriteReg((REG_ADDR + MCX514_WR7H), (REG_ADDR + MCX514_WR7L), Data));
}

///////////////////////////////////////////////////////////////////////////////////////
// Read functions for RR register
///////////////////////////////////////////////////////////////////////////////////////
int ReadReg0(unsigned short *Data){ 				// Reads out RR0 register
	return(ReadReg((REG_ADDR + MCX514_RR0H), (REG_ADDR + MCX514_RR0L), Data));
}
int ReadReg1(int Axis, unsigned short *Data){ 			// Reads out RR1 register
	WriteReg0(((Axis << 8) + MCX514_CMD1F_NOP));		// Axis assignment
	return(ReadReg((REG_ADDR + MCX514_RR1H), (REG_ADDR + MCX514_RR1L), Data));
}
int ReadReg2(int Axis, unsigned short *Data){ 			// Reads out RR2 register
	WriteReg0(((Axis << 8) + MCX514_CMD1F_NOP));		// Axis assignment
	return(ReadReg((REG_ADDR + MCX514_RR2H), (REG_ADDR + MCX514_RR2L), Data));
}
int ReadReg3(int Page, int Axis, unsigned short *Data){ 		// Reads out RR3 register
	if (Page == 0) {					// Specifies Page0
		WriteReg0(((unsigned short)(Axis << 8) | MCX514_CMD7A_RR3P0));
	}
	else {						// Specifies Page1
		WriteReg0(((unsigned short)(Axis << 8) | MCX514_CMD7B_RR3P1));
	}
	WriteReg0(((unsigned short)(Axis << 8) | MCX514_CMD1F_NOP));	// Axis assignment
	return(ReadReg((REG_ADDR + MCX514_RR3H), (REG_ADDR + MCX514_RR3L), Data));
}
int ReadReg4(unsigned short *Data){ 				// Reads out RR4 register
	return(ReadReg((REG_ADDR + MCX514_RR4H), (REG_ADDR + MCX514_RR4L), Data));
}
int ReadReg5(unsigned short *Data){ 				// Reads out RR5 register
	return(ReadReg((REG_ADDR + MCX514_RR5H), (REG_ADDR + MCX514_RR5L), Data));
}
int ReadReg6(unsigned short *Data){ 				// Reads out RR6 register
	return(ReadReg((REG_ADDR + MCX514_RR6H), (REG_ADDR + MCX514_RR6L), Data));
}
int ReadReg7(unsigned short *Data){ 				// Reads out RR7 register 
	return(ReadReg((REG_ADDR + MCX514_RR7H), (REG_ADDR + MCX514_RR7L), Data));
}


///////////////////////////////////////////////////////////////////////////////////////
// Functions of commands for writing data
///////////////////////////////////////////////////////////////////////////////////////
int SetStartSpd(int Axis, long Data){ 				// Initial speed setting
	return(SetData(MCX514_CMD04_SV, Axis, Data));
}
int SetSpeed(int Axis, long Data){ 				// Drive speed setting
	return(SetData(MCX514_CMD05_DV, Axis, Data));
}
int SetJerk(int Axis, long Data){ 				// Jerk setting
	return(SetData(MCX514_CMD00_JK, Axis, Data));
}


int SetDJerk(int Axis, long Data){ 				// Deceleration increasing rate setting
	return(SetData(MCX514_CMD01_DJ, Axis, Data));
}
int SetAcc(int Axis, long Data){ 				// Acceleration setting
	return(SetData(MCX514_CMD02_AC, Axis, Data));
}
int SetDec(int Axis, long Data){ 				// Deceleration setting
	return(SetData(MCX514_CMD03_DC, Axis, Data));
}
int SetPulse(int Axis, long Data){ 				// Drive pulse number / Finish point setting
	return(SetData(MCX514_CMD06_TP, Axis, Data));
}

int SetDecP(int Axis, long Data){ 				// Manual deceleration point setting
	return(SetData(MCX514_CMD07_DP, Axis, Data));
}
int SetCCP(int Axis, long Data){ 				// Manual circular center point setting
	return(SetData(MCX514_CMD08_CP, Axis, Data));
}
int SetLp(int Axis, long Data){ 				// Logical position counter setting
	return(SetData(MCX514_CMD09_LP, Axis, (unsigned long)Data));
}
int SetRp(int Axis, long Data){ 				// Real position counter setting
	return(SetData(MCX514_CMD0A_RP, Axis, (unsigned long)Data));
}
int SetCompP(int Axis, long Data){ 				// Software limit + setting
	return(SetData(MCX514_CMD0B_SP, Axis, (unsigned long)Data));
}
int SetCompM(int Axis, long Data){ 				// Software limit ? setting
	return(SetData(MCX514_CMD0C_SM, Axis, (unsigned long)Data));
}
int SetAccOfst(int Axis, long Data){ 				// Acceleration counter offsetting
	return(SetData(MCX514_CMD0D_AO, Axis, Data));
}
int SetHomeSpd(int Axis, long Data){ 				// Home search speed setting
	return(SetData(MCX514_CMD14_HV, Axis, Data));
}
int SetLpMax(int Axis, long Data){ 				// Logical position counter maximum value setting
	return(SetData(MCX514_CMD0E_LX, Axis, Data));
}
int SetRpMax(int Axis, long Data){ 				// Real position counter maximum value setting
	return(SetData(MCX514_CMD0F_RX, Axis, Data));
}
int SetMR0(int Axis, long Data){ 				// Multi-purpose register 0 setting
	return(SetData(MCX514_CMD10_MR0, Axis, Data));
}
int SetMR1(int Axis, long Data){ 				// Multi-purpose register 1 setting
	return(SetData(MCX514_CMD11_MR1, Axis, Data));
}
int SetMR2(int Axis, long Data){ 				// Multi-purpose register 2 setting
	return(SetData(MCX514_CMD12_MR2, Axis, Data));
}
int SetMR3(int Axis, long Data){ 				// Multi-purpose register 3 setting
	return(SetData(MCX514_CMD13_MR3, Axis, Data));
}
int SetSpeedInc(int Axis, long Data){ 				// Speed increasing / decreasing value setting
	return(SetData(MCX514_CMD15_IV, Axis, Data));
}
int SetTimer(int Axis, long Data){ 				// Timer value setting
	return(SetData(MCX514_CMD16_TM, Axis, Data));
}
int SetSplit1(int Axis, unsigned short Data1, unsigned short Data2) {	// Split pulse setting 1
	unsigned long Data;

	Data = (((unsigned long)Data1 << 16) |  (unsigned long)Data2);
	return(SetData(MCX514_CMD17_SP1, Axis, Data));
}
int SetSplit2(int Axis, unsigned long Data){			// Split pulse setting 2
	return(SetData(MCX514_CMD18_SP2, Axis, Data));
}
int SetTPMax(long Data){					// Interpolation / Finish point maximum value setting
	return(SetData(MCX514_CMD39_TX, MCX514_AXIS_NONE, Data));
}
int SetHLNumber(unsigned short Data){				// Helical rotation number setting
	return(SetData(MCX514_CMD3A_CHLN, MCX514_AXIS_NONE, (long )Data));
}
int SetHLValue(long Data){					// Helical calculation value  setting
	return(SetData(MCX514_CMD3B_HLV, MCX514_AXIS_NONE, Data));
}


///////////////////////////////////////////////////////////////////////////////////////
// Functions of commands for writing mode
///////////////////////////////////////////////////////////////////////////////////////
int SetModeMRm(int Axis, unsigned short Data){ 			// Multi-purpose register mode setting
	return(SetModeData(MCX514_CMD20_MRM, Axis, Data));
}
int SetModePIO1(int Axis, unsigned short Data){ 			// PIO signal setting 1
	return(SetModeData(MCX514_CMD21_P1M, Axis, Data));
}
int SetModePIO2(int Axis, unsigned short Data){ 			// PIO signal setting 2?Other settings
	return(SetModeData(MCX514_CMD22_P2M, Axis, Data));
}
int SetModeHMSrch1(int Axis, unsigned short Data){ 		        // Automatic home search mode setting 1
	return(SetModeData(MCX514_CMD23_H1M, Axis, Data));
}
int SetModeHMSrch2(int Axis, unsigned short Data){ 		        // Automatic home search mode setting 2
	return(SetModeData(MCX514_CMD24_H2M, Axis, Data));
}
int SetModeFilter(int Axis, unsigned short Data){ 			// Input signal filter mode setting
	return(SetModeData(MCX514_CMD25_FLM, Axis, Data));
}
int SetModeSync0(int Axis, unsigned short Data){ 			// Synchronous action SYNC0 setting
	return(SetModeData(MCX514_CMD26_S0M, Axis, Data));
}
int SetModeSync1(int Axis, unsigned short Data){ 			// Synchronous action SYNC1 setting
	return(SetModeData(MCX514_CMD27_S1M, Axis, Data));
}
int SetModeSync2(int Axis, unsigned short Data){ 			// Synchronous action SYNC2 setting
	return(SetModeData(MCX514_CMD28_S2M, Axis, Data));
}
int SetModeSync3(int Axis, unsigned short Data){ 			// Synchronous action SYNC3 setting
	return(SetModeData(MCX514_CMD29_S3M, Axis, Data));
}
int SetModeIPM(unsigned short Data){ 				// Interpolation mode setting
	return(SetModeData(MCX514_CMD2A_IPM, MCX514_AXIS_NONE, Data));
}

///////////////////////////////////////////////////////////////////////////////////////
// Functions of commands for reading data
///////////////////////////////////////////////////////////////////////////////////////
int GetLp(int Axis, long *Data){				// Logical position counter reading
	return(GetData(MCX514_CMD30_LP, Axis, (long*) Data));
}
int GetRp(int Axis, long *Data){			// Real position counter reading
	return(GetData(MCX514_CMD31_RP, Axis, (long*) Data));
}
int GetCV(int Axis, long *Data){ 			// Current drive speed reading
	return(GetData(MCX514_CMD32_CV, Axis, Data));
}
int GetCA(int Axis, long *Data){			// Current acceleration/deceleration reading
	return(GetData(MCX514_CMD33_CA, Axis, Data));
}
int GetCT(int Axis, long *Data){			// Current timer value reading
	return(GetData(MCX514_CMD38_CT, Axis, Data));
}
int GetMR0(int Axis, long *Data){			// Multi-purpose register 0 reading
	return(GetData(MCX514_CMD34_MR0, Axis, Data));
}
int GetMR1(int Axis, long *Data){			// Multi-purpose register 1 reading
	return(GetData(MCX514_CMD35_MR1, Axis, Data));
}

int GetMR2(int Axis, long *Data){			// Multi-purpose register 2 reading
	return(GetData(MCX514_CMD36_MR2, Axis, Data));
}
int GetMR3(int Axis, long *Data){			// Multi-purpose register 3 reading
	return(GetData(MCX514_CMD37_MR3, Axis, Data));
}
int GetTX(long *Data){				// Interpolation/Finish point maximum value reading
	return(GetData(MCX514_CMD39_TX, MCX514_AXIS_NONE, Data));
}
int GetCHLN(long *Data){				// Current helical rotation number reading
	return(GetData(MCX514_CMD3A_CHLN, MCX514_AXIS_NONE, Data));
}
int GetHLV(long *Data){				// Helical calculation value reading
	return(GetData(MCX514_CMD3B_HLV, MCX514_AXIS_NONE, Data));
}

int GetWR1(int Axis, long *Data){			// WR1 setting value reading
	return(GetData(MCX514_CMD3D_WR1, Axis, Data));
}
int GetWR2(int Axis, long *Data){			// WR2 setting value reading
	return(GetData(MCX514_CMD3E_WR2, Axis, Data));
}
int GetWR3(int Axis, long *Data){			// WR3 setting value reading
	return(GetData(MCX514_CMD3F_WR3, Axis, Data));
}
int GetMRM(int Axis, long *Data){			// Multi-purpose register mode setting reading
	return(GetData(MCX514_CMD40_MRM, Axis, Data));
}
/*
int GetP1M(int Axis, long *Data){			// PIO signal setting 1 reading
	return(GetDataIO(MCX514_CMD41_P1M, Axis, Data));
}
int GetP2M(int Axis, long *Data){			// PIO signal setting 2?Other settings reading
	return(GetDataIO(MCX514_CMD42_P2M, Axis, Data));
}
*/
int GetAc(int Axis, long *Data ){			// Acceleration setting value reading
	return(GetData(MCX514_CMD43_AC, Axis, Data));
}
int GetStartSpd(int Axis, long *Data ){			// Initial speed setting value reading
	return(GetData(MCX514_CMD44_SV, Axis, Data));
}
int GetSpeed(int Axis, long *Data ){			// Drive speed setting value reading
	return(GetData(MCX514_CMD45_DV, Axis, Data));
}
int GetPulse(int Axis, long *Data ){			// Drive pulse number/Finish point setting value reading
	return(GetData(MCX514_CMD46_TP, Axis, Data));
}
int GetSplit(int Axis,long *Data ){			// Split pulse setting 1 reading
	return(GetData(MCX514_CMD47_SP1, Axis, Data));
}
int GetUI(long *Data ){ 				// General purpose input value reading
	return(GetData(MCX514_CMD48_UI, MCX514_AXIS_NONE, Data));
}

///////////////////////////////////////////////////////////////////////////////////////
// Driving command functions
///////////////////////////////////////////////////////////////////////////////////////
int ExeDRVRL(int Axis){ 					// Relative position driving
	return (ExeCmd(MCX514_CMD50_DRVRL, Axis));
}
int ExeDRVNR(int Axis){ 					// Counter relative position driving
	return (ExeCmd(MCX514_CMD51_DRVNR, Axis));
}
int ExeDRVVP(int Axis){ 					// + Direction continuous pulse driving
	return (ExeCmd(MCX514_CMD52_DRVVP, Axis));
}
int ExeDRVVM(int Axis){ 					// - Direction continuous pulse driving
	return (ExeCmd(MCX514_CMD53_DRVVM, Axis));
}
int ExeDRVAB(int Axis){ 					// Absolute position driving
	return (ExeCmd(MCX514_CMD54_DRVAB, Axis));
}
int ExeDRVSBRK(int Axis){ 					// Decelerating stop
	return (ExeCmd(MCX514_CMD56_DRVSBRK, Axis));
}
int ExeDRVFBRK(int Axis){ 					// Instant stop
	return (ExeCmd(MCX514_CMD57_DRVFBRK, Axis));
}
int ExeDIRCP(int Axis){ 					// Direction signal + setting
	return (ExeCmd(MCX514_CMD58_DIRCP, Axis));
}
int ExeDIRCM(int Axis){ 					// Direction signal - setting
	return (ExeCmd(MCX514_CMD59_DIRCM, Axis));
}
int ExeHMSRC(int Axis){ 					// Automatic home search execution
	return (ExeCmd(MCX514_CMD5A_HMSRC, Axis));
}

///////////////////////////////////////////////////////////////////////////////////////
// Interpolation command functions
///////////////////////////////////////////////////////////////////////////////////////
int ExeLHK1(void ){ 						// 1-axis linear interpolation driving (multichip)
	return (ExeCmd(MCX514_CMD60_LHK1, MCX514_AXIS_NONE));
}
int ExeLHK2(void ){ 						// 2-axis linear interpolation driving
	return (ExeCmd(MCX514_CMD61_LHK2, MCX514_AXIS_NONE));
}
int ExeLHK3(void ){ 						// 3-axis linear interpolation driving
	return (ExeCmd(MCX514_CMD62_LHK3, MCX514_AXIS_NONE));
}
int ExeLHK4(void ){ 						// 4-axis linear interpolation driving
	return (ExeCmd(MCX514_CMD63_LHK4, MCX514_AXIS_NONE));
}
int ExeCHKCW(void ){ 					        // CW circular interpolation driving
	return (ExeCmd(MCX514_CMD64_CHKCW, MCX514_AXIS_NONE));
}
int ExeCHKCCW(void ){ 					        // CCW circular interpolation driving
	return (ExeCmd(MCX514_CMD65_CHKCCW, MCX514_AXIS_NONE));
}
int ExeBHK2(void ){ 						// 2-axis bit pattern interpolation driving
	return (ExeCmd(MCX514_CMD66_BHK2, MCX514_AXIS_NONE));
}
int ExeBHK3(void ){ 						// 3-axis bit pattern interpolation driving
	return (ExeCmd(MCX514_CMD67_BHK3, MCX514_AXIS_NONE));
}
int ExeBHK4(void ){ 						// 4-axis bit pattern interpolation driving
	return (ExeCmd(MCX514_CMD68_BHK4, MCX514_AXIS_NONE));
}
int ExeHLCW(void ){ 						// CW helical interpolation driving
	return (ExeCmd(MCX514_CMD69_HLCW, MCX514_AXIS_NONE));
}
int ExeHLCCW(void ){ 					// CCW helical interpolation driving
	return (ExeCmd(MCX514_CMD6A_HLCCW, MCX514_AXIS_NONE));
}
int ExeHLPCW(void ){ 					// CW helical calculation
	return (ExeCmd(MCX514_CMD6B_HLPCW, MCX514_AXIS_NONE));
}
int ExeHLPCCW(void ){ 					// CCW helical calculation
	return (ExeCmd(MCX514_CMD6C_HLPCCW, MCX514_AXIS_NONE));
}
int ExeDECEN(void ){	 			    // Deceleration enabling
	return (ExeCmd(MCX514_CMD6D_DECEN, MCX514_AXIS_NONE));
}
int ExeDECDIS(void ){	 				// Deceleration disabling
	return (ExeCmd(MCX514_CMD6E_DECDIS, MCX514_AXIS_NONE));
}
int ExeCLRSTEP(void ){	 			// Interpolation interrupt clear / Single-step interpolation
	return (ExeCmd(MCX514_CMD6F_CLRSTEP, MCX514_AXIS_NONE));
}
///////////////////////////////////////////////////////////////////////////////////////
// Synchronous action operation command function
///////////////////////////////////////////////////////////////////////////////////////
int ExeSYNC(int Axis, unsigned short Cmd){ 			// Command related to synchronous action
	return (ExeCmd(Cmd, Axis));
}

///////////////////////////////////////////////////////////////////////////////////////
// Other Commands functions
///////////////////////////////////////////////////////////////////////////////////////
int ExeVINC(int Axis){ 					// Speed increase
	return (ExeCmd(MCX514_CMD70_VINC, Axis));
}
int ExeVDEC(int Axis){ 					// Speed decrease
	return (ExeCmd(MCX514_CMD71_VDEC, Axis));
}
int ExeDCC(int Axis){ 					// Deviation counter clear output
	return (ExeCmd(MCX514_CMD72_DCC, Axis));
}
int ExeTMSTA(int Axis){ 					// Timer-start
	return (ExeCmd(MCX514_CMD73_TMSTA, Axis));
}

int ExeTMSTP(int Axis){ 					// Timer-stop
	return (ExeCmd(MCX514_CMD74_TMSTP, Axis));
}
int ExeSPSTA(int Axis){ 					// Split pulse start
	return (ExeCmd(MCX514_CMD75_SPSTA, Axis));
}
int ExeSPSTP(int Axis){ 					// Split pulse stop
	return (ExeCmd(MCX514_CMD76_SPSTP, Axis));
}
int ExeDHOLD(int Axis){ 					// Drive start holding
	return (ExeCmd(MCX514_CMD77_DHOLD, Axis));
}
int ExeDFREE (int Axis){ 					// Drive start holding release
	return (ExeCmd(MCX514_CMD78_DFREE, Axis));
}
int ExeR2CLR(int Axis){ 					// Error / Finishing status clear
	return (ExeCmd(MCX514_CMD79_R2CLR, Axis));
}
int ExeRR3P0(int Axis){ 					// RR3 Page0 display
	return (ExeCmd(MCX514_CMD7A_RR3P0, Axis));
}
int ExeRR3P1(int Axis){ 					// RR3 Page1 display
	return (ExeCmd(MCX514_CMD7B_RR3P1, Axis));
}
int ExeNOP(int Axis){ 					// NOP
	return (ExeCmd(MCX514_CMD1F_NOP, Axis));
}
int ExeSRST(void ){ 						// Command reset
	return (ExeCmd(MCX514_CMDFF_RST, MCX514_AXIS_NONE));
}