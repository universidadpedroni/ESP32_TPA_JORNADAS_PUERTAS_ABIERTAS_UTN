#include "blink.h"

blink::blink(int pin)
{
	this->pin=pin;
	init();
}

void blink::init()
{
	pinMode(pin,OUTPUT);
}


void blink::update(unsigned long interval)
{
	static unsigned long previousMillis = 0;        // will store last time LED was updated
	//const long interval = 1000;           // interval at which to blink (milliseconds)
	unsigned long currentMillis = millis();
	static bool estadoPin=false;
	
	if(currentMillis - previousMillis > interval) 
	{
		previousMillis = currentMillis;
		estadoPin==false? estadoPin=true : estadoPin=false;
		digitalWrite(pin, estadoPin);
	}
    
}
void blink::on(){
  digitalWrite(pin, HIGH);
}

void blink::off(){
  digitalWrite(pin, LOW);
}