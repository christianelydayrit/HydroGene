#include <SoftwareSerial.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

SoftwareSerial SIM800L(11, 10); 
int update = 1, recv = 0, getter = 0, batt1 = 0, batt2 = 0, batt3 = 0, Tank = 1, tnkCounter,active, Bbye, tankworking = 0, p1=1,p2=1,p3=1,legit, chat=0, a ,b,c;
float  vIn , vInb1, vInb2, vInb3, vOut, vOutb1,vOutb2, vOutb3, voltageSensorVal, voltageSensorValb1, voltageSensorValb2, voltageSensorValb3, minVoltage = 3.2, maxVoltage = 3.9;
String userText = "", userTxt = ""; 
Servo myServo;
const int buttonPin = 7;
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int DCV = A0,voltageSensorPinb1 = A1,voltageSensorPinb2 = A2, voltageSensorPinb3 = A3 ;          // sensor pin                   // value on pin A3 (0 - 1023)
const float factor = 5.128, vCC = 5.00;  
float sensitivity = 0.185;
int addressVariable1 = 0;  // EEPROM address for variable1
int addressVariable2 = 2;  // EEPROM address for variable2
int addressVariable3 = 4;  // EEPROM address for variable
int variable1;
int variable2;
int variable3; 
          // reduction factor of the Voltage Sensor shield                   // Arduino input voltage (measurable by voltmeter)

void writeIntToEEPROM(int address, int value) {
  EEPROM.put(address, value);
}

int readIntFromEEPROM(int address) {
  int value;
  EEPROM.get(address, value);
  return value;
}

void setup() {
    SIM800L.begin(9600); // Initialize SIM800L communication
  Serial.begin(9600);
  Serial.println("HydroGene!!");
  pinMode(buttonPin, INPUT);
  variable1 = readIntFromEEPROM(addressVariable1);
  variable2 = readIntFromEEPROM(addressVariable2);
  variable3 = readIntFromEEPROM(addressVariable3);
  myServo.attach(6);
  myServo.write(90);
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   HELLO FROM");
  lcd.setCursor(0, 1);
  lcd.print("   HYDROGENE");
  delay(5000);
   pinMode(buttonPin, INPUT_PULLUP); 
}

