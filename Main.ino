
/*
// /////////////////////////////////////////////////////////////////////// COPYRIGHT NOTICE
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with Pip-Project.  If not, see <http://www.gnu.org/licenses/>.
// /////////////////////////////////////////////////////////////////////// COPYRIGHT NOTICE
// /////////////////////////////////////////////////////////////////////// VERSION CONTROL
// PAGE CREATED BY: Phillip Kraguljac
// PAGE CREATED DATE: 2019-07-20
// DATE       || NAME           || MODIFICATION
// 2019-11-22   || Phillip Kraguljac    || Created.
// /////////////////////////////////////////////////////////////////////// VERSION CONTROL
*/

/* STILL UNDER CONSTRUCTION */

#include <LiquidCrystal.h>

#include <Wire.h>
#include "Adafruit_MCP9808.h"
Adafruit_MCP9808 Sensor_1 = Adafruit_MCP9808();
Adafruit_MCP9808 Sensor_2 = Adafruit_MCP9808();
Adafruit_MCP9808 Sensor_3 = Adafruit_MCP9808();

// /////////////////////////////////////////////////////////////////////// USER DEFINABLE VARIABLES
int Startup_Delay = 60;                                                             // Cycle time allocated for start-up.
int Waiting_Delay = 200;                                                            // Cycle time allocated for start-up.
int Serial_Output_Delay = 60;                                                       // .
int Display_Output_Delay = 100;    
int Override_Switch = 53;
int Scan_Delay = 100;
float Maximum_Temperature = 60.0;

// /////////////////////////////////////////////////////////////////////// SYSTEM VARIABLES
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;                           // Required for LCD.
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);                                          // Required for LCD.
bool Override_Flag = false;                                                         // Flag used for triggering system override.
int Mode_Time_Indexer = 0;                                                          // Index used for mode cycle timers.
int Scan_Delay_Indexer = 0;
String Mode = "START-UP";                                                           // Mode Indexer.
String Upper_LCD_Line = "";
String Lower_LCD_Line = "";

String Indicator_Output = "Stopped";                                                // ...
int Green_LED = 17;                                                                 // ...
int Red_LED = 18;                                                                   // ...
int Blue_LED = 19;                                                                  // ...

bool Flash_Fast_Flag = false;                                                       // ...
int Flash_Indexer = 0;                                                              // ...
int Flash_Fast_Cycles = 20;                                                         // ...



float Sensor_1_Temperature_Celsius = 0;
float Sensor_2_Temperature_Celsius = 0;
float Sensor_3_Temperature_Celsius = 0;
float Highest_Temperature = 0;

bool Alarm_High_Temperature = false;
bool High_Temp_Check_1 = false;
bool High_Temp_Check_2 = false;
bool High_Temp_Check_3 = false;


int temp_index = 0;


void setup() {
  
Serial.begin (9600);
pinMode(LED_BUILTIN, OUTPUT);                                                       // Declare IO's.
pinMode(16, OUTPUT);
lcd.begin(16, 2);
lcd.clear();                                                                        // Reset LCD screen.

pinMode(Green_LED, OUTPUT);                                                  // Declare IO's.
pinMode(Red_LED, OUTPUT);                                                    // Declare IO's.
pinMode(Blue_LED, OUTPUT);                                                   // Declare IO's.

//
//tempsensor.begin(0x1C);

//if (!tempsensor.begin(0x19)) {
//    Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct.");
//    while (1);
//  }



//Mode_Restart();                                                                   // Complete Mode reset.  
}


