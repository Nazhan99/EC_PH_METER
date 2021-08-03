int resval = 0;  // holds the value
int respin = A3; // sensor pin used
 
void setup() { 

  // start the serial console
  Serial.begin(9600);
} 
 
void loop() { 
  
  resval = analogRead(respin); //Read data from analog pin and store it to resval variable
  
  if (resval<=100){ 
    Serial.println("Water level: empty");
    Serial.println(resval);
  } 
  else if (resval>100 && resval<=499){ 
    Serial.println("Water level: low");
    Serial.println(resval); } 
  else if (resval>500 && resval<=699){ 
    Serial.println("Water level: medium");
    Serial.println(resval); } 
  else if (resval>700){ 
    Serial.println("Water level: High");
    Serial.println(resval); 
  }
  delay(1000); 
}
