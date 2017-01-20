////////////////////////////////////////////////////////////////////////////////////// 
////	Lab 2 - Digital To Analog Converter 
////		-SAMPLE CODE DOES NOT WORK-
////			- insert variables into appropriate registers
////			- set up DAC pointer (similar to port_inst setup)
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
void configure_dac(void);
void configure_dac_clock(void);
void configure_ports(void);
int get_key(void);
void sine(void);
void square(void);
void triangle(void);
void sawtooth(void);
void music(void);
//initialize the DAC pointer
Dac *Set_DAC = DAC; //ADC pointer created
int main(void)
{
	simple_clk_init();
	configure_dac_clock();
	configure_dac();
	configure_ports();
	// Wait until the synchronization is complete
	while (Set_DAC->STATUS.reg & DAC_STATUS_SYNCBUSY);
	volatile int toggle=0, delay=0, set=0, wave=0,i=0;
	int sin[]={512,576,639,700,758,812,862,906,943,974,998,1014,1022,1022,1014,998,974,943,906,862,812,758,700,639,576,512,447,384,323,265,211,161,117,80,49,25,9,1,1,9,25,49,80,117,161,211,265,323,384,447};
	for(;;){
		switch(get_key()){
			case -1:
				set=0;
				break;
			case 0:
				if(set==0)
					set=1;
				wave^=1;
				delay=0;
				break;
			case 1://c
				delay=1246;
				break;
			case 2://d
				delay=1094;
				break;
			case 3://e
				delay=977;
				break;
			case 4://f
				delay=917;
				break;
			case 5://g
				delay=809;
				break;
			case 6://a
				delay=714;
				break;
			case 7://b
				delay=629;
				break;
			case 8://C
				delay=591;
				break;
			case 9://play music
				music();
				break;
			case 10://1kHz sine
				sine();
				break;
			case 11://1kHz triangle
				triangle();
				break;
			case 12://1kHz square
				square();
				break;
			case 13://1kHz sawtooth
				sawtooth();
				break;
			case 14: //subtraction
				if(set==0)
					set=1;
				delay++;
				break;
			case 15://addition
				if(set==0)
					set=1;
				delay--;
				break;
			default:
				break;
		}
		if(wave==0){
			Set_DAC->DATA.reg = DAC_DATA_DATA(toggle*1023);
			toggle^=1;
		}
		else{
			if(i==50)
				i=0;
			Set_DAC->DATA.reg = DAC_DATA_DATA(sin[i]);
			i++;
		}
		for(volatile int j=0;j<delay;j++);
	}
}
void music(){
	//The Imperial March-Star Wars
	int frequency[]={2409,2409,2409,3035,2026,2409,3035,2026,2409,1608,1608,1608,1518,2026,2552,3035,2026,2409,1205,2409,2409,1205,1276,1352,1432,1608,1518,2274,1703,1804,1912,2026,2146,2026,3035,2552,3035,2409,2026,2409,2026,1608,1205,2409,2409,1205,1276,1352,1432,1608,1518,2274,1703,1804,1912,2026,2146,2026,3035,2552,3035,2026,2409,3035,2026,2409};
	int length[]={265000,265000,265000,198750,66250,265000,198750,66250,530000,265000,265000,265000,198750,66250,265000,198750,66250,530000,265000,198750,66250,265000,198750,66250,66250,66250,132500,132500,265000,198750,66250,66250,66250,132500,132500,265000,198750,66250,265000,198750,66250,530000,265000,198750,66250,265000,198750,66250,66250,66250,132500,132500,265000,198750,66250,66250,66250,132500,132500,265000,198750,66250,265000,198750,66250,530000};
	int delay[]={265000,265000,265000,198750,66250,265000,198750,66250,530000,265000,265000,265000,198750,66250,265000,198750,66250,530000,265000,198750,66250,265000,198750,66250,66250,66250,265000,132500,265000,198750,66250,66250,66250,265000,132500,265000,198750,66250,265000,198750,66250,530000,265000,198750,66250,265000,198750,66250,66250,66250,265000,132500,265000,198750,66250,66250,66250,265000,132500,265000,198750,66250,265000,198750,66250,530000};
	int temp_frequency[66];
	int temp_length[66];
	int temp_delay[66];
	for(int i=0;i<66;i++){//sets up time delays
		temp_frequency[i]=(frequency[i]/2);
		temp_length[i]=(length[i]/frequency[i]);
		temp_delay[i]=(delay[i]/frequency[i]);
	}
	for(int i=0; i<66;i++){
		for(int j=0;j<temp_length[i];j++){//uses a square wave for the piezoelectric buzzer
			Set_DAC->DATA.reg = DAC_DATA_DATA(0);
			for(int k=0;k<temp_frequency[i];k++);
			Set_DAC->DATA.reg = DAC_DATA_DATA(1023);
			for(volatile int k=0;k<temp_frequency[i];k++);
		}
		for(int j=0;j<temp_delay[i];j++){
			Set_DAC->DATA.reg = DAC_DATA_DATA(0);
			for(volatile int k=0;k<temp_frequency[i];k++);
		}
	}
}
void sine(){
	while (Set_DAC->STATUS.reg & DAC_STATUS_SYNCBUSY);
	int sin[]={512,518,524,530,536,542,548,554,560,566,572,578,584,590,596,602,608,614,620,626,632,638,643,649,655,661,667,672,678,684,690,695,701,707,712,718,723,729,734,740,745,750,756,761,766,772,777,782,787,792,797,802,807,812,817,822,827,831,836,841,845,850,855,859,863,868,872,876,881,885,889,893,897,901,905,909,912,916,920,924,927,931,934,937,941,944,947,950,953,956,959,962,965,968,971,973,976,978,981,983,986,988,990,992,994,996,998,1000,1002,1003,1005,1006,1008,1009,1011,1012,1013,1014,1015,1016,1017,1018,1019,1020,1020,1021,1021,1022,1022,1023,1023,1023,1023,1023,1023,1023,1023,1022,1022,1021,1021,1020,1020,1019,1018,1017,1016,1015,1014,1013,1012,1011,1009,1008,1006,1005,1003,1002,1000,998,996,994,992,990,988,986,983,981,978,976,973,971,968,965,962,959,956,953,950,947,944,941,937,934,931,927,924,920,916,912,909,905,901,897,893,889,885,881,876,872,868,863,859,855,850,845,841,836,831,827,822,817,812,807,802,797,792,787,782,777,772,766,761,756,750,745,740,734,729,723,718,712,707,701,695,690,684,678,672,667,661,655,649,643,638,632,626,620,614,608,602,596,590,584,578,572,566,560,554,548,542,536,530,524,518,512,505,499,493,487,481,475,469,463,457,451,445,439,433,427,421,415,409,403,397,391,385,380,374,368,362,356,351,345,339,333,328,322,316,311,305,300,294,289,283,278,273,267,262,257,251,246,241,236,231,226,221,216,211,206,201,196,192,187,182,178,173,168,164,160,155,151,147,142,138,134,130,126,122,118,114,111,107,103,99,96,92,89,86,82,79,76,73,70,67,64,61,58,55,52,50,47,45,42,40,37,35,33,31,29,27,25,23,21,20,18,17,15,14,12,11,10,9,8,7,6,5,4,3,3,2,2,1,1,0,0,0,0,0,0,0,0,1,1,2,2,3,3,4,5,6,7,8,9,10,11,12,14,15,17,18,20,21,23,25,27,29,31,33,35,37,40,42,45,47,50,52,55,58,61,64,67,70,73,76,79,82,86,89,92,96,99,103,107,111,114,118,122,126,130,134,138,142,147,151,155,160,164,168,173,178,182,187,192,196,201,206,211,216,221,226,231,236,241,246,251,257,262,267,273,278,283,289,294,300,305,311,316,322,328,333,339,345,351,356,362,368,374,380,385,391,397,403,409,415,421,427,433,439,445,451,457,463,469,475,481,487,493,499,505};
	for(;;){
		for(int i=0;i<530;i++){
			Set_DAC->DATA.reg = DAC_DATA_DATA(sin[i]);
		}
	}
	
}

