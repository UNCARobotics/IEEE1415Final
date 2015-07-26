#include <math.h>

#define BeginLux A0

float rawRange = 1024;
float logRange = 5.0;
int THRESH = 200;

bool waitToBegin(int stallTime){
	int i;
	int reading;
	bool success=0;
	float timingCount;
	double average;
	double count=0;
	float finalLux;
	
	
	analogReference(EXTERNAL);
	for (i=0;i<1000;i++){             //take an average of the lux sensor by 1000 readings
		count+=analogRead(BeginLux);
		delay(1);
	}
	average=count/1000;               //we must average the raw value rather than the lux value
	finalLux=RawToLux(average);       //average value is transferred to a lux value
	
	timingCount=millis();
	while(millis()<timingCount+stallTime){  //as long as it hasn't timed out, we keep reading
		if(RawToLux(analogRead(BeginLux)) < (finalLux-THRESH)){  //if it drops below the average lux below a threshold
			success=1;  //we have read the light to go off, therefore SUCCESS!
			break;
		}
	}
	return success;  //return whether or not the light went off
}

float RawToLux(int raw){
  float logLux = raw * logRange / rawRange;
  return pow(10, logLux);
}