void loop() {
Override_Flag = digitalRead(Override_Switch);

Flash_Operation();
Scan_Temperature_Sensors();
Temperature_Monitoring();




// /////////////////////////////////////////////////////////////////////// MODE(S)

// [MODE] => "START-UP"
if(Mode=="START-UP"){                                                               // Start-up mode in operation.
if(Override_Flag){Mode = "OVERRIDE"; Mode_Restart();}                               // Go to override if switched.
Mode_Time_Indexer = Mode_Time_Indexer + 1;                                          // Increment mode counter.
Upper_LCD_Line = "Mode: "+Mode;
Lower_LCD_Line = Progress_Bar(Startup_Delay, Mode_Time_Indexer);
General_LCD_Output(Upper_LCD_Line, Lower_LCD_Line);
if(Mode_Time_Indexer > Startup_Delay){Mode = "AUX"; Mode_Restart();}            // Switch mode once complete.
}


// [MODE] => "AUX"
if(Mode=="AUX"){                                                                // Waiting for pallet mode in operation.
if(Override_Flag){Mode = "OVERRIDE"; Mode_Restart();}                               // Go to override if switched.
Mode_Time_Indexer = Mode_Time_Indexer + 1;                                          // Increment mode counter.
String Aux_Check = "";

// ---------------------------------------

if (Sensor_1.begin(0x19)){
Aux_Check = Aux_Check + "1;";
Sensor_1.setResolution(3);
}
if (Sensor_2.begin(0x1A)){
Aux_Check = Aux_Check + "2;";
Sensor_2.setResolution(3);
}
if (Sensor_3.begin(0x1C)){
Aux_Check = Aux_Check + "3;";
Sensor_3.setResolution(3);
}
// ------------------------------------------

Upper_LCD_Line = "Mode: "+Mode;
Lower_LCD_Line = Aux_Check;
General_LCD_Output(Upper_LCD_Line, Lower_LCD_Line);
Indicator_Output = "System Fault (Operational)";
if(Mode_Time_Indexer > Waiting_Delay){Mode = "DISPLAY-01"; Mode_Restart();}        // Switch mode once complete.
}  


// [MODE] => "DISPLAY-01"
if(Mode=="DISPLAY-01"){                                                                // Waiting for pallet mode in operation.
if(Override_Flag){Mode = "OVERRIDE"; Mode_Restart();}                               // Go to override if switched.
if(Alarm_High_Temperature){Mode = "ALARM"; Mode_Restart();}                         // ...
Mode_Time_Indexer = Mode_Time_Indexer + 1;                                          // Increment mode counter.
Upper_LCD_Line = "Mode: "+Mode;
Lower_LCD_Line = String(Sensor_1_Temperature_Celsius)+"^C";
General_LCD_Output(Upper_LCD_Line, Lower_LCD_Line);
Indicator_Output = "Running - Condition";
if(Mode_Time_Indexer > Display_Output_Delay){Mode = "DISPLAY-02"; Mode_Restart();}        // Switch mode once complete.
}  


// [MODE] => "DISPLAY-02"
if(Mode=="DISPLAY-02"){                                                                // Waiting for pallet mode in operation.
if(Override_Flag){Mode = "OVERRIDE"; Mode_Restart();}                               // Go to override if switched.
if(Alarm_High_Temperature){Mode = "ALARM"; Mode_Restart();}                         // ...
Mode_Time_Indexer = Mode_Time_Indexer + 1;                                          // Increment mode counter.
Upper_LCD_Line = "Mode: "+Mode;
Lower_LCD_Line = String(Sensor_2_Temperature_Celsius)+"^C";
General_LCD_Output(Upper_LCD_Line, Lower_LCD_Line);
Indicator_Output = "Running - Condition";
if(Mode_Time_Indexer > Display_Output_Delay){Mode = "DISPLAY-03"; Mode_Restart();}        // Switch mode once complete.
}  

// [MODE] => "DISPLAY-03"
if(Mode=="DISPLAY-03"){                                                                // Waiting for pallet mode in operation.
if(Override_Flag){Mode = "OVERRIDE"; Mode_Restart();}                               // Go to override if switched.
if(Alarm_High_Temperature){Mode = "ALARM"; Mode_Restart();}                         // ...
Mode_Time_Indexer = Mode_Time_Indexer + 1;                                          // Increment mode counter.
Upper_LCD_Line = "Mode: "+Mode;
Lower_LCD_Line = String(Sensor_3_Temperature_Celsius)+"^C";
General_LCD_Output(Upper_LCD_Line, Lower_LCD_Line);
Indicator_Output = "Running - Condition";
if(Mode_Time_Indexer > Display_Output_Delay){Mode = "DISPLAY-01"; Mode_Restart();}        // Switch mode once complete.
}  


// [MODE] => "ALARM"
if(Mode=="ALARM"){                                                                // Waiting for pallet mode in operation.
if(Override_Flag){Mode = "OVERRIDE"; Mode_Restart();}                               // Go to override if switched.




String Alarm_Check = "";
if(High_Temp_Check_1){Alarm_Check = Alarm_Check + "1;";}
if(High_Temp_Check_2){Alarm_Check = Alarm_Check + "2;";}
if(High_Temp_Check_3){Alarm_Check = Alarm_Check + "3;";}

if(Sensor_1_Temperature_Celsius>Highest_Temperature){Highest_Temperature = Sensor_1_Temperature_Celsius;}
if(Sensor_2_Temperature_Celsius>Highest_Temperature){Highest_Temperature = Sensor_2_Temperature_Celsius;}
if(Sensor_3_Temperature_Celsius>Highest_Temperature){Highest_Temperature = Sensor_3_Temperature_Celsius;}

Mode_Time_Indexer = Mode_Time_Indexer + 1;                                          // Increment mode counter.
Upper_LCD_Line = "Mode: "+Mode;
Lower_LCD_Line = Alarm_Check+"=>"+Highest_Temperature+"^C";
General_LCD_Output(Upper_LCD_Line, Lower_LCD_Line);
Indicator_Output = "System Fault (Safety)";

if(!Alarm_High_Temperature){Mode = "DISPLAY-01"; Mode_Restart();}                   // Switch mode once complete.
} 



if(Alarm_High_Temperature&&Flash_Fast_Flag){
tone(16,1000);
}else{
  noTone(16);
}


if(Indicator_Output == "Stopped"){
digitalWrite(Green_LED, HIGH);
digitalWrite(Red_LED, HIGH);
digitalWrite(Blue_LED, HIGH);
}

if(Indicator_Output == "Running - Condition"){
digitalWrite(Green_LED, LOW);
digitalWrite(Red_LED, HIGH);
digitalWrite(Blue_LED, HIGH);
}


if(Indicator_Output == "System Fault (Operational)"){
digitalWrite(Green_LED, Flash_Fast_Flag);
digitalWrite(Red_LED, Flash_Fast_Flag);
digitalWrite(Blue_LED, HIGH);
}

if(Indicator_Output == "System Fault (Safety)"){
digitalWrite(Green_LED, HIGH);
digitalWrite(Red_LED, Flash_Fast_Flag);
digitalWrite(Blue_LED, HIGH);
}



delay(25);                                                                          // Base cycle throttle. [Do not remove!]
}


