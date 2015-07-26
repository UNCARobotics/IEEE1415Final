// Line_Follow_Rev1
//
// succeeds ONLY_CODE_THAT_MATTERS_REV *all revisions

#include <Wire.h>
#include <SPI.h>
#include <math.h>
#include "pins_arduino.h"
#include "Simon_Says_lib.h"
#include "RubixCube.h"
#include "CardPickupLib.h"
#include "EtchaSketch.h"

#define LINE(z)  (Ring[z].tip < threshold_N)
#define FINE_LINE(z)  (Ring[z].tip < threshold_F)
#define SEE_LINE(z)  (Ring[z].tip < threshold_S)

#define BeginLux A0

float rawRange = 1024;
float logRange = 5.0;
int THRESH = 200;

struct SensorArray{
	unsigned int pos, pos_prev, pin_ss, tip;
};

struct GearMotor{
	unsigned int motor_speed_pin, motor_direction_pin;
	boolean motor_direction;
};

struct Controller{
	float error, lasterror, integral_value, correction;

	float proportional, integral, derivative,
		ex_proportional, ex_derivative, d_derivative;

	unsigned int baseSpeed, setPoint;
};

SensorArray Ring[4];
Controller Control[5];
GearMotor Motor[4];

unsigned int count, stage, task, courseFront, writeSpeed, tune_time, holdWrite,
threshold_N, threshold_F, threshold_S, holdstage,
FRONT, LEFT, RIGHT, REAR, sideRead, correction_time;

int trackLeft, trackRight;
boolean setLeft, setRight, sawIt, lastTurn;

EtchaSketch myEAS;
RubixCube myRubix;
CardPickupLib myCard;
Simon_Says_lib mySimon(2500, 500);



unsigned long timerX;

/**************************************************************************************************************************************/
void setup(){

	Serial.begin(115200);
	delay(1000);
	initializeMembers();
	SPI.begin();
	//SPI.setClockDivider(SPI_CLOCK_DIV8);
	delay(1000);

	mySimon.setup();
	mySimon.homeLow(5);
	mySimon.homeTop(5);
	myCard.cardActuator.retract();
	delay(5);
	myCard.cardActuator.stop();

	//waitToBegin(10000);
}

void loop(){
	lineFollow(HIGH);
}

/*********************************************************************************************************************************/
void lineFollow(boolean controlL){
	// Stage 0 --> Control HIGH: Checking for intersection
	// Stage 1 --> Control HIGH: Checking for task box
	// Stage 2 --> Control HIGH: Checking for intersection

	readAll();
	if (controlL){
		if (stage == 0){  // MAIN COURSE FOLLOWING

			sawIt = LOW;
			if (LINE(LEFT) && !LINE(RIGHT)){
				sideRead = LEFT;
				sawIt = HIGH;
				lastTurn = LOW;
			}
			else if (!LINE(LEFT) && LINE(RIGHT)){
				sideRead = RIGHT;
				sawIt = HIGH;
				lastTurn = HIGH;
			}
			else{}

			if (sawIt && SEE_LINE(FRONT) && SEE_LINE(REAR)){

				writeMotors(sideRead, LOW);
				delay(200); // manual(LOW) shift onto the task line

				//if(sideRead==LEFT) intersectCorrect(RIGHT,LOW); // lateral(LOW) correction on the task line
				//else intersectCorrect(LEFT,LOW);

				intersectCorrect(sideRead, LOW);
				rotate(sideRead);  // compute angular direction and distance to move task component to task line

				haltMotors(); delay(250);

				shuffle(task);  // set the new Front and courseFront 
				stage = 1;  // shift to stage 1 (Task Following)
				sawIt = LOW;
			}
		}

		else if (stage == 1){  // TASK LINE FOLLOWING

			readAll();
			if (FINE_LINE(LEFT) && FINE_LINE(RIGHT)){

				writeMotors(REAR, LOW); delay(150);

				intersectCorrect(FRONT, HIGH);

				Control[3].setPoint = 5500;
				intersectCorrect(LEFT, LOW);
				Control[3].setPoint = 3000;

				intersectCorrect(REAR, LOW);
				haltMotors();
				delay(250);
				intersectCorrect(FRONT, HIGH);
				//intersectCorrect(FRONT,HIGH);

				haltMotors();
				delay(1000);
				playGame();

				shuffle(REAR);
				stage = 2;

				//haltMotors(); delay(100);

				readAll();
				//printTips();
				//          Serial.print(LEFT);
				//          Serial.println(Ring[LEFT].tip);
				//          Serial.print(RIGHT);
				//          Serial.println(Ring[RIGHT].tip);

				//          while(LINE(LEFT) || LINE(RIGHT)){ // Escape!
				//            lineFollow(LOW); 
				//            readAll(); 
				//            //printTips();
				//          }
				//printTips();
				//haltMotors(); delay(10000);
				//readAll();
				//while(LINE(LEFT) || LINE(RIGHT)) { writeMotors(FRONT,LOW); delay(50); readAll(); }
				writeMotors(FRONT, LOW); delay(300);
				intersectCorrect(FRONT, LOW);
				return;
			}
		}

		else if (stage == 2){  // RETURN LINE FOLLOWING

			readAll();  // looking for the main course
			if (LINE(courseFront)){

				writeMotors(courseFront, LOW); delay(200);

				intersectCorrect(courseFront, LOW);
				shuffle(courseFront);

				taskOrder();
				stage = 0; // set next task, reset to stage 0

				readAll();
				//          while(LINE(LEFT) || LINE(RIGHT)){
				//            lineFollow(LOW); 
				//            readAll();
				//          } 
				writeMotors(FRONT, LOW); delay(300);
				return;
			}
		}
		else{}
	}

	//readAll();  //printPositions();
	setNewSpeed(LOW, FRONT);  // normal(LOW) controller for line following
}

