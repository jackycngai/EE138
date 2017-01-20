////////////////////////////////////////////////////////////////////////////////////// 
////	Lab 3 - PWM
////		-SAMPLE CODE DOES NOT WORK-
////			- set up appropriate registers
////			- set up TC pointer 
//////////////////////////////////////////////////////////////////////////////////////

#include <asf.h>
//definitions for simplicity
#define Row1 PORT_PA07
#define Row2 PORT_PA06
#define Row3 PORT_PA05
#define Row4 PORT_PA04
#define Col1 PORT_PA19
#define Col2 PORT_PA18
#define Col3 PORT_PA17
#define Col4 PORT_PA16
#define Input_Enable PORT_PINCFG_INEN
#define Drive_Strength PORT_PINCFG_DRVSTR

void simple_clk_init(void);
void enable_ports(void);
void enable_tc_clocks(void);
void enable_tc_8bit(void);
void enable_tc_16bit(void);
void sine_8bit(void);
void sine_16bit(void);
int get_key(void);

int main(void)
{
	simple_clk_init();
	enable_ports();
	enable_tc_clocks();

	// Wait until the synchronization is complete
	for(;;){
		switch(get_key()){
			case 10:
				enable_tc_8bit();
				sine_8bit();
				break;
			case 11:
				enable_tc_16bit();
				sine_16bit();
				break;
			default:
				break;
		}
	}
}
void sine_8bit(void){
	Tc *tc = TC2;
	TcCount8 *tc8 = &(tc->COUNT8);
	int sine[]={128,131,134,137,140,143,146,149,152,155,158,161,164,167,170,173,176,178,181,184,187,189,192,195,197,200,202,205,207,210,212,214,217,219,221,223,225,227,229,231,232,234,236,237,239,240,242,243,244,246,247,248,249,250,251,251,252,253,253,254,254,254,255,255,255,255,255,255,255,254,254,254,253,253,252,251,251,250,249,248,247,246,244,243,242,240,239,237,236,234,232,231,229,227,225,223,221,219,217,214,212,210,207,205,202,200,197,195,192,189,187,184,181,178,176,173,170,167,164,161,158,155,152,149,146,143,140,137,134,131,128,124,121,118,115,112,109,106,103,100,97,94,91,88,85,82,79,77,74,71,68,66,63,60,58,55,53,50,48,45,43,41,38,36,34,32,30,28,26,24,23,21,19,18,16,15,13,12,11,9,8,7,6,5,4,4,3,2,2,1,1,1,0,0,0,0,0,0,0,1,1,1,2,2,3,4,4,5,6,7,8,9,11,12,13,15,16,18,19,21,23,24,26,28,30,32,34,36,38,41,43,45,48,50,53,55,58,60,63,66,68,71,74,77,79,82,85,88,91,94,97,100,103,106,109,112,115,118,121,124};
	for(;;){
		for(int i=0;i<265;i++){
				tc8->CC[1].reg=TC_COUNT8_CC_CC(sine[i]);
		}
	}
}
void sine_16bit(void){
	Tc *tc = TC2;
	TcCount16 *tc16 = &(tc->COUNT16);
	int sine[]={32768,33179,33591,34003,34414,34825,35236,35646,36056,36466,36874,37283,37690,38097,38503,38908,39311,39714,40116,40517,40916,41315,41711,42107,42501,42893,43284,43673,44061,44446,44830,45212,45592,45970,46346,46719,47091,47460,47827,48191,48553,48913,49270,49624,49976,50325,50671,51015,51355,51693,52028,52359,52688,53013,53335,53654,53970,54282,54591,54896,55198,55497,55792,56083,56370,56654,56934,57210,57482,57751,58015,58276,58532,58785,59033,59277,59517,59753,59984,60211,60434,60652,60867,61076,61281,61482,61678,61870,62056,62239,62416,62589,62758,62921,63080,63234,63383,63527,63667,63802,63931,64056,64176,64291,64401,64506,64605,64700,64790,64875,64955,65029,65099,65163,65222,65277,65326,65370,65408,65442,65470,65494,65512,65525,65532,65535,65532,65525,65512,65494,65470,65442,65408,65370,65326,65277,65222,65163,65099,65029,64955,64875,64790,64700,64605,64506,64401,64291,64176,64056,63931,63802,63667,63527,63383,63234,63080,62921,62758,62589,62416,62239,62056,61870,61678,61482,61281,61076,60867,60652,60434,60211,59984,59753,59517,59277,59033,58785,58532,58276,58015,57751,57482,57210,56934,56654,56370,56083,55792,55497,55198,54896,54591,54282,53970,53654,53335,53013,52688,52359,52028,51693,51355,51015,50671,50325,49976,49624,49270,48913,48553,48191,47827,47460,47091,46719,46346,45970,45592,45212,44830,44446,44061,43673,43284,42893,42501,42107,41711,41315,40916,40517,40116,39714,39311,38908,38503,38097,37690,37283,36874,36466,36056,35646,35236,34825,34414,34003,33591,33179,32768,32356,31944,31532,31121,30710,30299,29889,29479,29069,28661,28252,27845,27438,27032,26627,26224,25821,25419,25018,24619,24220,23824,23428,23034,22642,22251,21862,21474,21089,20705,20323,19943,19565,19189,18816,18444,18075,17708,17344,16982,16622,16265,15911,15559,15210,14864,14520,14180,13842,13507,13176,12847,12522,12200,11881,11565,11253,10944,10639,10337,10038,9743,9452,9165,8881,8601,8325,8053,7784,7520,7259,7003,6750,6502,6258,6018,5782,5551,5324,5101,4883,4668,4459,4254,4053,3857,3665,3479,3296,3119,2946,2777,2614,2455,2301,2152,2008,1868,1733,1604,1479,1359,1244,1134,1029,930,835,745,660,580,506,436,372,313,258,209,165,127,93,65,41,23,10,3,0,3,10,23,41,65,93,127,165,209,258,313,372,436,506,580,660,745,835,930,1029,1134,1244,1359,1479,1604,1733,1868,2008,2152,2301,2455,2614,2777,2946,3119,3296,3479,3665,3857,4053,4254,4459,4668,4883,5101,5324,5551,5782,6018,6258,6502,6750,7003,7259,7520,7784,8053,8325,8601,8881,9165,9452,9743,10038,10337,10639,10944,11253,11565,11881,12200,12522,12847,13176,13507,13842,14180,14520,14864,15210,15559,15911,16265,16622,16982,17344,17708,18075,18444,18816,19189,19565,19943,20323,20705,21089,21474,21862,22251,22642,23034,23428,23824,24220,24619,25018,25419,25821,26224,26627,27032,27438,27845,28252,28661,29069,29479,29889,30299,30710,31121,31532,31944,32356};
	for(;;){
		for(int i=0;i<500;i++){
				tc16->CC[1].reg=TC_COUNT16_CC_CC(sine[i]);
		}
	}
	
}
int get_key(void){
	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);
	unsigned int keyPad [4][4] = {
	{1, 2, 3, 10},
	{4, 5, 6, 11},
	{7, 8, 9, 12},
	{14, 0, 15, 13}
	};					//Keypad Matrix, 10=A, 11=B, 12=C, 13=D, 14=*, 15=#
	int keypress=-1; //set to -1 for no input
	//reads all inputs and sets output to corresponding input.
	for(int i=0;i<4;i++){//loop unrolling for optimization
		porA->OUTCLR.reg = (1ul<<(7-i));//Enables the Row to be scanned (PA7-PA4)
		if (porA->IN.reg & (1ul<<(19)))//column scanning (PB19-PB16)
			keypress=keyPad[i][0];//Gets the keypad button that was pressed based off of the 4x4 array
		if (porA->IN.reg & (1ul<<(18)))//column scanning (PB19-PB16)
			keypress=keyPad[i][1];//Gets the keypad button that was pressed based off of the 4x4 array
		if (porA->IN.reg & (1ul<<(17)))//column scanning (PB19-PB16)
			keypress=keyPad[i][2];//Gets the keypad button that was pressed based off of the 4x4 array
		if (porA->IN.reg & (1ul<<(16)))//column scanning (PB19-PB16)
			keypress=keyPad[i][3];//Gets the keypad button that was pressed based off of the 4x4 array
		porA->OUTSET.reg = (1ul<<(7-i));
	}
	return keypress;
}

