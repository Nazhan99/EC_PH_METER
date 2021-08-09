
#include <SPI.h>
#include <SD.h>

File myFile;
int EC25;
int temperature;
int resval;

void setup() {
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

}

void loop() {
  save_EC();
}

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
    Serial.print(temperature);
    myFile.println(temperature); //write the data into the text file
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
  myFile = SD.open("water_level.txt", FILE_WRITE);

  if (myFile) { //can open the file
    Serial.print(resval);
    myFile.println(resval); //write the data into the text file
    // close the file:
    myFile.close();
    Serial.println("done.");
  }
  else {
    // if the file didn't open, print an error:
    Serial.println("error opening water_level.txt");
  }
}