/******************************************************************************************************************************************/
void readAll(){ // Sample All 4 Slaves

	delayMicroseconds(tune_time);

	byte grab[4], crab[4], drab[4];

	for (int y = 0; y < 4; y++){
		Ring[y].pos = 0;
		Ring[y].tip = 0;
		if (y == 2) digitalWrite(SS, LOW);
		else digitalWrite(Ring[y].pin_ss, LOW);

		drab[y] = SPI.transfer(0);
		delayMicroseconds(10);
		crab[y] = SPI.transfer(0);
		delayMicroseconds(10);
		grab[y] = SPI.transfer(0);
		delayMicroseconds(10);

		if (y == 2) digitalWrite(SS, HIGH);
		else digitalWrite(Ring[y].pin_ss, HIGH);

		Ring[y].pos = ((grab[y] << 8) & 0xff00) + (crab[y] & 0x00ff);
		Ring[y].tip = constrain(map(drab[y], 0, 255, 0, 1000), 0, 1000);

		//Ring[y].pos = constrain(map((Ring[y].pos + grab[y]),0,255,0,6000),0,6000);
		//Ring[y].tip = constrain(map((Ring[y].tip + crab[y]),0,255,0,1000),0,1000);

		//Serial.print(Ring[y].pos); Serial.print("    ");
		if ((Ring[y].pos_prev == 6000) && (Ring[y].pos < 3000)) Ring[y].pos = 6000;
		if ((Ring[y].pos_prev == 0) && (Ring[y].pos > 3000)) Ring[y].pos = 0;

		//      if(Ring[y].pos > Ring[y].pos_prev){
		//        if((Ring[y].pos - Ring[y].pos_prev) > 2000) Ring[y].pos = Ring[y].pos_prev;
		//      }
		//      else{
		//        if((Ring[y].pos_prev - Ring[y].pos) > 2000) Ring[y].pos = Ring[y].pos_prev;
		//      }
		Ring[y].pos_prev = Ring[y].pos;
	}
	//Serial.println();
}

/**************************************************************************************************************************************/
void intersectCorrect(unsigned int Aray, boolean controlI){
	// Control HIGH: Lateral correction (Stage 3), LOW is Rotational (Stage 4)

	holdstage = stage;

	if (!controlI){
		stage = 3; // Force a lateral correction controller
		unsigned long startR = millis();
		while (millis() < (startR + correction_time)){
			readAll();
			if (!((Ring[Aray].pos == 6000) || (Ring[Aray].pos == 0))) setNewSpeed(HIGH, Aray);
			//Serial.println(Control[stage].correction);
		}
	}

	else{
		stage = 4; // Force a rotational correction controller
		unsigned long startD = millis();
		while (millis() < (startD + correction_time)){
			readAll();
			setNewSpeed(HIGH, Aray);
		}
	}

	stage = holdstage;
}

