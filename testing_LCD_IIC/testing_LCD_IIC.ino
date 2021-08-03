//#include <Wire.h> 
//#include <LiquidCrystal_I2C.h>
//
//LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
//
//void setup()
//{
//  lcd.init();                 // initialize the lcd 
//  // Print a message to the LCD.
//  lcd.backlight();
//  lcd.setCursor(1,0);
//  lcd.print("hello everyone");
//  lcd.setCursor(1,1);
//  lcd.print("konichiwaa");
//}
//
//
//void loop()
//{
//  
//}


YWROBOT
Compatible with the Arduino IDE 1.0
Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
}

void PrintReadings(){
  
//Digital Pin Resistance
//lcd.setCursor(1,0);
//lcd.print("Rc: ");
////lcd.print(Rc);
//delay(3000);
//lcd.clear();

lcd.setCursor(1,0);
lcd.print(" EC: ");
//lcd.print(EC25);
delay(3000);
lcd.clear();

//Compensating For Temperature
//lcd.setCursor(1,0);
//lcd.print(" Simens  ");
////lcd.print(ppm);
//lcd.print(" ppm  ");
//delay(3000);
//lcd.clear();

lcd.setCursor(1,0);
lcd.println("Temperature: ");
//lcd.print(Temperature);
lcd.println(" *C");
delay(3000);
lcd.clear();
}
 

void loop()
{
  PrintReadings();
}