void triangle(){
	while (Set_DAC->STATUS.reg & DAC_STATUS_SYNCBUSY);
	int tri[]={4,8,12,15,19,23,27,31,35,39,42,46,50,54,58,62,66,69,73,77,81,85,89,93,97,100,104,108,112,116,120,124,127,131,135,139,143,147,151,154,158,162,166,170,174,178,181,185,189,193,197,201,205,208,212,216,220,224,228,232,235,239,243,247,251,255,259,263,266,270,274,278,282,286,290,293,297,301,305,309,313,317,320,324,328,332,336,340,344,347,351,355,359,363,367,371,374,378,382,386,390,394,398,401,405,409,413,417,421,425,429,432,436,440,444,448,452,456,459,463,467,471,475,479,483,486,490,494,498,502,506,510,513,517,521,525,529,533,537,540,544,548,552,556,560,564,567,571,575,579,583,587,591,594,598,602,606,610,614,618,622,625,629,633,637,641,645,649,652,656,660,664,668,672,676,679,683,687,691,695,699,703,706,710,714,718,722,726,730,733,737,741,745,749,753,757,760,764,768,772,776,780,784,788,791,795,799,803,807,811,815,818,822,826,830,834,838,842,845,849,853,857,861,865,869,872,876,880,884,888,892,896,899,903,907,911,915,919,923,926,930,934,938,942,946,950,954,957,961,965,969,973,977,981,984,988,992,996,1000,1004,1008,1011,1015,1019,1023,1019,1015,1011,1008,1004,1000,996,992,988,984,981,977,973,969,965,961,957,954,950,946,942,938,934,930,926,923,919,915,911,907,903,899,896,892,888,884,880,876,872,869,865,861,857,853,849,845,842,838,834,830,826,822,818,815,811,807,803,799,795,791,788,784,780,776,772,768,764,760,757,753,749,745,741,737,733,730,726,722,718,714,710,706,703,699,695,691,687,683,679,676,672,668,664,660,656,652,649,645,641,637,633,629,625,622,618,614,610,606,602,598,594,591,587,583,579,575,571,567,564,560,556,552,548,544,540,537,533,529,525,521,517,513,510,506,502,498,494,490,486,483,479,475,471,467,463,459,456,452,448,444,440,436,432,429,425,421,417,413,409,405,401,398,394,390,386,382,378,374,371,367,363,359,355,351,347,344,340,336,332,328,324,320,317,313,309,305,301,297,293,290,286,282,278,274,270,266,263,259,255,251,247,243,239,235,232,228,224,220,216,212,208,205,201,197,193,189,185,181,178,174,170,166,162,158,154,151,147,143,139,135,131,127,124,120,116,112,108,104,100,97,93,89,85,81,77,73,69,66,62,58,54,50,46,42,39,35,31,27,23,19,15,12,8,4,0};
	for(;;){
		for(int i=0;i<530;i++){
			Set_DAC->DATA.reg = DAC_DATA_DATA(tri[i]);
		}
	}
	
}