/****************************************************************************************************************************************/
void setNewSpeed(boolean controlS, unsigned int Dside){ // Normal LOW, Forced Set/ Matching Tracks HIGH

	if (stage == 4){
		if (Dside == FRONT) Control[stage].error = Ring[LEFT].tip - Ring[RIGHT].tip;
		else if (Dside == REAR) Control[stage].error = Ring[RIGHT].tip - Ring[LEFT].tip;
		else if (Dside == LEFT) Control[stage].error = Ring[REAR].tip - Ring[FRONT].tip;
		else Control[stage].error = Ring[FRONT].tip - Ring[REAR].tip;

		if (Ring[RIGHT].tip > Ring[LEFT].tip) Control[stage].error = -Control[stage].error;
		Control[stage].error = -Control[stage].error;
	}

	else Control[stage].error = (float)((float)Control[stage].setPoint - (float)Ring[Dside].pos);

	Control[stage].integral = (.5 * Control[stage].integral_value) + Control[stage].error;

	Control[stage].correction = (float)((Control[stage].proportional * Control[stage].error +
		((Control[stage].ex_proportional * Control[stage].error * Control[stage].error * Control[stage].error) / 100)) +
		(Control[stage].derivative * (Control[stage].error - Control[stage].lasterror) +
		(Control[stage].ex_derivative * Control[stage].error * Control[stage].error * (Control[stage].error - Control[stage].lasterror) +
		(Control[stage].integral * Control[stage].integral_value))));

	//Control[stage].correction = -Control[stage].correction;   
	//Control[stage].correction = 0;    
	Control[stage].lasterror = Control[stage].error;

	if (!controlS){
		if (Control[stage].correction >= 0){
			if ((Control[stage].baseSpeed - Control[stage].correction) >= 0){
				trackLeft = Control[stage].baseSpeed - Control[stage].correction;
				setLeft = LOW;
			}
			else{
				trackLeft = Control[stage].correction - Control[stage].baseSpeed;
				setLeft = HIGH;
			}
			if ((Control[stage].baseSpeed + Control[stage].correction) <= 255){
				trackRight = Control[stage].baseSpeed + Control[stage].correction;
				setRight = HIGH;
			}
			else{
				trackRight = 255;
				setRight = HIGH;
			}

			if (trackLeft > 255) trackLeft = 255;
			if (trackRight > 255) trackRight = 255;
		}
		else{
			if ((Control[stage].baseSpeed + Control[stage].correction) >= 0){
				trackRight = Control[stage].baseSpeed + Control[stage].correction;
				setRight = HIGH;
			}
			else{
				trackRight = -(Control[stage].correction + Control[stage].baseSpeed);
				setRight = LOW;
			}
			if ((Control[stage].baseSpeed - Control[stage].correction) <= 255){
				trackLeft = Control[stage].baseSpeed - Control[stage].correction;
				setLeft = LOW;
			}
			else{
				trackLeft = 255;
				setLeft = LOW;
			}

			if (trackLeft > 255) trackLeft = 255;
			if (trackRight > 255) trackRight = 255;
		}
		writeMotors(FRONT, HIGH);
	}

	else{
		setLeft = LOW;
		if (stage == 3) setRight = HIGH;
		else setRight = LOW;

		trackLeft = Control[stage].baseSpeed - Control[stage].correction;
		if (trackLeft < 0){
			trackLeft = -trackLeft;
			setLeft = !setLeft;
			setRight = !setRight;
		}
		if (trackLeft > 255) trackLeft = 255;
		trackRight = trackLeft;

		if (stage == 3){
			if (Dside == LEFT) writeMotors(FRONT, HIGH);
			else if (Dside == RIGHT) writeMotors(REAR, HIGH);
			else if (Dside == FRONT) writeMotors(RIGHT, HIGH);
			else writeMotors(LEFT, HIGH);
		}
		else writeMotors(FRONT, HIGH);
	}
}

/***************************************************************************************************************************************/
void taskOrder(){ // SET ORDER OF TASKS HERE
	// Mounted Order: Rubix(0),Etch(1),Card(2),Simon(3)
	if (task == 0) task = 2;
	else if (task == 1) task = 0;
	else if (task == 2) task = 3;
	else task = 1;
}

