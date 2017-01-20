#include <asf.h>

void simple_clock_init(void);
void enable_adc_clocks(void);
void enable_dac_clocks(void);
void enable_tc2_clocks(void);
void enable_eic_clocks(void);

void configure_ports(void);
void configure_adc(void);
void configure_dac(void);
void configure_tc2_8bit(void);
void configure_eic(void);

unsigned int read_adc(void);

Adc *adc = ADC;
Dac *dac = DAC;
Tc *tc2 = TC2;    //At port_pa00
Eic *eic = EIC;
int x,x_1,x_2,y,y_1,y_2;

int main(void){
	simple_clock_init();
	enable_adc_clocks();
	enable_dac_clocks();
	enable_tc2_clocks();
	enable_eic_clocks();
	
	configure_ports();
	configure_adc();
	configure_dac();
	configure_tc2_8bit();
	configure_eic();
	for(;;){
		
	}
}
void simple_clock_init(void){//Simple Clock Initialization
	/* Various bits in the INTFLAG register can be set to one at startup.
	   This will ensure that these bits are cleared */
	SYSCTRL->INTFLAG.reg = SYSCTRL_INTFLAG_BOD33RDY | SYSCTRL_INTFLAG_BOD33DET | SYSCTRL_INTFLAG_DFLLRDY;	
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
void enable_adc_clocks(void){
	PM->APBCMASK.reg |= PM_APBCMASK_ADC; 			// PM_APBCMASK_ADC is in the 16 position
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_ADC; 				// Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN; 		// enable it.
}
void enable_dac_clocks(void){
	PM->APBCMASK.reg |= PM_APBCMASK_DAC; 			// PM_APBCMASK_DAC is in the 18 position
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_DAC; 			// Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN; 		// enable it.
}
void enable_tc2_clocks(void){
	PM->APBCMASK.reg |= PM_APBCMASK_TC2;  	// PM_APBCMASK_TC2 is in the 19 position
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_TC2_TC3;   		//  Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN;    	// enable it.
}
void enable_eic_clocks(void){
	PM->APBAMASK.reg |= PM_APBAMASK_EIC;             // PM_APBAMASK_EIC is in the 6 position
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_EIC;                 // Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN;         // enable it.
}
void configure_ports(void){
	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);
	porA->DIRSET.reg = PORT_PA02;//logic high for pot
	porA->OUTSET.reg = PORT_PA02;//logic high for pot
	porA->DIRCLR.reg = PORT_PA11;//pot
	porA->OUTCLR.reg = PORT_PA11;//pot
}	
void configure_adc(void){
	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);
	porA->PMUX[5].reg = PORT_PMUX_PMUXO_B; 	// config PA11 to be owned by ADC Peripheral sets mux pin for PA11 (where voltage is read)
	porA->PINCFG[11].reg = PORT_PINCFG_PMUXEN; // sets to perf B for PA11 (where voltage is read)
	// you will need to configure 5 registers
	adc->REFCTRL.reg=ADC_REFCTRL_REFSEL_INTVCC1; //1/2 VDDANA (only for VDDANA > 2.0V)
	adc->AVGCTRL.reg=ADC_AVGCTRL_SAMPLENUM_1; //1 sample
	adc->SAMPCTRL.reg=ADC_SAMPCTRL_SAMPLEN(0); // Sampling Time Length 1/2 adc clock
	adc->CTRLB.reg=ADC_CTRLB_PRESCALER_DIV512|ADC_CTRLB_RESSEL_10BIT; //Peripheral clock divided by 512 and 12 bits
	adc->INPUTCTRL.reg = ADC_INPUTCTRL_GAIN_DIV2|ADC_INPUTCTRL_MUXPOS_PIN19|ADC_INPUTCTRL_MUXNEG_GND; // 1/2 gain, mux pin 19 (output where voltage is read), mux neg gnd
	adc->CTRLA.reg = ADC_CTRLA_ENABLE ;				//Enable ADC
}
void configure_dac(void){
	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);
	porA->PMUX[1].reg = PORT_PMUX_PMUXE_B;			//set pin as output for the dac set to correct peripheral
	porA->PINCFG[2].reg = PORT_PINCFG_PMUXEN ;		// set to correct pin configuration
	while (dac->STATUS.reg & DAC_STATUS_SYNCBUSY);// Wait until the synchronization is complete
	dac->CTRLB.reg|=DAC_CTRLB_REFSEL_AVCC;/* Set reference voltage with CTRLB */
	while (dac->STATUS.reg & DAC_STATUS_SYNCBUSY);// Wait until the synchronization is complete
	dac->CTRLA.reg |= DAC_CTRLA_ENABLE;//Enables the DAC module
	dac->CTRLB.reg |= DAC_CTRLB_EOEN;// Enable selected output with CTRLBWrites a 1 to External Output Enable Bit
}
void configure_tc2_8bit(void){
	/* pretty sure we don't need this.
	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);
	porA->PINCFG[13].reg = PORT_PINCFG_PMUXEN ;		// set to correct pin configuration
	porA->PMUX[6].reg = PORT_PMUX_PMUXO_E;
	*/
	Tc *tc = TC2;
	TcCount8 *tc8 = &(tc->COUNT8);
	/* Set up CTRLA */
	tc8->CTRLA.reg = TC_CTRLA_MODE_COUNT8|TC_CTRLA_PRESCALER_DIV64|TC_CTRLA_PRESCSYNC_PRESC|TC_CTRLA_WAVEGEN_NPWM;//counter mode, prescaler divider, prescaler prescync
	tc8->PER.reg = 250;
	/*Enable TC*/
	tc8->CTRLA.reg |= TC_CTRLA_ENABLE;
}
void configure_eic(void){
	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);
	eic->CONFIG[1].bit.SENSE0 = 0x1;//pos edge
	NVIC->ISER[0] |= (1 << 15);	//Interrupt for the EIC
	eic->INTENSET.reg = 0x1;
	porA->PMUX[6].bit.PMUXO = 0x01; // sets mux pin for PA11 (where voltage is read)
	porA->PINCFG[13].bit.PMUXEN = 0 ; // sets to perf B for PA11 (where voltage is read)
	eic->CTRL.reg |= 0x2;					//Enables EIC interrupt
}
unsigned int read_adc(void) {
	// start the conversion  
	adc->SWTRIG.reg = ADC_SWTRIG_START;
	while(!adc->INTFLAG.bit.RESRDY); //wait for conversion to be available
	return(adc->RESULT.reg );
}
void TC2_Handler(void){
	y_2=y_1;
	y_1=y;
	x_2=x_1;
	x_1=x;
	x=read_adc();
	y=(-9891*y_2+14797*y_1+8819*x_2-13724*x_1+10000*x)/10000;
	Tc *tc = TC2;
	TcCount8 *tc8 = &(tc->COUNT8);
	dac->DATA.reg = DAC_DATA_DATA(y);
	tc8->INTFLAG.reg = TC_INTFLAG_OVF;
}


