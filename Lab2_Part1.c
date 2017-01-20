#include <asf.h>

//definitions for simplicity
#define a PORT_PB00
#define b PORT_PB01
#define c PORT_PB02
#define d PORT_PB03
#define e PORT_PB04
#define f PORT_PB05
#define g PORT_PB06
#define dpt PORT_PB07
#define neg_sign PORT_PB09;
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

void Simple_Clk_Init(void); 
void enable_adc_clocks(void); 
void init_adc(void); 
int Output_Display(int []);
 void Configure_Ports(void);
unsigned int read_adc(void);
	Adc *Set_ADC = ADC; //ADC pointer created
int main(void){
	Simple_Clk_Init();
	enable_adc_clocks();  
	Configure_Ports(); 
	init_adc();
	//int digits=0,set=0,x=0,getkey;
	int x=0,vin, vin_old=0;
	int buffer[4];
	for(;;){
		x=read_adc();
		vin=(3300*x)/(4095);//Scales voltage read from ADC, voltage value displayed
		if(vin>vin_old+15||vin<vin_old-15)//deadband
			vin_old=vin;
		for(int i=0,j=1;i<4;i++,j*=10)//To store individual digits into an array
			buffer[i]=vin_old/j%10;
		Output_Display(buffer);//Sets voltage read from ADC to display
	}
}
int Output_Display(int display[]){
	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);
	PortGroup *porB = &(ports->Group[1]);
	unsigned int SevenSeg[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x88,0x83,0xC6,0xA1,0x00};//0123456789ABCDnull
	unsigned int keyPad [4][4] = {
	{1, 2, 3, 10},
	{4, 5, 6, 11},
	{7, 8, 9, 12},
	{14, 0, 15, 13}
	};					//Keypad Matrix, 10=A, 11=B, 12=C, 13=D, 14=*, 15=#
	int keypress=-1,count=0,oldpress=-1,leadzero=0;; //set to -1 for no input
	//reads all inputs and sets output to corresponding input.
	for(int i=0;i<4;i++){
		if(display[3-i]<0){//sets negative if number less than 0
			porB->OUTCLR.reg=neg_sign;
			display[3-i]*=(-1);
		}
		else if(display[3-i]>=0)//Turns off negative number LED if number greater than 0
			porB->OUTSET.reg=neg_sign;
		porA->OUTCLR.reg = (1ul << (7-i));//Enables the Row to be scanned (PA7-PA4)
		/*
		if(display[3-i]!=0||i==3||leadzero==1){//Removes leading zeros
			porB->OUTCLR.reg = SevenSeg[display[3-i]];
			leadzero=1;
		}
		*/
		porB->OUTCLR.reg = SevenSeg[display[3-i]];
		if(i==0)
			porB->OUTCLR.reg = dpt;
		for(int k=0;k<20;k++){//Debouncing in addition to 7-segment on delay
			for(int j=0;j<4;j++){//columns to be scanned
				if (porA->IN.reg & (1ul << (19-j)))//column scanning (PB19-PB16)
					keypress=keyPad[i][j];//Gets the keypad button that was pressed based off of the 4x4 array
			}
			if(keypress!=-1){//debouncing counter
				oldpress=keypress;//if button pressed debounced, button pressed is set
				keypress=-1;//resets debouncing
				count++;
			}
		}
		porA->OUTSET.reg = (1ul << (7-i));//clears the currently set row 
		/*
		if(display[3-i]!=0||i==3||leadzero==1)//clears the 7-segment display
			porB->OUTSET.reg = SevenSeg[display[3-i]];
		*/
		porB->OUTSET.reg = SevenSeg[display[3-i]];
		if(i==0)
			porB->OUTSET.reg = dpt;
	}
	if(count=20)//returns valid keypress
		return oldpress;
	return keypress;
}
void Configure_Ports(){
	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);
	PortGroup *porB = &(ports->Group[1]);

	porB->DIRSET.reg = a | b | c | d | e | f | g | dpt | neg_sign; //7-segment Display 
	porB->OUTSET.reg = a |b | c | d | e | f | g | dpt | neg_sign; // 7-segment Display
	porB->PINCFG[0].reg = Drive_Strength; // Sets Drive Strength to high for pin 0
	for(int i=16;i<20;i++)
		porA->PINCFG[i].reg=Input_Enable;
	porA->DIRSET.reg = Row1 | Row2 | Row3 | Row4; // Rows of keypad for output
	porA->OUTSET.reg = Row1 | Row2 | Row3 | Row4; // Rows of keypad for output
	porA->DIRCLR.reg = Col1 | Col2 | Col3 | Col4; // Columns of keypad for input
	porA->OUTCLR.reg = Col1 | Col2 | Col3 | Col4; // Columns of keypad for input
	porA->DIRSET.reg = PORT_PA13;//logic high for pot
	porA->OUTSET.reg = PORT_PA13;//logic high for pot
	porA->DIRCLR.reg = PORT_PA11;//pot
	porA->OUTCLR.reg = PORT_PA11;//pot
}	