void shuffle(unsigned int newFront){ // set the cardinals, and courseFront

	if (task == 0){
		if (!lastTurn) courseFront = 1;
		else courseFront = 3;
	}
	else if (task == 1){
		if (!lastTurn) courseFront = 2;
		else courseFront = 0;
	}
	else if (task == 2){
		if (!lastTurn) courseFront = 3;
		else courseFront = 1;
	}
	else{
		if (!lastTurn) courseFront = 0;
		else courseFront = 2;
	}

	if (newFront == 3){
		FRONT = 3;
		LEFT = 2;
		REAR = 1;
		RIGHT = 0;
	}
	else if (newFront == 2){
		FRONT = 2;
		LEFT = 1;
		REAR = 0;
		RIGHT = 3;
	}
	else if (newFront == 1){
		FRONT = 1;
		LEFT = 0;
		REAR = 3;
		RIGHT = 2;
	}
	else{
		FRONT = 0;
		LEFT = 3;
		REAR = 2;
		RIGHT = 1;
	}
}

void rotate(unsigned int finish){ // rotate from task to finish

	if (finish == 0){
		if (task == 0) return;
		else if (task == 3) maneuver(HIGH);
		else if (task == 2) maneuver(HIGH); //maneuver(HIGH);}
		else if (task == 1) maneuver(LOW);
	}
	else if (finish == 3){
		if (task == 0) maneuver(LOW);
		else if (task == 3) return;
		else if (task == 2) maneuver(HIGH);
		else if (task == 1) maneuver(HIGH); //maneuver(HIGH);}
	}
	else if (finish == 2){
		if (task == 0) maneuver(HIGH); //maneuver(HIGH);}
		else if (task == 3) maneuver(LOW);
		else if (task == 2) return;
		else if (task == 1) maneuver(HIGH);
	}
	else{
		if (task == 0) maneuver(HIGH);
		else if (task == 3) maneuver(HIGH); //maneuver(HIGH);}
		else if (task == 2) maneuver(LOW);
		else if (task == 1) return;
	}
}

void maneuver(boolean dir){ // rotate from current task 90 degrees: CW(HIGH),CCW(LOW)

	unsigned int a, setTaskL, setTaskR;

	for (a = 0; a < 4; a++){
		if ((a == 0) || (a == 3)) digitalWrite(Motor[a].motor_direction_pin, !dir);
		else digitalWrite(Motor[a].motor_direction_pin, dir);
	}

	// Manual Rotation
	for (a = 0; a < 4; a++){
		analogWrite(Motor[a].motor_speed_pin, writeSpeed);
	}
	delay(100);

	readAll();
	while (FINE_LINE(task)){
		for (a = 0; a < 4; a++){
			analogWrite(Motor[a].motor_speed_pin, writeSpeed);
		}
		readAll();
	}
	while (!FINE_LINE(task)){
		for (a = 0; a < 4; a++){
			analogWrite(Motor[a].motor_speed_pin, writeSpeed);
		}
		readAll();
	}
}

/******************************************************************************************************************************************/

void printPositions(){
	Serial.print("Positions:  ");
	for (int x = 0; x < 4; x++){
		Serial.print(Ring[x].pos);
		Serial.print("\t");
	}
	Serial.println();
}

void printTips(){
	Serial.print("\tTips:  ");
	for (int x = 0; x < 4; x++){
		Serial.print(Ring[x].tip);
		Serial.print("\t");
	}
	Serial.println();
	Serial.println();
}

void printSpeeds(){
	Serial.print(trackLeft);
	Serial.print(" ");
	Serial.print(setLeft);
	Serial.print("      ");
	Serial.print(setRight);
	Serial.print(" ");
	Serial.print(trackRight);
	Serial.println();
}

