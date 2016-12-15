#include <Wire.h>    // Required for I2C communication
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR    0x3F // <<----- Add your address here.  Find it from I2C Scanner
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
LiquidCrystal_I2C	lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);

long count = 0;
long cpm = 0;
int led = 13;
double usv = 0.0;
int moyen[60];
int moyTemp =  0;
int tmp = 0;
long totalCount = 0;

int vert  = 7;
int rouge = 4;
int bleu  = 9;
int buzz  = 12;
void setup()  
{ 
	Serial.begin(9600);
	Serial.println("Booting...");
	Serial.println(" ");
	Serial.println("=================================");
	Serial.println("= Geiger-Muler Radiation Metter =");
	Serial.println("=================================");

	pinMode(led, OUTPUT);
	pinMode(vert, OUTPUT);
	pinMode(rouge, OUTPUT);
	pinMode(bleu, OUTPUT);
	pinMode(buzz, OUTPUT);
		
	Serial.println("Starting : i2c");
	Wire.begin();
	Serial.println("Starting : LCD");
	lcd.begin (16, 2); //  <<----- My LCD was 20x4
	lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
	lcd.setBacklight(HIGH);
	
	lcd.home();
  
  //uncomment this if runing on arduino 16mhz (uno, etc) 
  setPwmFrequency(5,64); 
  
  //setPwmFrequency(9,8); //pin 9 for 8Mhz. Comment this line if running on 16Mhz
  analogWrite(5, 180);   //starts PWM on pin 9. (replace 9 with 5 if runing on 16Mhz) 
  
  //calls 'countPulse' function when interupt pin goes low 
  //interupt 0 is pin 2 on the Arduino
  attachInterrupt(0,countPulse,FALLING); 
  
  for(int i=0; i<60; i++) moyen[i] = 0;
  
} 

void loop()  
{ 
	if(totalCount > 0) digitalWrite(led, 1);
  	
  delay(1000); //the count is incrementing during this delay
  
  //cpm = 6 *count; 
  usv = cpm * 0.0057;
  totalCount = totalCount + count;
	if(usv > 0.0) 
	{
		digitalWrite(rouge, 1);
		digitalWrite(vert, 0);
	}
	else
	{
		digitalWrite(rouge, 0);
		digitalWrite(vert, 1);
	}

  for(int i=0; i<60; i++) 
  {
  	tmp = i + 1;
  	moyen[i] = moyen[tmp];
  }
  
  moyen[60] = count;
  cpm = 0;
  for(int i=0; i<60; i++) cpm = cpm + moyen[i];
  
  Serial.print("Count = ");
  Serial.print(count);
  Serial.print(" / ");
  Serial.print("CPM = ");
  Serial.print(cpm,DEC);
  Serial.print(" / ");
  Serial.print("uSv = ");
  Serial.print(usv);
  Serial.print(" / ");
  Serial.print("TotalCount = ");
  Serial.println(totalCount);

	lcd.setCursor(0, 0);
	lcd.print("[        CPM   ]");
	lcd.setCursor(0, 1);
	lcd.print("[        uSv   ]");
  	lcd.setCursor(2, 0);
  	lcd.print(cpm); 
  	lcd.setCursor(2, 1);
  	lcd.print(usv); 
  
  count=0; //reset the count
  

                            
}


void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}

void countPulse(){
  digitalWrite(bleu, 1);
  digitalWrite(buzz, 1);
  detachInterrupt(0);
  count++; 
  while(digitalRead(2)==0){
  }
  attachInterrupt(0,countPulse,FALLING);
  digitalWrite(bleu, 0);
  digitalWrite(buzz, 0);

}