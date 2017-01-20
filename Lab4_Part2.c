#include <asf.h>

void simple_clock_init(void);
void enable_adc_clocks(void);
void enable_eic_clocks(void);
void enable_tc2_clocks(void);
void enable_tc4_clocks(void);

void configure_ports(void);
void configure_adc(void);
void configure_eic(void);
void configure_tc2_8bit(void);
void configure_tc4_8bit(void);

unsigned int read_adc(void);

int x,y,y_1,count=0,display_digit=0,dir=0,temp_1=0;

int main(void){
	simple_clock_init();
	enable_adc_clocks();
	enable_eic_clocks();
	enable_tc2_clocks();
	enable_tc4_clocks();
	
	configure_ports();
	configure_adc();
	configure_eic();
	configure_tc2_8bit();
	configure_tc4_8bit();
	Tc *tc = TC4;
	TcCount8 *tc8 = &(tc->COUNT8);
	for(;;){
		int read_value=read_adc();
		if (read_value<4)//removes motor jitters at endpoints
			read_value=4;
		if (read_value>251)
			read_value=251;
		tc8->CC[0].reg=(read_value);
		tc8->CC[1].reg=(255-read_value);
	}
}
void simple_clock_init(void){//Simple Clock Initialization
	/* Various bits in the INTFLAG register can be set to one at startup.
	   This will ensure that these bits are cleared */
	SYSCTRL->INTFLAG.reg=SYSCTRL_INTFLAG_BOD33RDY | SYSCTRL_INTFLAG_BOD33DET | SYSCTRL_INTFLAG_DFLLRDY;	
	system_flash_set_waitstates(0);  //Clock_flash wait state=0
	SYSCTRL_OSC8M_Type temp=SYSCTRL->OSC8M;      /* for OSC8M initialization  */
	temp.bit.PRESC=0;    // no divide, i.e., set clock=8Mhz  (see page 170)
	temp.bit.ONDEMAND=1;    //  On-demand is true
	temp.bit.RUNSTDBY=0;    //  Standby is false
	SYSCTRL->OSC8M=temp;
	SYSCTRL->OSC8M.reg|=0x1u << 1;  //SYSCTRL_OSC8M_ENABLE bit=bit-1 (page 170)
	PM->CPUSEL.reg=(uint32_t)0;    // CPU and BUS clocks Divide by 1  (see page 110)
	PM->APBASEL.reg=(uint32_t)0;     // APBA clock 0= Divide by 1  (see page 110)
	PM->APBBSEL.reg=(uint32_t)0;     // APBB clock 0= Divide by 1  (see page 110)
	PM->APBCSEL.reg=(uint32_t)0;     // APBB clock 0= Divide by 1  (see page 110)
	PM->APBAMASK.reg|=01u<<3;   // Enable Generic clock controller clock (page 127)
	/* Software reset Generic clock to ensure it is re-initialized correctly */
	GCLK->CTRL.reg=0x1u << 0;   // Reset gen. clock (see page 94)
	while (GCLK->CTRL.reg&0x1u ) {  /* Wait for reset to complete */ }
	// Initialization and enable generic clock #0
	*((uint8_t*)&GCLK->GENDIV.reg)=0;  // Select GCLK0 (page 104, Table 14-10)
	GCLK->GENDIV.reg=0x0100;   		 // Divide by 1 for GCLK #0 (page 104)
	GCLK->GENCTRL.reg=0x030600;  		 // GCLK#0 enable, Source=6(OSC8M), IDC=1 (page 101)
}
void enable_adc_clocks(void){
	PM->APBCMASK.reg|=PM_APBCMASK_ADC; 			// PM_APBCMASK_ADC is in the 16 position
	GCLK->CLKCTRL.reg=GCLK_CLKCTRL_ID_ADC; 		// Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg|=GCLK_CLKCTRL_CLKEN; 		// enable it.
}
void enable_eic_clocks(void){
	PM->APBAMASK.reg|=PM_APBAMASK_EIC; 			// PM_APBAMASK_EIC is in the 6 position
	GCLK->CLKCTRL.reg=GCLK_CLKCTRL_ID_EIC; 		// Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg|=GCLK_CLKCTRL_CLKEN; 		// enable it.
}
void enable_tc2_clocks(void){
	PM->APBCMASK.reg|=PM_APBCMASK_TC2; 			// PM_APBCMASK_TC2 is in the 19 position
	GCLK->CLKCTRL.reg=GCLK_CLKCTRL_ID_TC2_TC3;  //  Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg|=GCLK_CLKCTRL_CLKEN;    	// enable it.
}
void enable_tc4_clocks(void){
	PM->APBCMASK.reg |= PM_APBCMASK_TC4; 			// PM_APBCMASK_TC2 is in the 12 position
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_TC4_TC5;   	//  Setup in the CLKCTRL register for timer
	GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN;    	// enable it.
}
void configure_ports(void){
	Port *ports=PORT_INSTS;
	PortGroup *porA=&(ports->Group[0]);
	PortGroup *porB=&(ports->Group[1]);
	porA->DIRSET.reg=PORT_PA13;//logic high for pot
	porA->OUTSET.reg=PORT_PA13;
	porA->DIRSET.reg=PORT_PA03;//timer pin
	porA->DIRCLR.reg=PORT_PA28;//encoder
	porB->DIRCLR.reg=PORT_PB14;
	porA->DIRSET.reg=PORT_PA04|PORT_PA05|PORT_PA06|PORT_PA07; //7-segment rows
	porB->DIRSET.reg=PORT_PB00|PORT_PB01|PORT_PB02|PORT_PB03|PORT_PB04|PORT_PB05|PORT_PB06|PORT_PB09; //7-segment display
	
	porA->PMUX[5].reg=PORT_PMUX_PMUXO_B;//adc
	porA->PINCFG[11].reg=PORT_PINCFG_PMUXEN;
	porA->PMUX[14].reg=PORT_PMUX_PMUXE_A;//eic
	porA->PINCFG[28].reg=PORT_PINCFG_PMUXEN|PORT_PINCFG_PULLEN;
	porB->PMUX[7].reg=PORT_PMUX_PMUXE_A;
	porB->PINCFG[14].reg=PORT_PINCFG_PMUXEN|PORT_PINCFG_PULLEN;
	porA->PMUX[11].reg=PORT_PMUX_PMUXO_F|PORT_PMUX_PMUXE_F;//motor
	porA->PINCFG[22].reg=PORT_PINCFG_PMUXEN ;
	porA->PINCFG[23].reg=PORT_PINCFG_PMUXEN ;
}	
void configure_adc(void){
	Adc *adc=ADC;// you will need to configure 5 registers
	adc->REFCTRL.reg=ADC_REFCTRL_REFSEL_INTVCC1; //1/2 VDDANA (only for VDDANA > 2.0V)
	adc->AVGCTRL.reg=ADC_AVGCTRL_SAMPLENUM_1; //1 sample
	adc->SAMPCTRL.reg=ADC_SAMPCTRL_SAMPLEN(0); // Sampling Time Length 1/2 adc clock
	adc->CTRLB.reg=ADC_CTRLB_PRESCALER_DIV512|ADC_CTRLB_RESSEL_8BIT; //Peripheral clock divided by 512 and 10 bits
	adc->INPUTCTRL.reg=ADC_INPUTCTRL_GAIN_DIV2|ADC_INPUTCTRL_MUXPOS_PIN19|ADC_INPUTCTRL_MUXNEG_GND; // 1/2 gain, mux pin 19 (output where voltage is read), mux neg gnd
	adc->CTRLA.reg=ADC_CTRLA_ENABLE ;				//Enable ADC
}
void configure_eic(void){
	Eic *eic=EIC;
	eic->CONFIG[1].reg|=EIC_CONFIG_SENSE0_BOTH;		//Senses on both edges
	eic->CONFIG[1].reg|=EIC_CONFIG_SENSE6_BOTH;
	NVIC->ISER[0]|=(1 << 4);					//Interrupt for the EIC
	eic->INTENSET.reg|=EIC_INTENSET_EXTINT8;
	eic->INTENSET.reg|=EIC_INTENSET_EXTINT14;
	eic->CTRL.reg|=EIC_CTRL_ENABLE;					//Enables EIC interrupt
}
void configure_tc2_8bit(void){
	Tc *tc=TC2;
	TcCount8 *tc8=&(tc->COUNT8);
	tc8->CTRLA.reg=TC_CTRLA_MODE_COUNT8|TC_CTRLA_PRESCALER_DIV256|TC_CTRLA_PRESCSYNC_GCLK|TC_CTRLA_WAVEGEN_NPWM;//Set up CTRLA counter mode, prescaler divider, prescaler prescync
	tc8->PER.reg=153;
	tc8->INTENSET.reg=TC_INTENSET_OVF;
	NVIC->ISER[0]|=(1 << 15);
	tc8->CTRLA.reg|=TC_CTRLA_ENABLE;//Enable TC
}
void configure_tc4_8bit(void){
	Tc *tc = TC4;
	TcCount8 *tc8=&(tc->COUNT8);
	tc8->CTRLA.reg=TC_CTRLA_MODE_COUNT8|TC_CTRLA_PRESCALER_DIV1|TC_CTRLA_PRESCSYNC_GCLK|TC_CTRLA_WAVEGEN_NPWM;//counter mode, prescaler divider, prescaler prescync, waveform
	tc8->CTRLA.reg|=TC_CTRLA_ENABLE;
}
unsigned int read_adc(void){
	// start the conversion
	Adc *adc=ADC;
	adc->SWTRIG.reg=ADC_SWTRIG_START;
	while(!adc->INTFLAG.bit.RESRDY); //wait for conversion to be available
	return(adc->RESULT.reg);
}
void TC2_Handler(void){
	Port *ports=PORT_INSTS;
	PortGroup *porB=&(ports->Group[1]);
	PortGroup *porA=&(ports->Group[0]);
	int sevenseg[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
	int rpm=(count*15/2);//(200Hz*60s)/(1600count/rev)=15/2rpm
	y=(9691*y_1+309*rpm)/10000;//low pass
	y_1=y;
	count=0;
	Tc *tc=TC2;
	TcCount8 *tc8=&(tc->COUNT8);
	if(dir==0)
		porB->OUTSET.reg=PORT_PB09;
	else
		porB->OUTCLR.reg=PORT_PB09;
	switch(display_digit){
		case 0:
			porA->OUTSET.reg=PORT_PA04|PORT_PA05|PORT_PA06|PORT_PA07;
			porB->OUTSET.reg=PORT_PB00|PORT_PB01|PORT_PB02|PORT_PB03|PORT_PB04|PORT_PB05|PORT_PB06;
			porA->OUTCLR.reg=PORT_PA07;
			porB->OUTCLR.reg=sevenseg[y/1000%10];
			display_digit++;
			break;
		case 1:
			porA->OUTSET.reg=PORT_PA04|PORT_PA05|PORT_PA06|PORT_PA07;
			porB->OUTSET.reg=PORT_PB00|PORT_PB01|PORT_PB02|PORT_PB03|PORT_PB04|PORT_PB05|PORT_PB06;
			porA->OUTCLR.reg=PORT_PA06;
			porB->OUTCLR.reg=sevenseg[y/100%10];
			display_digit++;
			break;
		case 2:
			porA->OUTSET.reg=PORT_PA04|PORT_PA05|PORT_PA06|PORT_PA07;
			porB->OUTSET.reg=PORT_PB00|PORT_PB01|PORT_PB02|PORT_PB03|PORT_PB04|PORT_PB05|PORT_PB06;
			porA->OUTCLR.reg=PORT_PA05;
			porB->OUTCLR.reg=sevenseg[y/10%10];
			display_digit++;
			break;
		case 3:
			porA->OUTSET.reg=PORT_PA04|PORT_PA05|PORT_PA06|PORT_PA07;
			porB->OUTSET.reg=PORT_PB00|PORT_PB01|PORT_PB02|PORT_PB03|PORT_PB04|PORT_PB05|PORT_PB06;
			porA->OUTCLR.reg=PORT_PA04;
			porB->OUTCLR.reg=sevenseg[y/1%10];
			display_digit=0;
			break;
		default:
			break;
	}
	tc8->INTFLAG.reg=TC_INTFLAG_OVF;
	//porA->OUTCLR.reg=PORT_PA03;
}
void EIC_Handler(void){
	Eic *eic=EIC;
	Port *ports=PORT_INSTS;
	PortGroup *porA=&(ports->Group[0]);
	PortGroup *porB=&(ports->Group[1]);
	porA->OUTSET.reg=PORT_PA03;
	if(porA->IN.reg&PORT_PA28){
		if(temp_1==0)
			temp_1++;
		else if(porB->IN.reg&PORT_PB14)
			dir=0;//ccw
		else
			dir=1;//cw
	}
	else
		temp_1=0;
	count++;
	eic->INTFLAG.reg|=EIC_INTFLAG_EXTINT8;
	eic->INTFLAG.reg|=EIC_INTFLAG_EXTINT14;
	porA->OUTCLR.reg=PORT_PA03;
}