#include <Wire.h>  
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
// OR
//LiquidCrystal_I2C lcd(0x3F, 16, 2);

void setup(){
lcd.init();   
lcd.backlight();
lcd.setCursor(5,0);
lcd.print("TA DAA...!");
}

void loop(){
// do nothing here
}
