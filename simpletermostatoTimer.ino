// written by Marco Niccolini IW5EJM
// www.grvdc.eu
// This sketch control home furnace: user inputs desired room temperature and how many hours the system   
// must mantein the set temperature.
// Four button user control: two for +/- temperature and two for timer set.
// At power up the heater is OFF.
// At the press of btnRight countdown starts and the system turn on the boiler until the set temperature is reached or the countdown runs to zero.
// Each press of btnRight add an hour to countdown, btnLeft deducts an hour or powers off the system if the set time is lower than one hour.
// Thanks to Robert Tillaart for countdown library: https://github.com/RobTillaart/Arduino/tree/master/libraries/CountDown
// Released to the public domain

#include <LiquidCrystal.h>
#include "DHT.h"
#include "CountDown.h"

#define DHTPIN A0   //input pin for tempsensor
#define DHTTYPE DHT11 // select 1wire DHT11 sensor
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // lcd is at 12,11,5,4,3,2

#define out 13 // relay output for heater

int temp;  
int settemp = 20; //default temperature
const int btnUp = 6;
const int btnDown = 7;
const int btnRight = 8;
const int btnLeft = 9;

bool Status = 0;
bool Relay = 0;
bool IST = 0;
int timerSet = 0;
int Hour;
int Min;
CountDown CD; //countdown for auto power off

byte upArrow[8] = { // Up Arrow symbol for heater ON
  0b00100, //   *
  0b01110, //  ***
  0b10101, // * * *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100  //   *
};


void setup() {
  pinMode(out,OUTPUT);  
  pinMode(btnUp,INPUT_PULLUP);
  pinMode(btnDown,INPUT_PULLUP);
  pinMode(btnLeft,INPUT_PULLUP);
  pinMode(btnRight,INPUT_PULLUP);
  
  lcd.begin(16, 2);
  lcd.createChar(0, upArrow); //create HD44780 Up Arrow char
  lcd.setCursor(0, 0);            
  lcd.print("Termostato");      
  lcd.setCursor(0, 1);          
  lcd.print("Parigi v0.9");
  delay(2000);
  lcd.clear();
}

void loop() {

  temp = dht.readTemperature(); 
  lcd.setCursor (0,0); 
  lcd.print ("Now:");
  lcd.print (temp);  //print actual temperature
  lcd.print ("C");
  if (Relay) {lcd.write(byte(0)); lcd.write(" ");} else lcd.write("  "); //printe Up Arrow if heater (and the relay output) is ON
  lcd.print ("Set:"); 
  lcd.print (settemp); //print set temperature
  lcd.print ("C");

  lcd.setCursor (0,1); 
  if (CD.isRunning())  lcd.print ("ON for "); else  lcd.print ("OFF                      "); //verify if time is over and print thermostat status on display
  delay (250); 
  
 // routine to read button press
  if (digitalRead(btnUp) == 0) settemp ++;
  if (digitalRead (btnDown) == 0 && (settemp != 0)) settemp --;
  
  if (digitalRead(btnRight) == 0 && (timerSet != 9)) {
    timerSet ++; 
    CD.start(0,timerSet,0,0);
    }
  if (digitalRead(btnLeft) == 0 && (timerSet != 0)) {
    timerSet --; 
    CD.start(0,timerSet,0,0); 
    } 
 // routine to print countdown timer     
  if(timerSet !=0) {
    Hour = CD.remaining()/3600;
    Min = (CD.remaining()/60)%60; if (Min>59) Min = 0;
    lcd.print (timerSet);
    lcd.print ("H |-");
    lcd.print (Hour);
    lcd.print (":"); 
    if (Min<10) lcd.print (0);
    lcd.print (Min);
  }
// very simple hysteresis for temperature control and countdown reset
  if (temp < (settemp - 1)) IST = 1;
  if (temp > (settemp + 1)) IST = 0;
  if (IST && CD.remaining() > 1 && CD.isRunning()) {Relay=1; digitalWrite(out,1);} else {Relay=0; digitalWrite(out,0);} //heater ON-OFF control
  if (CD.remaining() < 1) CD.stop();
  
  delay (250); // wait 250 milliseconds

}
