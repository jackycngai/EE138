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
void enable_tc_adc_clocks(void);
void enable_tc_8bit_adc(void);
int get_key(void);
unsigned int read_adc(void);
Adc *Set_ADC = ADC;
int main(void)
{
	simple_clk_init();
	enable_ports();
	enable_tc_adc_clocks();
	enable_tc_8bit_adc();
	Tc *tc = TC4;
	TcCount8 *tc8 = &(tc->COUNT8);
	int read_value=0;
	for(;;){
		read_value=read_adc();
		tc8->CC[0].reg=(read_value);
		tc8->CC[1].reg=(255-read_value);
	}
}

unsigned int read_adc(void) {
	// start the conversion  
	Set_ADC->SWTRIG.reg = ADC_SWTRIG_START;
	while(!Set_ADC->INTFLAG.bit.RESRDY); //wait for conversion to be available
	return(Set_ADC->RESULT.reg);
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
	porA->DIRSET.reg = PORT_PA22|PORT_PA23;
	porA->OUTSET.reg = PORT_PA22|PORT_PA23;
	porA->DIRSET.reg = PORT_PA13;//logic high for pot
	porA->OUTSET.reg = PORT_PA13;//logic high for pot
	porA->DIRCLR.reg = PORT_PA11;//pot
	porA->OUTCLR.reg = PORT_PA11;//pot
}
/* Perform Clock configuration to source the TC
1) ENABLE THE APBC CLOCK FOR THE CORREECT MODULE
2) WRITE THE PROPER GENERIC CLOCK SELETION ID*/
void enable_tc_adc_clocks(void)
{
	PM->APBCMASK.reg |= PM_APBCMASK_ADC;  // PM_APBCMASK_ADC for ADC	
	PM->APBCMASK.reg |= PM_APBCMASK_TC4;// PM_APBCMASK_TC4 for timer
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_TC4_TC5;   		//  Setup in the CLKCTRL register for timer
	GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN;
	GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_ID_ADC;	//  Setup in the CLKCTRL register for ADC
	GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN;    	// enable it.
}
/* Configure the basic timer/counter for 8 bits*/
void enable_tc_8bit_adc(void)
{

	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);
	porA->PINCFG[22].reg = PORT_PINCFG_PMUXEN ;		// set to correct pin configuration
	porA->PINCFG[23].reg = PORT_PINCFG_PMUXEN ;		// set to correct pin configuration
	porA->PMUX[11].reg = PORT_PMUX_PMUXO_F|PORT_PMUX_PMUXE_F;
	//adc
	porA->PINCFG[11].reg = PORT_PINCFG_PMUXEN;
	porA->PMUX[5].reg = PORT_PMUX_PMUXO_B;
	
	Tc *tc = TC4;
	TcCount8 *tc8 = &(tc->COUNT8);
	/* Set up CTRLA */
	tc8->CTRLA.reg = TC_CTRLA_MODE_COUNT8|TC_CTRLA_PRESCALER_DIV1|TC_CTRLA_PRESCSYNC_PRESC|TC_CTRLA_WAVEGEN_NPWM;//counter mode, prescaler divider, prescaler prescync, waveform
	tc8->PER.reg = 255;
	/*Enable TC*/
	tc8->CTRLA.reg |= TC_CTRLA_ENABLE;
	
	//Set_ADC->CTRLA.reg = ~ADC_CTRLA_ENABLE ;
	Set_ADC->REFCTRL.reg=ADC_REFCTRL_REFSEL_INTVCC1; //1/2 VDDANA (only for VDDANA > 2.0V)
	Set_ADC->AVGCTRL.reg=ADC_AVGCTRL_SAMPLENUM_1; //1 sample
	Set_ADC->SAMPCTRL.reg=ADC_SAMPCTRL_SAMPLEN(0); // Sampling Time Length 1/2 adc clock
	Set_ADC->CTRLB.reg=ADC_CTRLB_PRESCALER_DIV512|ADC_CTRLB_RESSEL_8BIT; //Peripheral clock divided by 512 and 12 bits
	Set_ADC->INPUTCTRL.reg = ADC_INPUTCTRL_GAIN_DIV2|ADC_INPUTCTRL_MUXPOS_PIN19|ADC_INPUTCTRL_MUXNEG_GND; // 1/2 gain, mux pin 19, mux neg gnd
	
	Set_ADC->CTRLA.reg = ADC_CTRLA_ENABLE; //Enable ADC
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