unsigned int read_adc(void) {
	// start the conversion  
	Set_ADC->SWTRIG.reg = ADC_SWTRIG_START;
	while(!Set_ADC->INTFLAG.bit.RESRDY); //wait for conversion to be available
	return(Set_ADC->RESULT.reg );
}


// set up generic clock for ADC
void enable_adc_clocks(void)
{
	PM->APBCMASK.reg |= 0x1ul << 16; 			// PM_APBCMASK_______ is in the ___ position
	uint32_t temp = 0x17; 				// ID for ________ is__________ (see table 14-2)
	temp |= 0<<8; 							// Selection Generic clock generator 0
	GCLK->CLKCTRL.reg = temp; 				// Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg |= 0x1ul << 14; 		// enable it.
}

// initialize the on-board ADC system 

void init_adc(void)
{
	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);

	Set_ADC->CTRLA.reg = ~ADC_CTRLA_ENABLE ;				//ADC block is disabled
	
	// you will need to configure 5 registers
	Set_ADC->REFCTRL.reg=ADC_REFCTRL_REFSEL_INTVCC1; //1/2 VDDANA (only for VDDANA > 2.0V)
	Set_ADC->AVGCTRL.reg=ADC_AVGCTRL_SAMPLENUM_1; //1 sample
	Set_ADC->SAMPCTRL.reg=ADC_SAMPCTRL_SAMPLEN(0); // Sampling Time Length 1/2 adc clock
	Set_ADC->CTRLB.reg=ADC_CTRLB_PRESCALER_DIV512|ADC_CTRLB_RESSEL_12BIT; //Peripheral clock divided by 512 and 12 bits
	Set_ADC->INPUTCTRL.reg = ADC_INPUTCTRL_GAIN_DIV2|ADC_INPUTCTRL_MUXPOS_PIN19|ADC_INPUTCTRL_MUXNEG_GND; // 1/2 gain, mux pin 19, mux neg gnd
	// config PA11 to be owned by ADC Peripheral
	porA->PMUX[5].bit.PMUXO = 0x01; // sets mux pin
	porA->PINCFG[11].bit.PMUXEN = 1 ; // sets to perf B

	Set_ADC->CTRLA.reg = ADC_CTRLA_ENABLE ;				//Enable ADC	
}
//Simple Clock Initialization
void Simple_Clk_Init(void)
{
	/* Various bits in the INTFLAG register can be set to one at startup.
	   This will ensure that these bits are cleared */
	
	SYSCTRL->INTFLAG.reg = SYSCTRL_INTFLAG_BOD33RDY | SYSCTRL_INTFLAG_BOD33DET |
			SYSCTRL_INTFLAG_DFLLRDY;
			
	system_flash_set_waitstates(0);  //Clock_flash wait state =0

	SYSCTRL_OSC8M_Type temp = SYSCTRL->OSC8M;      /* for OSC8M initialization  */

	temp.bit.PRESC    = 0;    // no divide, i.e., set clock=8Mhz  (see page 170)
	temp.bit.ONDEMAND = 1;    //  On-demand is true
	temp.bit.RUNSTDBY = 0;    //  Standby is false
	
	SYSCTRL->OSC8M = temp;

	SYSCTRL->OSC8M.reg |= 0x1u << 1;  //SYSCTRL_OSC8M_ENABLE bit = bit-1 (page 170)
	
	PM->CPUSEL.reg = (uint32_t)0;    // CPU and BUS clocks Divide by 1  (see page 110)
	PM->APBASEL.reg = (uint32_t)0;     // APBA clock 0= Divide by 1  (see page 110)
	PM->APBBSEL.reg = (uint32_t)0;     // APBB clock 0= Divide by 1  (see page 110)
	PM->APBCSEL.reg = (uint32_t)0;     // APBB clock 0= Divide by 1  (see page 110)

	PM->APBAMASK.reg |= 01u<<3;   // Enable Generic clock controller clock (page 127)

	/* Software reset Generic clock to ensure it is re-initialized correctly */

	GCLK->CTRL.reg = 0x1u << 0;   // Reset gen. clock (see page 94)
	while (GCLK->CTRL.reg & 0x1u ) {  /* Wait for reset to complete */ }
	
	// Initialization and enable generic clock #0

	*((uint8_t*)&GCLK->GENDIV.reg) = 0;  // Select GCLK0 (page 104, Table 14-10)

	GCLK->GENDIV.reg  = 0x0100;   		 // Divide by 1 for GCLK #0 (page 104)

	GCLK->GENCTRL.reg = 0x030600;  		 // GCLK#0 enable, Source=6(OSC8M), IDC=1 (page 101)
}