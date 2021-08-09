  #include <OneWire.h> //Temperature sensor library
  #include <DallasTemperature.h> //Temperature sensor library
  #include <Wire.h>
  #include <LiquidCrystal_I2C.h> //LCD library
  #include <SPI.h> //SD card library
  #include <SD.h> //SD card library
  
  
  #define SensorPin A5          // the pH meter Analog output is connected with the Arduino’s Analog
  //************************* User Defined Variables ********************************************************//
  
  //EC sensor
  int R1 = 1000; //Do not Replace R1 with a resistor lower than 300 ohms
  int Ra = 25; //Resistance of powering Pins
  int ECPin = A0;
  int ECGround = A1;
  int ECPower = A2;
  
  //water level sensor
  int resval = 0;  // holds the value
  int respin = A3; // sensor pin used
  
  //LCD Display
  LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
  
  
  //pH sensor
  unsigned long int avgValue;  //Store the average value of the sensor feedback
  float phValue; // ph
  int buf[10], temp;
  float calibration_value = 21.34;
  float volt;

  //sd card module
  File myFile;
  
  
  float PPMconversion = 0.7;
  
  
  //*************Compensating for temperature ************************************//
  //The value below will change depending on what chemical solution we are measuring
  //0.019 is generaly considered the standard for plant nutrients [google "Temperature compensation EC" for more info
  float TemperatureCoef = 0.019; //this changes depending on what chemical we are measuring
  
  
  
  
  //********************** Cell Constant For Ec Measurements *********************//
  //Mine was around 2.9 with plugs being a standard size they should all be around the same
  //But If you get bad readings you can use the calibration script and fluid to get a better estimate for K
  float K = 2.1; // change this to calibrate the EC, the lower the K constant, the higher the EC reading
  
  
  
  
  //************ Temp Probe Related *********************************************//
  #define ONE_WIRE_BUS 10          // Data wire For Temp Probe is plugged into pin 10 on the Arduino
  const int TempProbePossitive = 8; //Temp Probe power connected to pin 9
  const int TempProbeNegative = 9;  //Temp Probe Negative connected to pin 8
  
  
  
  
  //***************************** END Of Recomended User Inputs *****************************************************************//
  
  //temperature sensor
  OneWire oneWire(ONE_WIRE_BUS);// Setup a oneWire instance to communicate with any OneWire devices
  DallasTemperature sensors(&oneWire);// Pass our oneWire reference to Dallas Temperature.
  
  
  float Temperature = 10;
  float EC = 0;
  float EC25 = 0;
  int ppm = 0;
  
  float raw = 0;
  float Vin = 5;
  float Vdrop = 0;
  float Rc = 0;
  float buffer = 0;
  
  
  
  
  //*********************************Setup - runs Once and sets pins etc ******************************************************//
  void setup()
  {
    //Initialize the lcd
    lcd.begin();
  
    lcd.backlight();
  
    pinMode(TempProbeNegative , OUTPUT ); //seting ground pin as output for tmp probe
    digitalWrite(TempProbeNegative , LOW );//Seting it to ground so it can sink current
    pinMode(TempProbePossitive , OUTPUT );//ditto but for positive
    digitalWrite(TempProbePossitive , HIGH );
    pinMode(ECPin, INPUT);
    pinMode(ECPower, OUTPUT); //Setting pin for sourcing current
    pinMode(ECGround, OUTPUT); //setting pin for sinking current
    digitalWrite(ECGround, LOW); //We can leave the ground connected permanantly
  
    delay(100);// gives sensor time to settle
    sensors.begin();
    delay(100);
    //** Adding Digital Pin Resistance to [25 ohm] to the static Resistor *********//
    // Consule Read-Me for Why, or just accept it as true
    R1 = (R1 + Ra); // Taking into acount Powering Pin Resitance
  
    Serial.println("Arduino EC-PPM measurments");
    Serial.println("By: KaizokuMusou");
    Serial.println("");
    Serial.println("Make sure Probe and Temp Sensor are in Solution and solution is well mixed");
    Serial.println("");
    Serial.println("Measurments at 5's Second intervals [Dont read Ec morre than once every 5 seconds]:");
  
    //initialize the SD card module
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
  
    Serial.print("Initializing SD card...");
  
    if (!SD.begin(4)) {
      Serial.println("initialization failed!");
      while (1);
    }
    Serial.println("initialization done.");
  
  
  
  };
  //******************************************* End of Setup **********************************************************************//
  
  
  
  
  //************************************* Main Loop - Runs Forever ***************************************************************//
  //Moved Heavy Work To subroutines so you can call them from main loop without cluttering the main loop
  void loop()
  {
    GetEC();          //Calls Code to Go into GetEC() Loop [Below Main Loop] dont call this more that 1/5 hhz [once every five seconds] or you will polarise the water
    GetPH();
    waterlevel();
    PrintReadings();  // Cals Print routine [below main loop]
  
    //Saving data into SD card
    save_EC();
    save_temp();
    save_waterlevel();
  
  
    return 0;
  }
  //************************************** End Of Main Loop **********************************************************************//
  
  
  
  
  //************ This Loop Is called From Main Loop************************//
  void GetEC() {
  
  
    //*********Reading Temperature Of Solution *******************//
    sensors.requestTemperatures();// Send the command to get temperatures
    Temperature = sensors.getTempCByIndex(0); //Stores Value in Variable
  
  
    //************Estimates Resistance of Liquid ****************//
    digitalWrite(ECPower, HIGH);
    raw = analogRead(ECPin);
    raw = analogRead(ECPin); // This is not a mistake, First reading will be low beause if charged a capacitor
    digitalWrite(ECPower, LOW);
  
  
    //***************** Converts to EC **************************//
    Vdrop = (Vin * raw) / 1024.0;
    Rc = (Vdrop * R1) / (Vin - Vdrop);
    Rc = Rc - Ra; //acounting for Digital Pin Resitance
    EC = 1000 / (Rc * K);
  
  
    //*************Compensating For Temperature********************//
    EC25  =  EC / (1 + TemperatureCoef * (Temperature - 25.0));
    ppm = (EC25) * (PPMconversion * 1000);
  
    ;
  }
  //************************** End OF EC Function ***************************//
  
  void waterlevel() {
    resval = analogRead(respin); //Read data from analog pin and store it to resval variable
  
    if (resval <= 100) {
      lcd.println("Water level: empty");
      lcd.println(resval);
      delay(3000);
      lcd.clear();
    }
    else if (resval > 100 && resval <= 499) {
      lcd.println("Water level: Low");
      lcd.println(resval);
      delay(3000);
      lcd.clear();
    }
    else if (resval > 500 && resval <= 699) {
      lcd.println("Water level: Medium");
      lcd.println(resval);
      delay(3000);
      lcd.clear();
    }
    else if (resval > 700) {
      lcd.println("Water level: High");
      lcd.println(resval);
      delay(3000);
      lcd.clear();
    }
  }
  //***This Loop Is called From Main Loop- Prints to serial usefull info ***//
  void PrintReadings() {
    //Digital Pin Resistance
    //lcd.setCursor(1,0);
    //lcd.print("Rc: ");
    ////lcd.print(Rc);
    //delay(3000);
    //lcd.clear();
  
    //  lcd.setCursor(1, 0);
    //  lcd.print(" EC: ");
    //  lcd.print(EC25);
    //  delay(3000);
    //  lcd.clear();
  
    //Compensating For Temperature
    lcd.setCursor(1, 0);
    lcd.print(" EC: ");
    lcd.print(ppm);
    lcd.print(" ppm  ");
    delay(3000);
    lcd.clear();
  
    lcd.setCursor(1, 0);
    lcd.print("Temp: ");
    lcd.print(Temperature);
    delay(3000);
    lcd.clear();
  }
  
  void GetPH()
  {
  
    for (int i = 0; i < 10; i++) //Get 10 sample value from the sensor for smooth the value
  
    {
      buf[i] = analogRead(SensorPin);
      delay(500);
    }
    for (int i = 0; i < 9; i++) //sort the analog from small to large
    {
      for (int j = i + 1; j < 10; j++)
      {
        if (buf[i] > buf[j])
        {
          temp = buf[i];
          buf[i] = buf[j];
          buf[j] = temp;
        }
      }
    }
    avgValue = 0;
    for (int i = 2; i < 8; i++)               //take the average value of 6 center sample
      avgValue += buf[i];
    float phValue = -5.70;
    float volt = (float)avgValue * 5.0 / 1024 / 6; //convert the analog into millivolt
    //* volt + calibration_value;
    phValue = 3.5 * phValue;                  //convert the millivolt into pH value
  
    //  lcd.print("pH: ");
    //  //lcd.print(phValue, 2);
    //  delay(3000);
    //  lcd.clear();
  
  }
  
  //Saving reading into SD Card
  
  void save_EC() {
    myFile = SD.open("EC.txt", FILE_WRITE);
  
    if (myFile) { //can open the file
      Serial.print(EC25);
      myFile.println(EC25); //write the data into the text file
      // close the file:
      myFile.close();
      Serial.println("done.");
    }
    else {
      // if the file didn't open, print an error:
      Serial.println("error opening EC.txt");
    }
  }
  
  
  void save_temp() {
    myFile = SD.open("temp.txt", FILE_WRITE);
  
    if (myFile) { //can open the file
      Serial.print(Temperature);
      myFile.println(Temperature); //write the data into the text file
      // close the file:
      myFile.close();
      Serial.println("done.");
    }
    else {
      // if the file didn't open, print an error:
      Serial.println("error opening temp.txt");
    }
  }
  
  void save_waterlevel() {
    myFile = SD.open("waterlevel.txt", FILE_WRITE);
  
    if (myFile) { //can open the file
      Serial.print(resval);
      myFile.println(resval); //write the data into the text file
      // close the file:
      myFile.close();
      Serial.println("done.");
    }
    else {
      // if the file didn't open, print an error:
      Serial.println("error opening waterlevel.txt");
  }
}
