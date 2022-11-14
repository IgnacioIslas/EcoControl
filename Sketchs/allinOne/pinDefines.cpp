#include "pinDefines.h"
#include "esp32-hal-gpio.h"

// Class Reless
Reless::Reless(uint8_t PIN_INIT) 
{
	this->PIN=PIN_INIT;
	pinMode(this->PIN, OUTPUT);
	digitalWrite(this->PIN, LOW);
}

void Reless::High() 
{
	digitalWrite(this->PIN, HIGH);
	this->status=HIGH;
}

void Reless::Low() 
{
	digitalWrite(this->PIN, LOW);
	this->status=LOW;
}


void Reless::toggle() 
{
	if (this->status==HIGH)
	{
		digitalWrite(this->PIN, LOW);
		this->status=LOW;
	} 
	else if(this->status==LOW)
	{
		digitalWrite(this->PIN, HIGH);
		this->status=HIGH;
	}
}

// Class Reless

Opto::Opto(uint8_t PIN_INIT) 
{
	this->PIN=PIN_INIT;
	pinMode(this->PIN, INPUT);
  this->status=digitalRead(this->PIN);
}

void Opto::enableInterrupt(void (*functionHandler)(), int modo)
{
  
  this->funcionCallback = &functionHandler;
  attachInterrupt(digitalPinToInterrupt(this->PIN), functionHandler,  modo);
}

uint8_t Opto::ReadOpto() 
{
  this->status=digitalRead(this->PIN);
  return this->status;
}

void Opto::disableInterrupt()
{
  detachInterrupt(this->PIN);
}