// ///////////////////////////////////////////////////////////////// FUNCTION(S)


// [FUNCTION] => FLASH FUNCTIONS
void Flash_Operation(){
if(Flash_Indexer < Flash_Fast_Cycles){Flash_Indexer=Flash_Indexer+1;}               // ...
if(Flash_Indexer >= Flash_Fast_Cycles){                                             // ...
Flash_Indexer = 0;                                                                  // ...
if(Flash_Fast_Flag == true){Flash_Fast_Flag = false;}else{Flash_Fast_Flag = true;}  // ...
}                                                                                   // ...
}

// [FUNCTION] => OBTAING DISTANCE FROM ULTRASONIC SENSORS (RAW PULSES)
void Scan_Temperature_Sensors(){
Scan_Delay_Indexer = Scan_Delay_Indexer + 1;
if(Scan_Delay_Indexer > Scan_Delay){
Sensor_1_Temperature_Celsius = Sensor_1.readTempC();
Sensor_2_Temperature_Celsius = Sensor_2.readTempC();
Sensor_3_Temperature_Celsius = Sensor_3.readTempC();
Scan_Delay_Indexer = 0;
}
}

void Temperature_Monitoring(){

High_Temp_Check_1 = false;
High_Temp_Check_2 = false;
High_Temp_Check_3 = false;
if(Sensor_1_Temperature_Celsius > Maximum_Temperature){High_Temp_Check_1 = true;}
if(Sensor_2_Temperature_Celsius > Maximum_Temperature){High_Temp_Check_2 = true;}
if(Sensor_3_Temperature_Celsius > Maximum_Temperature){High_Temp_Check_3 = true;}

if(High_Temp_Check_1 || High_Temp_Check_2 || High_Temp_Check_3){
  Alarm_High_Temperature = true;
}else{
  Alarm_High_Temperature = false;
}

  
}