void square(){
	while (Set_DAC->STATUS.reg & DAC_STATUS_SYNCBUSY);
	int squ[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023};
	for(;;){
		for(int i=0;i<530;i++){
			Set_DAC->DATA.reg = DAC_DATA_DATA(squ[i]);
		}
	}
	
}

void sawtooth(){
	while (Set_DAC->STATUS.reg & DAC_STATUS_SYNCBUSY);
	int saw[]={2,4,6,8,10,12,14,15,17,19,21,23,25,27,29,31,33,35,37,39,41,42,44,46,48,50,52,54,56,58,60,62,64,66,68,69,71,73,75,77,79,81,83,85,87,89,91,93,95,97,98,100,102,104,106,108,110,112,114,116,118,120,122,124,125,127,129,131,133,135,137,139,141,143,145,147,149,151,152,154,156,158,160,162,164,166,168,170,172,174,176,178,180,181,183,185,187,189,191,193,195,197,199,201,203,205,207,208,210,212,214,216,218,220,222,224,226,228,230,232,234,235,237,239,241,243,245,247,249,251,253,255,257,259,261,263,264,266,268,270,272,274,276,278,280,282,284,286,288,290,291,293,295,297,299,301,303,305,307,309,311,313,315,317,318,320,322,324,326,328,330,332,334,336,338,340,342,344,346,347,349,351,353,355,357,359,361,363,365,367,369,371,373,374,376,378,380,382,384,386,388,390,392,394,396,398,400,401,403,405,407,409,411,413,415,417,419,421,423,425,427,429,430,432,434,436,438,440,442,444,446,448,450,452,454,456,457,459,461,463,465,467,469,471,473,475,477,479,481,483,484,486,488,490,492,494,496,498,500,502,504,506,508,510,512,513,515,517,519,521,523,525,527,529,531,533,535,537,539,540,542,544,546,548,550,552,554,556,558,560,562,564,566,567,569,571,573,575,577,579,581,583,585,587,589,591,593,594,596,598,600,602,604,606,608,610,612,614,616,618,620,622,623,625,627,629,631,633,635,637,639,641,643,645,647,649,650,652,654,656,658,660,662,664,666,668,670,672,674,676,677,679,681,683,685,687,689,691,693,695,697,699,701,703,705,706,708,710,712,714,716,718,720,722,724,726,728,730,732,733,735,737,739,741,743,745,747,749,751,753,755,757,759,760,762,764,766,768,770,772,774,776,778,780,782,784,786,788,789,791,793,795,797,799,801,803,805,807,809,811,813,815,816,818,820,822,824,826,828,830,832,834,836,838,840,842,843,845,847,849,851,853,855,857,859,861,863,865,867,869,871,872,874,876,878,880,882,884,886,888,890,892,894,896,898,899,901,903,905,907,909,911,913,915,917,919,921,923,925,926,928,930,932,934,936,938,940,942,944,946,948,950,952,954,955,957,959,961,963,965,967,969,971,973,975,977,979,981,982,984,986,988,990,992,994,996,998,1000,1002,1004,1006,1008,1009,1011,1013,1015,1017,1019,1021,1023};
	for(;;){
		for(int i=0;i<530;i++){
			Set_DAC->DATA.reg = DAC_DATA_DATA(saw[i]);
		}
	}
	
}