void enable_ports(){
	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);
	for(int i=16;i<20;i++)
		porA->PINCFG[i].reg=Input_Enable;
	porA->DIRSET.reg = Row1 | Row2 | Row3 | Row4; // Rows of keypad for output
	porA->OUTSET.reg = Row1 | Row2 | Row3 | Row4; // Rows of keypad for output
	porA->DIRCLR.reg = Col1 | Col2 | Col3 | Col4; // Columns of keypad for input
	porA->OUTCLR.reg = Col1 | Col2 | Col3 | Col4; // Columns of keypad for input
	porA->DIRSET.reg = PORT_PA13;
	porA->OUTSET.reg = PORT_PA13;
}
/* Perform Clock configuration to source the TC
1) ENABLE THE APBC CLOCK FOR THE CORREECT MODULE
2) WRITE THE PROPER GENERIC CLOCK SELETION ID*/
void enable_tc_clocks(void)
{
	PM->APBCMASK.reg |= PM_APBCMASK_TC2;  	// PM_APBCMASK_TC2 is in the 19 position
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_TC2_TC3;   		//  Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN;    	// enable it.
}
/* Configure the basic timer/counter for 8 bits*/
void enable_tc_8bit(void)
{

	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);
	porA->PINCFG[13].reg = PORT_PINCFG_PMUXEN ;		// set to correct pin configuration
	porA->PMUX[6].reg = PORT_PMUX_PMUXO_E;
	
	Tc *tc = TC2;
	TcCount8 *tc8 = &(tc->COUNT8);
	/* Set up CTRLA */
	tc8->CTRLA.reg = TC_CTRLA_MODE_COUNT8|TC_CTRLA_PRESCALER_DIV1|TC_CTRLA_PRESCSYNC_PRESC|TC_CTRLA_WAVEGEN_NPWM;//counter mode, prescaler divider, prescaler prescync, waveform
	/*Enable TC*/
	tc8->CTRLA.reg |= TC_CTRLA_ENABLE;
}
/* Configure the basic timer/counter for 8 bits*/
void enable_tc_16bit(void)
{

	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);
	porA->PINCFG[13].reg = PORT_PINCFG_PMUXEN ;		// set to correct pin configuration
	porA->PMUX[6].reg = PORT_PMUX_PMUXO_E;
	
	Tc *tc = TC2;
	TcCount16 *tc16 = &(tc->COUNT16);
	/* Set up CTRLA */
	tc16->CTRLA.reg = TC_CTRLA_MODE_COUNT16|TC_CTRLA_PRESCALER_DIV1|TC_CTRLA_PRESCSYNC_PRESC|TC_CTRLA_WAVEGEN_NPWM;//counter mode, prescaler divider, prescaler prescync, waveform
	/*Enable TC*/
	tc16->CTRLA.reg |= TC_CTRLA_ENABLE;
}
//Simple Clock Initialization
void simple_clk_init(void)
{
	/* Various bits in the INTFLAG register can be set to one at startup.
	   This will ensure that these bits are cleared */
	SYSCTRL->INTFLAG.reg = SYSCTRL_INTFLAG_BOD33RDY | SYSCTRL_INTFLAG_BOD33DET |SYSCTRL_INTFLAG_DFLLRDY;
	system_flash_set_waitstates(0);  		//Clock_flash wait state = 0
	SYSCTRL_OSC8M_Type temp = SYSCTRL->OSC8M;      	/* for OSC8M initialization  */
	temp.bit.PRESC    = 0;    			// no divide, i.e., set clock=8Mhz  (see page 170)
	temp.bit.ONDEMAND = 1;    			//  On-demand is true
	temp.bit.RUNSTDBY = 0;    			//  Standby is false
	SYSCTRL->OSC8M = temp;
	SYSCTRL->OSC8M.reg |= 0x1u << 1;  		// SYSCTRL_OSC8M_ENABLE bit = bit-1 (page 170)
	PM->CPUSEL.reg = (uint32_t)0;    		// CPU and BUS clocks Divide by 1  (see page 110)
	PM->APBASEL.reg = (uint32_t)0;     		// APBA clock 0= Divide by 1  (see page 110)
	PM->APBBSEL.reg = (uint32_t)0;     		// APBB clock 0= Divide by 1  (see page 110)
	PM->APBCSEL.reg = (uint32_t)0;     		// APBB clock 0= Divide by 1  (see page 110)
	PM->APBAMASK.reg |= 01u<<3;   			// Enable Generic clock controller clock (page 127)
	/* Software reset Generic clock to ensure it is re-initialized correctly */
	GCLK->CTRL.reg = 0x1u << 0;   			// Reset gen. clock (see page 94)
	while (GCLK->CTRL.reg & 0x1u ) {  /* Wait for reset to complete */ }
	// Initialization and enable generic clock #0
	*((uint8_t*)&GCLK->GENDIV.reg) = 0;  		// Select GCLK0 (page 104, Table 14-10)
	GCLK->GENDIV.reg  = 0x0100;   		 	// Divide by 1 for GCLK #0 (page 104)
	GCLK->GENCTRL.reg = 0x030600;  		 	// GCLK#0 enable, Source=6(OSC8M), IDC=1 (page 101)
}