/********************************************************************************************************************************************/
void writeMotors(unsigned int forward, boolean controlled){ // When controlled (HIGH), use Sets/Tracks

	if (!controlled){
		setLeft = LOW;
		setRight = HIGH;
		trackLeft = writeSpeed;
		trackRight = writeSpeed;
	}

	//if(stage == 4) setRight = !setRight;

	if (forward == 0){
		digitalWrite(Motor[0].motor_direction_pin, setLeft);
		digitalWrite(Motor[1].motor_direction_pin, !setRight);
		digitalWrite(Motor[2].motor_direction_pin, !setRight);
		digitalWrite(Motor[3].motor_direction_pin, setLeft);
		analogWrite(Motor[0].motor_speed_pin, trackLeft);
		analogWrite(Motor[1].motor_speed_pin, trackRight);
		analogWrite(Motor[2].motor_speed_pin, trackRight);
		analogWrite(Motor[3].motor_speed_pin, trackLeft);
	}
	else if (forward == 2){
		digitalWrite(Motor[0].motor_direction_pin, setRight);
		digitalWrite(Motor[1].motor_direction_pin, !setLeft);
		digitalWrite(Motor[2].motor_direction_pin, !setLeft);
		digitalWrite(Motor[3].motor_direction_pin, setRight);
		analogWrite(Motor[0].motor_speed_pin, trackRight);
		analogWrite(Motor[1].motor_speed_pin, trackLeft);
		analogWrite(Motor[2].motor_speed_pin, trackLeft);
		analogWrite(Motor[3].motor_speed_pin, trackRight);
	}
	else if (forward == 1){
		digitalWrite(Motor[0].motor_direction_pin, setLeft);
		digitalWrite(Motor[1].motor_direction_pin, !setLeft);
		digitalWrite(Motor[2].motor_direction_pin, !setRight);
		digitalWrite(Motor[3].motor_direction_pin, setRight);
		analogWrite(Motor[0].motor_speed_pin, trackLeft);
		analogWrite(Motor[1].motor_speed_pin, trackLeft);
		analogWrite(Motor[2].motor_speed_pin, trackRight);
		analogWrite(Motor[3].motor_speed_pin, trackRight);
	}
	else{
		digitalWrite(Motor[0].motor_direction_pin, setRight);
		digitalWrite(Motor[1].motor_direction_pin, !setRight);
		digitalWrite(Motor[2].motor_direction_pin, !setLeft);
		digitalWrite(Motor[3].motor_direction_pin, setLeft);
		analogWrite(Motor[0].motor_speed_pin, trackRight);
		analogWrite(Motor[1].motor_speed_pin, trackRight);
		analogWrite(Motor[2].motor_speed_pin, trackLeft);
		analogWrite(Motor[3].motor_speed_pin, trackLeft);
	}
}

void haltMotors(){
	for (int u = 0; u < 4; u++){
		analogWrite(Motor[u].motor_speed_pin, 0);
	}
}

/*******************************************************************************************************************************/
void playGame(){
	//return;
	if (task == 0) playRubix();
	else if (task == 1) playEtch();
	else if (task == 2) playCard();
	else playSimon();
}

void playRubix(){
	drive(FRONT, 50, 1250);
	delay(1000);
	myRubix.playRubixCube();
	drive(REAR, 50, 1500);
}
void playEtch(){
	holdWrite = writeSpeed;
	writeSpeed = 50;
	writeMotors(FRONT, LOW); delay(550); haltMotors(); delay(1000);
	myEAS.LowerMechanismToKnobs(); //shimmy(2000);
	myEAS.PlayEtchaSketch();
	writeMotors(REAR, LOW); delay(550); haltMotors(); delay(1000);
	writeSpeed = holdWrite;
}
void playCard(){
	myCard.init();
	myCard.pickupCard();
}
void playSimon(){
	for (int i = 0; i < 2; i++)
		if (mySimon.playSimon(15000))
			break;
}

void drive(int nDirection, int nSpeed, int nTime)
{
	holdWrite = writeSpeed;
	writeSpeed = nSpeed;
	writeMotors(nDirection, LOW);
	unsigned int timer = millis();
	while (millis() < (timer + nTime))
	{
		// Do nothing
	}
	haltMotors();
	writeSpeed = holdWrite;
}