void configure_dac_clock(void)
{
	PM->APBCMASK.reg |= 0x1ul << 18; 			// PM_APBCMASK_______ is in the ___ position
	
	uint32_t temp = 0x1A; 			// ID for ________ is__________ (see table 14-2)
	temp |= 0<<8; 					// Selection Generic clock generator 0
	GCLK->CLKCTRL.reg = temp; 			// Setup in the CLKCTRL register
	GCLK->CLKCTRL.reg |= 0x1u << 14; 		// enable it.
}

void configure_dac(void)
{
	//set pin as output for the dac
	Port *ports = PORT_INSTS;
	PortGroup *por = &(ports->Group[0]);
	
	por->PINCFG[2].bit.PMUXEN =1 ;		// set to correct pin configuration
	por->PMUX[1].bit.PMUXE = 1 ;			// set to correct peripheral


	while (Set_DAC->STATUS.reg & DAC_STATUS_SYNCBUSY) {
		/* Wait until the synchronization is complete */
	}

	/* Set reference voltage with CTRLB */
	Set_DAC->CTRLB.reg|=DAC_CTRLB_REFSEL_AVCC;//May need to change reference voltage

	while (Set_DAC->STATUS.reg & DAC_STATUS_SYNCBUSY) {
		/* Wait until the synchronization is complete */
	}

	/* Enable the module with CTRLA */
	Set_DAC->CTRLA.reg |= DAC_CTRLA_ENABLE;//Enables the DAC module
	/* Enable selected output with CTRLB*/
	Set_DAC->CTRLB.reg |= DAC_CTRLB_EOEN;//Writes a 1 to External Output Enable Bit
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
void configure_ports(){
	Port *ports = PORT_INSTS;
	PortGroup *porA = &(ports->Group[0]);
	for(int i=16;i<20;i++)
		porA->PINCFG[i].reg=Input_Enable;
	porA->DIRSET.reg = Row1 | Row2 | Row3 | Row4; // Rows of keypad for output
	porA->OUTSET.reg = Row1 | Row2 | Row3 | Row4; // Rows of keypad for output
	porA->DIRCLR.reg = Col1 | Col2 | Col3 | Col4; // Columns of keypad for input
	porA->OUTCLR.reg = Col1 | Col2 | Col3 | Col4; // Columns of keypad for input
	porA->DIRSET.reg = PORT_PA02;//Speaker
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