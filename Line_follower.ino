
// Infrared setup /////////////////
#include <IRremote.h>
#define MAX_TIME 300 //max time between before checking putton presses

//track manual control inputs
int RECV_PIN = 0; // remote control input pin
bool state = false;	//true if remote button is currently pressed
bool man_mode = true; //true = manual control, false = line-following mode

//timers
long lastPressTime = 0;
unsigned long serial_timer = 0;
IRrecv irrecv(RECV_PIN);
decode_results results;


//infrared sensor teensy pins
const int irL = 19;
const int irC = 20;
const int irR = 21;
// IR Sensor values
int irValL; // Left
int irValC; // Centre
int irValR; // Right

//motor controllers
const int motorL = 23;
const int motorR = 22;

void setup()
{
	Serial.begin(9600);
	pinMode(13,OUTPUT); //use on board LED to test IR comms
	
	//start the IR receiver
	irrecv.enableIRIn();
	
	//set infrared inputs with internal pullup resistors
	pinMode(irL,INPUT_PULLUP);
	pinMode(irC,INPUT_PULLUP);
	pinMode(irR,INPUT_PULLUP);
 
  serial_timer = millis(); //set initial timer value
  
}

int roundVal(int val){
	if(val < 500){
		val = 0;
	}else{
		val = 1;
	}
	return val;
}
void getIR(){
    // Read sensors
	// Input ranges from 1023 (sensor is over white) and 0 (sensor is over black)
	// Typical range is 800 to 40 (tested on matt B&W photocopier printout on bond paper)
    // TO DO: calibrate sensors?
	irValL = roundVal(analogRead(irL));
    irValC = roundVal(analogRead(irC));
    irValR = roundVal(analogRead(irR));
}
void setMotors(int speedL,int speedR){
		// update motor pin voltages
	analogWrite(motorL,speedL);
	analogWrite(motorR,speedR);
}
void loop(){
	
	//track if in manual mode
	if (man_mode == true){
		digitalWrite(13,HIGH); //turn on onboard LED
	}else{
		digitalWrite(13,LOW); //turn off onboard LED
	}
	
	//handle manual inputs	
	if(irrecv.decode(&results)){

		// update motor outputs manually
			switch (results.value){
				case 0xB520CEC:
					if(state == false){
						//toggle man_mode
						Serial.println("enter");
						man_mode = !man_mode; 
						Serial.print("manual mode: ");
						Serial.println(man_mode);		
					}
				case 0xE246AFCA:
					Serial.println("forward");
					setMotors(1000,1000);
					break;
				case 0xBB0EDD22:
					Serial.println("right");
					setMotors(1000,0);
					break;
				case 0x4D43596A:
					Serial.println("left");
					setMotors(0,1000);
					break;
				
			}
		
		state = true;
		lastPressTime = millis();
		irrecv.resume();
	}
	
	if (millis() - lastPressTime > MAX_TIME && state == true){
		state = false;
		setMotors(0,0);
	}
	
	if (man_mode == false){
		
		/* Line follower mode */
		digitalWrite(13,LOW);
	
		//update IR sensor readings
		getIR();
		
		//determine line position
		if (irValL == 1 and irValC == 0 and irValR == 0 ){
			setMotors(0,1000);
		}else if (irValL == 1 and irValC == 1 and irValR == 0 ){
			setMotors(300,1000);
		}else if (irValL == 1 and irValC == 1 and irValR == 1 ){
			setMotors(1000,1000);
		}else if (irValL == 1 and irValC == 0 and irValR == 1 ){
			setMotors(1000,1000);
		}else if (irValL == 0 and irValC == 1 and irValR == 1 ){
			setMotors(1000,300);
		}else if (irValL == 0 and irValC == 0 and irValR == 1 ){
			setMotors(1000,0);
		}		

	}

//output to serial every second
	if(millis() - serial_timer > 1000){


	serial_timer = millis();
	}
}