// [FUNCTION] => OBTAING DISTANCE FROM ULTRASONIC SENSORS (RAW PULSES)
long Scan_Sensor(int Sensor_Trigger, int Sensor_Echo){
long Sensor_Duration = 0;                                                           // Value retrieved from sensor.
long Sensor_mm = 0;                                                                 // Converted measurement.
digitalWrite(Sensor_Trigger, LOW);                                                  // Set output LOW.
delayMicroseconds(5);                                                               // Wait clearance time.
digitalWrite(Sensor_Trigger, HIGH);                                                 // Set output HIGH.
delayMicroseconds(10);                                                              // Sending input trigger.
digitalWrite(Sensor_Trigger, LOW);                                                  // Set output LOW.
pinMode(Sensor_Echo, INPUT);
Sensor_Duration =  pulseIn(Sensor_Echo, HIGH);                                      // Retrieve HCSR04 feedback.
Sensor_mm = (Sensor_Duration/2)*0.303;                                              // Convert to mm.
return Sensor_mm;                                                                   // Return value.
}


// [FUNCTION] => Progress Bar Text Creation.
String Progress_Bar(int Set_Value, int Counter){
String Output_String = "";
long Available_Segments = 16;
float Real_Progress_Ratio = float(Counter) / float(Set_Value);
int Artificial_Progress = int(Real_Progress_Ratio*Available_Segments);
int i;
for (i = 0; i < Artificial_Progress; i++){
Output_String += ".";  
}
return Output_String;  
}


// [FUNCTION] => Determine if value is within margin.
bool Within_Margin(int Measurement, int Set_Distance, int Margin){
bool Output_String = false;
if(Measurement>(Set_Distance-Margin)&&Measurement<(Set_Distance+Margin)){
Output_String = true;  
}
  return Output_String; 
}


// [FUNCTION] => General Serial Output
void General_Serial_Output(){
Serial.print(":");
Serial.print("1");
Serial.print(":");
Serial.print("2");
Serial.print(":");
Serial.print("3");
Serial.print(":");
Serial.println("4");
}


// [FUNCTION] => General LCD Output
void General_LCD_Output(String Upper_LCD_Line, String Lower_LCD_Line){
//lcd.clear();
lcd.setCursor(0, 0);                                                                // Set LCD cursor.
lcd.print(Upper_LCD_Line);                                                          // .
lcd.setCursor(0, 1);                                                                // Set LCD cursor.
lcd.print(Lower_LCD_Line);                                                          // .
}


// [FUNCTION] => Mode Restart
void Mode_Restart(){
Mode_Time_Indexer = 0; // Reset mode indexer.
lcd.clear();
//lcd.setCursor(0, 0);
//lcd.print("Mode: ");
//lcd.setCursor(6, 0);
//lcd.print(Mode);
//Serial.print("Mode: " );
//Serial.println(Mode);
}