/**************************************************************************************************************************************/
void initializeMembers(){

	Motor[0].motor_speed_pin = 6;
	Motor[1].motor_speed_pin = 8;
	Motor[2].motor_speed_pin = 10;
	Motor[3].motor_speed_pin = 12;

	Motor[0].motor_direction_pin = 7;
	Motor[1].motor_direction_pin = 9;
	Motor[2].motor_direction_pin = 11;
	Motor[3].motor_direction_pin = 13;

	Ring[0].pin_ss = 49;
	Ring[1].pin_ss = 47;
	Ring[3].pin_ss = 48; // Ring[2] is SS

	for (int u = 0; u < 4; u++){
		if (u == 2){
			digitalWrite(SS, HIGH);
		}
		else{
			pinMode(Ring[u].pin_ss, OUTPUT);
			digitalWrite(Ring[u].pin_ss, HIGH);
		}
		pinMode(Motor[u].motor_speed_pin, OUTPUT);
		pinMode(Motor[u].motor_direction_pin, OUTPUT);
		Ring[u].pos_prev = 3000;
	}

	stage = 0;
	count = 0;
	task = 0; // starting task
	FRONT = 0;
	RIGHT = 1;
	LEFT = 3;
	REAR = 2;
	courseFront = FRONT;
	sawIt = LOW;

	threshold_N = 300; // tip threshold scaled 0-->1000
	threshold_F = 100;
	threshold_S = 500;
	writeSpeed = 125;
	correction_time = 400; // milliseconds of correcting on a line
	tune_time = 1000;

	// Main Course Follower Controller CONSTANTS (STAGE 0)
	Control[0].baseSpeed = 180;
	Control[0].setPoint = 3000;

	Control[0].proportional = .075;
	Control[0].integral = 0;
	Control[0].derivative = 0.15;

	Control[0].ex_proportional = 0.000008;
	Control[0].ex_derivative = 0.000015;
	Control[0].d_derivative = 0;

	// GOOD: 150,.3,0,4,.00009,.000035,0
	// GOOD: 150,.082,0,.97,.0001,.000012
	// OLD JASH: 250,.39,0,.78,.0000018,.00000025

	// Task Follower Controller CONSTANTS (STAGE 1)
	Control[1].baseSpeed = 120;
	Control[1].setPoint = 3000;

	Control[1].proportional = .075;
	Control[1].integral = 0;
	Control[1].derivative = 0.15;

	Control[1].ex_proportional = 0.000008;
	Control[1].ex_derivative = 0.000015;
	Control[1].d_derivative = 0;

	// Return Follower Controller CONSTANTS (STAGE 2)
	Control[2].baseSpeed = 180;
	Control[2].setPoint = 3000;

	Control[2].proportional = .075;
	Control[2].integral = 0;
	Control[2].derivative = 0.15;

	Control[2].ex_proportional = 0.000008;
	Control[2].ex_derivative = 0.000015;
	Control[2].d_derivative = 0;

	// Line Correction (Lateral) Controller CONSTANTS (STAGE 3)
	Control[3].baseSpeed = 0;
	Control[3].setPoint = 3000;
	// .15 & 4.6
	Control[3].proportional = .35;
	Control[3].integral = 0;
	Control[3].derivative = 7.5;

	Control[3].ex_proportional = 0;
	Control[3].ex_derivative = 0;
	Control[3].d_derivative = 0;

	// Line Correction (Rotational) Controller CONSTANTS (STAGE 4)
	Control[4].baseSpeed = 0;
	Control[4].setPoint = 3000;

	// .35 & 6
	Control[4].proportional = .45;
	Control[4].integral = .1;
	Control[4].derivative = 8;

	Control[4].ex_proportional = 0;
	Control[4].ex_derivative = 0;
	Control[4].d_derivative = 0;
}

bool waitToBegin(int stallTime){
	int i;
	int reading;
	bool success = 0;
	float timingCount;
	double average;
	double count = 0;
	float finalLux;


	analogReference(EXTERNAL);
	for (i = 0; i < 1000; i++){             //take an average of the lux sensor by 1000 readings
		count += analogRead(BeginLux);
		delay(1);
	}
	average = count / 1000;               //we must average the raw value rather than the lux value
	finalLux = RawToLux(average);       //average value is transferred to a lux value

	timingCount = millis();
	while (millis() < timingCount + stallTime){  //as long as it hasn't timed out, we keep reading
		if (RawToLux(analogRead(BeginLux)) < (finalLux - THRESH)){  //if it drops below the average lux below a threshold
			success = 1;  //we have read the light to go off, therefore SUCCESS!
			break;
		}
	}
	return success;  //return whether or not the light went off
}

float RawToLux(int raw){
	float logLux = raw * logRange / rawRange;
	return pow(10, logLux);
}

void shimmy(unsigned int shimmy_time){

	unsigned int t = 0; boolean setD = LOW;

	unsigned long timerS = millis();
	while (millis() < (timerS + shimmy_time)){
		for (t = 0; t < 4; t++){
			digitalWrite(Motor[t].motor_direction_pin, setD);
		}
		for (t = 0; t < 4; t++){
			analogWrite(Motor[t].motor_speed_pin, writeSpeed);
		}
		delay(5); setD = !setD;
		for (t = 0; t < 4; t++){
			digitalWrite(Motor[t].motor_direction_pin, setD);
		}
		for (t = 0; t < 4; t++){
			analogWrite(Motor[t].motor_speed_pin, writeSpeed);
		}
		delay(5);
	}
}





