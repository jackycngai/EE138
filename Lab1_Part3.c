/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>

//Global Variables

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

void Configure_Ports(void);
int Output_Display_KeyPad_Input(int []);

int main(void){ 
	Configure_Ports();
	int digits=0,set=0,temp=0,operation=0,getkey;
	int buffer[4];
	for(;;){
		for(int i=0,j=1;i<4;i++,j*=10)//To store individual digits into an array
			buffer[i]=digits/j%10;
		getkey=Output_Display_KeyPad_Input(buffer);//The returned keypress and output to 7-segment display
		switch(getkey){//determines main calculator functions
			case -1://no press, default case
				set=0;
				break;
			case 10://equal
				if(set==0){
					switch(operation){//These case statements determine which operation is done based off of the pressed operation button (add, subtract, etc.)
						case 0:
							operation=0;
							set=-1;
							break;
						case 1://addition
							digits+=temp;
							operation=0;
							set=-1;
							temp=0;
							break;
						case 2://subtraction
							digits=temp-digits;
							operation=0;
							set=-1;
							temp=0;
							break;
						case 3://multiplication
							digits*=temp;
							operation=0;
							set=-1;
							temp=0;
							break;
						case 4://division
							digits=temp/digits;
							operation=0;
							set=-1;
							temp=0;
							break;

					}
				break;
			case 11://backspace
				if(set==0){
					digits/=10;
					set=-1;
				}
				break;
			case 12: //multiplication
				if (set==0){
					temp=digits;
					digits=0;
					set=-1;
					operation=3;
			}
				break;
			case 13://division or negative. This is for the button D on the keypad. The section that has been commented out is for division. 
					//The block not commented out allows a negative number to be set by pressing the D button
			if (set==0){
				/*
					temp=digits;
					digits=0;
					set=-1;
					operation=4;
					*/
				digits=-digits;//
				set=-1;
			}
				break;
			case 14: //subtraction
				if(set==0){
					temp=digits;
					digits=0;
					set=-1;
					operation=2;
				}
				break;
			case 15://addition
				if(set==0){
					temp=digits;
					digits=0;
					set=-1;
					operation=1;
				}
				break;
			default:
				if(set==0){//prevents overflow on the 7-segment display, shifts and adds new numbers
					if(digits<1000)
						digits=digits*10+getkey;
					set=-1;
				}
				break;
		}
	}
}
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
}	

int Output_Display_KeyPad_Input(int display[]){
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
		if(display[3-i]!=0||i==3||leadzero==1){//Removes leading zeroes
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
	if(count=20)//returns valid keypress
		return oldpress;
	return keypress;
}