void loop() {
  static unsigned long iterationCount = 0;
  int buttonState = digitalRead(buttonPin);
  int numReadings = 10; // Number of readings to average
  int sensorValue1 = 0;
  int sensorValue2 = 0;
  int sensorValue3 = 0;
  lcd.clear();
  lcd.noBacklight();

  if (update > 0)
  {
    SIM800L.println("AT+CMGF=1"); 
    delay(100);
    SIM800L.println("AT+CMGS=\"+639984438865\""); 
    delay(500);
    SIM800L.println("HELLO FROM HYDROGENE\n Standby for 30secs.");
    //Serial.println("HELLO FROM HYDROGENE\n Standby for 30secs.");
    delay(100);
    SIM800L.write(26);

    update = 0;
    recv = 0;
  }
   /*if (Serial.available() > 0){
    switch (Serial.read()) {
      case 's':
        update = 1;
        Serial.println("Nigguh what");
        break;
    }
   }*/

   else if (update == 0){
    if(recv == 0){
    delay(100); // Allow time for the module to initialize
    SIM800L.println("AT"); // Check if the module is responding
    delay(500);
    SIM800L.println("AT+CMGF=1"); // Set SMS mode to text
    delay(500);
    SIM800L.println("AT+CNMI=2,2,0,0,0"); // Configure SMS to be received directly
    delay(500);
    SIM800L.write(26);

      recv = 1;
    }
    if (SIM800L.available()) {
  char c = SIM800L.read();

  // Look for the user text in the received message
  if (c != '\n') {
    userText += c;
    if (userText == "u" || userText == "U") {
      getter = 1;
      chat = 1;
      Serial.println("uuu");
    } else if (userText == "s" || userText == "S") {
      Bbye = 1;
      chat = 1;
       Serial.println("sss");
      // servo turn AND ALSO TEXT BACK WITH SIGNING OFF
    }
  } else {
    userText = "";
  }
}
 /*voltageSensorVal = analogRead(DCV);    // read the current sensor value (0 - 1023) ------------------------VOLTAGE READER / CURRENT SENSOR
  vOut = (voltageSensorVal / 1024) * vCC;
  vIn =  vOut * factor;            

  RPM_VALUE = mapRPM(vIn);     
  delay(100);         

  Serial.print("Voltage = ");             
  Serial.print(vIn);
  Serial.println("V");*/
        voltageSensorVal = analogRead(DCV);    // read the current sensor value (0 - 1023) ------------------------VOLTAGE READER 
  vOut = (voltageSensorVal / 1024) * vCC;
  vIn =  vOut * factor;      

  if (vIn > 0.7){
    legit++;
  }
  else{
    vIn =0;
    legit =0;}

  if (vIn > 0.7 && legit == 10){//------------------------------------------------------------------------------------------CHECK TANK INDICATOR
    tankworking = 1;
  }
  if(vIn == 0 && tankworking == 1){
    Tank =0;
    tankworking = 0;
  }
     //------------------------------------------------------------------------  -----------------BATTERY INDICATOR
  // Read the voltage from the sensor
  voltageSensorValb1 = analogRead(voltageSensorPinb1); // Read the current sensor value (0 - 1023)
  vOutb1 = (voltageSensorValb1 / 1024.0) * vCC;        // Convert the value to the real voltage on the analog pin
  vInb1= vOutb1 * factor;     
   //Serial.println(vInb1);                        // Convert the voltage on the source by multiplying with the factor
delay(15);
 voltageSensorValb2 = analogRead(voltageSensorPinb2); // Read the current sensor value (0 - 1023)
  vOutb2 = (voltageSensorValb2 / 1024.0) * vCC;        // Convert the value to the real voltage on the analog pin
  vInb2 = vOutb2 * factor;  
  //Serial.println(vInb2); 
delay(15);
  voltageSensorValb3 = analogRead(voltageSensorPinb3); // Read the current sensor value (0 - 1023)
  vOutb3 = (voltageSensorValb3 / 1024.0) * vCC;        // Convert the value to the real voltage on the analog pin
  vInb3 = vOutb3 * factor;  
  //Serial.println(vInb3);
  //Serial.println("percentage");  
delay(15);


  // Define your battery voltage range // Voltage at fully charged

  // Calculate battery percentage manually

  int percentage1 = ((vInb1 - minVoltage) / (maxVoltage - minVoltage)) * 100;
  int percentage2 = ((vInb2 - minVoltage) / (maxVoltage - minVoltage)) * 100;
  int percentage3 = ((vInb3 - minVoltage) / (maxVoltage - minVoltage)) * 100;

  // Ensure percentage is within the valid range (0 - 100)
  percentage1 = constrain(percentage1, 0, 100);
  percentage2 = constrain(percentage2, 0, 100);
  percentage3 = constrain(percentage3, 0, 100);
  delay(100);

  //Serial.println(percentage1);
  //Serial.println(percentage2);
  //Serial.println(percentage3);

 if (percentage1 >= 100 && p1 == 1){
   batt1 = 1;
   p1 = 0;
 }
 if (percentage2 >= 100 && p2 == 1){
   batt2 = 1;
   p2 = 0;
 }
 if (percentage3 >= 100 && p3 == 1){
   batt3 = 1;
   p3 = 0;
 }

 if (percentage1 <=0){
  p1 = 1;
 }
  if (percentage2 <=0){
  p2 = 1;
 }
  if (percentage3 <=0){
  p3 = 1;
 }

//--------------------------------------------------------------------------------------------------------- Print out the voltage and battery percentage

      



  /*Serial.print("Voltage: ");
  Serial.print(vInb1, 2); // Display voltage to 2 decimal places
  Serial.print("V, Battery Percentage: ");
  Serial.print(percentage1);
  Serial.println("%");
  delay(200);
    Serial.println(voltageSensorValb1);
      Serial.println(voltageSensorValb2);
        Serial.println(voltageSensorValb3);*/

      if (getter > 0)
    { //-----------------------------------------------------------UPDATE MODE BASED ON TEXT AND SENDER OF SMS
      delay(100);
      SIM800L.println("AT+CMGF=1"); // Set SMS mode to text
      delay(500);
      SIM800L.println("AT+CMGS=\"+639984438865\"");  // Replace with the recipient's phone number
      delay(500);
      SIM800L.print("Update:\n ");
      SIM800L.print("Battery 1: ");
      SIM800L.print(percentage1);
      SIM800L.print(" BC:");
      SIM800L.print(variable1);
      SIM800L.print("\nBattery 2: ");
      SIM800L.print(percentage2);
      SIM800L.print(" BC:");
      SIM800L.print(variable2);
      SIM800L.print("\nBattery 3: ");
      SIM800L.print(percentage3);
      SIM800L.print(" BC:");
      SIM800L.print(variable3);
      SIM800L.print("\n");
      if (vIn == 0){
        SIM800L.println("Turbine is NOT SPINNING!");
      }
      else{
        SIM800L.print("Turbine is SPINNING!: ");
        SIM800L.print(vIn);
        SIM800L.println("v"); 
      }

      delay(100);
      SIM800L.write(26); // Ctrl+Z to send the message
      delay(1000);
      getter = 0;
      recv = 0;
      chat = 0;
      Serial.println("uuu");
    }
    if (buttonState == LOW) { // Assuming LOW when button is pressed (check your wiring)
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("DCV:");
    lcd.print(vIn);
    if(vIn > 0){
          lcd.print(" CHARGING");
    }
    else{
          lcd.print(" ------");
    }

    lcd.setCursor(0, 1);
    /*Serial.print(voltage1);
    Serial.print(voltage2);
    Serial.print(voltage3);*/
    if (vInb1 >= 1)//------------------------------------------------------TURBINE is working
      {
        lcd.print("B1   ");
      }
      if (vInb2 >= 1)
      {
        lcd.print("  B2   ");
      }
      if (vInb3 >= 1)
      {
        lcd.print("  B3");
      }
       delay(5000);
    lcd.clear();
    lcd.noBacklight();
  }
  if (batt1 == 1)
  {
    delay(100);
    SIM800L.println("AT+CMGF=1"); // Set SMS mode to text
    delay(500);
    SIM800L.println("AT+CMGS=\"+639984438865\""); // Replace with the recipient's phone number
    delay(500);
    SIM800L.println("BATTERY 1 FULL CHARGED");
    Serial.println("battery 1full");
    delay(100);
    SIM800L.write(26); // Ctrl+Z to send the message
    delay(1000);
    variable1++;
    writeIntToEEPROM(addressVariable1, variable1);
    batt1 =0;
    recv = 0;
    chat = 0;
  }
  if (batt2 == 1)
  {
    delay(100);
    SIM800L.println("AT+CMGF=1"); // Set SMS mode to text
    delay(500);
    SIM800L.println("AT+CMGS=\"+639984438865\""); // Replace with the recipient's phone number
    delay(500);
    SIM800L.println("BATTERY 2 FULL CHARGED");
    Serial.println("battery2  full");
    delay(100);
    SIM800L.write(26); // Ctrl+Z to send the message
    delay(1000);
    variable2++;
    writeIntToEEPROM(addressVariable2, variable2);
    batt2 =0;
    recv = 0;
    chat = 0;
  }
  if (batt3 == 1)
  {
    delay(100);
    SIM800L.println("AT+CMGF=1"); // Set SMS mode to text
    delay(500);
    SIM800L.println("AT+CMGS=\"+639984438865\""); // Replace with the recipient's phone number
    delay(500);
    SIM800L.println("BATTERY 3 FULL CHARGED");
    Serial.println("battery 3 full");
    delay(100);
    SIM800L.write(26); // Ctrl+Z to send the message
    delay(1000);
    variable3++;
    writeIntToEEPROM(addressVariable3, variable3);
    batt3 =0;
    recv = 0;
    chat = 0;
  }

   }

   //------------------------------ battery full charge detector and notif sender
  // Read the voltage from the sensor


  // Print out the voltage and battery percentage
 /* Serial.print("Voltage: ");
  Serial.print(vInb1, 2); // Display voltage to 2 decimal places
  Serial.print("V, Battery Percentage: ");
  Serial.print(percentage1);
  Serial.println("%");

  delay(1000);*/
  //----------------------------------- READINGS OF SHNANIGANS

    if (Tank == 0){
    delay(500);
     SIM800L.println("AT+CMGF=1"); // Set SMS mode to text
    delay(500);
    SIM800L.println("AT+CMGS=\"+639984438865\"");// Replace with the recipient's phone number
    delay(500);
    SIM800L.println("Water tank is empty");
    Serial.println("Water tank is empty");
    delay(100);
    SIM800L.write(26); // Ctrl+Z to send the message
    delay(1000);
    Tank++;
    recv = 0;
    chat = 0;
   }

 if (vIn <=0 && chat == 0 && buttonState == HIGH)
  { //---------------------------------------------------------------------------------------INACTIVITY COUNTER
    iterationCount++;
    Serial.println(iterationCount);
  }
  else
  {
    iterationCount = 0;
  }
  if (iterationCount >=40000)
  {
    active = 1;
  }
  //--------------------------------------------------------------------------------------------------

  if (Bbye == 1)
  { //----------------------------------------------------------CLOSING AFTER 10MINS SMS NOTIF
    delay(100);
    SIM800L.println("AT+CMGF=1"); // Set SMS mode to text
    delay(500);
    SIM800L.println("AT+CMGS=\"+639984438865\""); // Replace with the recipient's phone number
    delay(500);
    SIM800L.println("HydroGene Signing OFF");
    delay(500);
    Serial.println("HydroGene Signing OFF");
    delay(100);
    SIM800L.write(26); // Ctrl+Z to send the message
    delay(5000);
    myServo.write(0);

    Bbye=0;
    recv = 0;
    chat = 0;
  }
  if (active == 1)
  { //----------------------------------------------------------CLOSING AFTER 5MINS SMS NOTIF
    delay(100);
    SIM800L.println("AT+CMGF=1"); // Set SMS mode to text
    delay(500);
    SIM800L.println("AT+CMGS=\"+639984438865\""); // Replace with the recipient's phone number
    delay(500);
    SIM800L.println("5mins of inactivity\n HydroGene Signing OFF");
    delay(500);
    SIM800L.write(26); // Ctrl+Z to send the message
    delay(5000);
    myServo.write(0);

   active = 0;
    recv = 0;
    chat = 0;
  }

}
