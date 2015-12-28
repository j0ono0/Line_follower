
//timer to throttle down serial out messages
unsigned long serial_timer = 0;

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


//output to serial every second
	if(millis() - serial_timer > 1000){


	serial_timer = millis();
	}
}
