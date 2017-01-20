#include <asf.h>

void simple_clock_init(void);
void enable_adc_clocks(void);
void enable_eic_clocks(void);
void enable_tc2_clocks(void);
void enable_tc3_clocks(void);
void enable_tc4_clocks(void);

void configure_ports(void);
void configure_adc(void);
void configure_eic(void);
void configure_tc2_8bit(void);
void configure_tc3_8bit(void);
void configure_tc4_8bit(void);

unsigned int read_adc(void);
int output_display_keypad_input(int);
void pid(int);
void motor_speed(int);

int x,y,y_1,rotation=0,display_digit=0,dir=0,rpm=0,motor_state=0,tc3_time=0,integral=0,pcmd=0,position=0,degrees=0,input=0,set=0; //motor_state 0=idle, 1=acc, 2=speed_contol, 4=decel
float Kp=0,Ki=0,Kff=0,Kd=0,p1=0,diff=0;

int main(void){
	simple_clock_init();
	enable_adc_clocks();
	enable_eic_clocks();
	enable_tc2_clocks();
	enable_tc3_clocks();
	enable_tc4_clocks();
	
	configure_ports();
	configure_adc();
	configure_eic();
	configure_tc2_8bit();
	configure_tc3_8bit();
	configure_tc4_8bit();
	for(;;){
		//int read_value=read_adc();
		//motor_speed(read_value);
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
	PM->APBCMASK.reg|=PM_APBCMASK_TC2; 			// PM_APBCMASK_TC2 is in the 10 position
	GCLK->CLKCTRL.reg=GCLK_CLKCTRL_ID_TC2_TC3;  //  Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg|=GCLK_CLKCTRL_CLKEN;    	// enable it.
}
void enable_tc3_clocks(void){
	PM->APBCMASK.reg|=PM_APBCMASK_TC3; 			// PM_APBCMASK_TC3 is in the 11 position
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
	porA->DIRSET.reg=PORT_PA16|PORT_PA17|PORT_PA18|PORT_PA19;
	for(int i=16;i<20;i++)
		porA->PINCFG[i].reg=PORT_PINCFG_INEN;
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
	eic->CONFIG[1].reg|=EIC_CONFIG_SENSE0_RISE;		//Senses on rise edge
	NVIC->ISER[0]|=(1 << 4);					//Interrupt for the EIC
	NVIC_SetPriority(EIC_IRQn,0);
	eic->INTENSET.reg|=EIC_INTENSET_EXTINT8;
	eic->CTRL.reg|=EIC_CTRL_ENABLE;					//Enables EIC interrupt
}
void configure_tc2_8bit(void){
	Tc *tc=TC2;
	TcCount8 *tc8=&(tc->COUNT8);
	tc8->CTRLA.reg=TC_CTRLA_MODE_COUNT8|TC_CTRLA_PRESCALER_DIV256|TC_CTRLA_PRESCSYNC_GCLK|TC_CTRLA_WAVEGEN_NPWM;//Set up CTRLA counter mode, prescaler divider, prescaler prescync
	tc8->PER.reg=153;
	tc8->INTENSET.reg=TC_INTENSET_OVF;
	NVIC->ISER[0]|=(1 << 15);
	NVIC_SetPriority(TC2_IRQn, 1);
	tc8->CTRLA.reg|=TC_CTRLA_ENABLE;//Enable TC
}
void configure_tc3_8bit(void){
	Tc *tc=TC3;
	TcCount8 *tc8=&(tc->COUNT8);
	tc8->CTRLA.reg=TC_CTRLA_MODE_COUNT8|TC_CTRLA_PRESCALER_DIV1024|TC_CTRLA_PRESCSYNC_GCLK|TC_CTRLA_WAVEGEN_NPWM;//Set up CTRLA counter mode, prescaler divider, prescaler prescync
	tc8->PER.reg=129;
	tc8->INTENSET.reg=TC_INTENSET_OVF;
	NVIC->ISER[0]|=(1 << 16);
	NVIC_SetPriority(TC3_IRQn,2);
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
	rpm=(rotation*30);//(200Hz*60s)/(400count/rev)=30rpm
	degrees=position*360/400;
	y=(9691*y_1+309*rpm)/10000;//low pass
	y_1=y;
	rotation=0;
	Tc *tc=TC2;
	TcCount8 *tc8=&(tc->COUNT8);
	if(motor_state==4)
		pid(degrees);
	else
		pid(rpm);
	tc8->INTFLAG.reg=TC_INTFLAG_OVF;
}
void TC3_Handler(void){
	//Port *ports=PORT_INSTS;
	//PortGroup *porB=&(ports->Group[1]);
	//PortGroup *porA=&(ports->Group[0]);

	Tc *tc3=TC3;
	TcCount8 *tc3_8=&(tc3->COUNT8);
	Tc *tc4=TC4;
	TcCount8 *tc4_8=&(tc4->COUNT8);
	
	int keypad=0;
	//motor_state 0=idle, 1=acc, 2=speed_contol, 3=decel, 4=pos_ctrl
	switch(motor_state){
		case 0:
			keypad=output_display_keypad_input(input);
			position=0;
			integral=0;
			tc3_time=0;
			tc4_8->CC[0].reg=(0);
			tc4_8->CC[1].reg=(0);
			switch(keypad){
				case -1://no press, default case
					set=0;
					break;
				case 10:
					motor_state=1;
					break;
				case 11:
					motor_state=4;
					break;
				case 12:
					if(set==0){
						input*=-1;
						set=-1;
					}
					break;
				case 13:
					input=0;
					break;
				default:
					if(set==0){
						input=input*10+keypad;
						set=-1;
					}
					break;
			}
			break;
		case 1:
			keypad=output_display_keypad_input(rpm);
			position=0;
			if(keypad==13){
				motor_state=3;
			}
			if(rpm==1500&&tc3_time>90)
				motor_state=2;
			if(input==0)
				pcmd=1590,Kp=0.05,Ki=8,Kd=0,Kff=0;
			else
				pcmd=input+60,Kp=0.05,Ki=8,Kd=0,Kff=0;
			break;
		case 2:
			keypad=output_display_keypad_input(rpm);
			position=0;
			integral=0;
			if(keypad==13){
				motor_state=3;
			}
			if(keypad==11)
				motor_state=3;
			break;
		case 3:
			keypad=output_display_keypad_input(rpm);
			position=0;
			if(rpm==0)
				motor_state=0;
			pcmd=0,Kp=0.05,Ki=8,Kd=0,Kff=0;
			if(keypad==11)
				motor_state=4;
			break;
		case 4:
			keypad=output_display_keypad_input(degrees);
			if(input==0)
				pcmd=0,Kp=.8,Ki=0.2,Kd=0.001,Kff=0;
			else
				pcmd=input,Kp=.8,Ki=0.2,Kd=0.001,Kff=0;
			if(keypad==13)
				motor_state=0;
			if(keypad==10)
				motor_state=1;
			break;
		default://shouldn't be possible to enter this state.
			motor_state=0;
			break;
	}
	tc3_time++;
	tc3_8->INTFLAG.reg=TC_INTFLAG_OVF;
	//porA->OUTCLR.reg=PORT_PA03;
}
void EIC_Handler(void){
	Eic *eic=EIC;
	Port *ports=PORT_INSTS;
	PortGroup *porB=&(ports->Group[1]);
	if(porB->IN.reg&PORT_PB14){
		position--;
		rotation--;
	}
	else{
		position++;
		rotation++;
	}
	eic->INTFLAG.reg|=EIC_INTFLAG_EXTINT8;
}

int output_display_keypad_input(int digits){
	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);
	PortGroup *porB = &(ports->Group[1]);
	int display[4];
	for(int i=0,j=1;i<4;i++,j*=10)//To store individual digits into an array
		display[i]=digits/j%10;

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
			porB->OUTCLR.reg=PORT_PB09;
			display[3-i]*=(-1);
		}
		else if(display[3-i]>=0)//Turns off negative number LED if number greater than 0
			porB->OUTSET.reg=PORT_PB09;
		porA->OUTCLR.reg = (1ul << (7-i));//Enables the Row to be scanned (PA7-PA4)
		if(display[3-i]!=0||i==3||leadzero==1){//Removes leading zeros
			porB->OUTCLR.reg = SevenSeg[display[3-i]];
			leadzero=1;
		}
		for(int k=0;k<20;k++){//Debouncing in addition to 7-segment on delay
			for(int j=0;j<4;j++){//columns to be scanned
				if (porA->IN.reg & (1ul << (19-j)))//column scanning (PB19-PB16)
					keypress=keyPad[i][j];//Gets the keypad buttton that was pressed based off of the 4x4 array
			}
			if(keypress!=-1){//debouncing counter
				oldpress=keypress;//if button pressed debounced, button pressed is set
				keypress=-1;//resets debouncing
				count++;
			}
		}
		porA->OUTSET.reg = (1ul << (7-i));//clears the currently set row 
		if(display[3-i]!=0||i==3||leadzero==1)//clears the 7-segment display
			porB->OUTSET.reg = SevenSeg[display[3-i]];
	}
	if(count==20)//returns valid keypress
		return oldpress;
	return keypress;
}

 void motor_speed(int read_value){
	Tc *tc = TC4;
	TcCount8 *tc8 = &(tc->COUNT8);
	read_value=read_value+128;
	 if (read_value<4)//removes motor jitters at endpoints
		read_value=4;
	if (read_value>251)
		read_value=251;
	tc8->CC[0].reg=(read_value);
	tc8->CC[1].reg=(255-read_value);
 }
 
 void pid(int p){
	if(motor_state!=0&&motor_state!=2){
		float z;
		//Input(p);								// p=positon , input from encoder input function.
		float error = pcmd-p;							// pcmd=position cmd
		integral = integral + 0.005*error;		// Integration
		diff= (p-p1)/0.005;						// Differentiation. Ts=sampling interval. Diff is the speed!
		z = Kp*error+Ki*integral+Kd*diff+Kff*pcmd;				//Put all 2 terms together.
		motor_speed(z);
		p1=p;
	}
}