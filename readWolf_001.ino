///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Most of this code comes from https://github.com/kneelo/Wolf-V500-dash.  Thank you.
//    I have made some modifications to suit my application.
//    Use a TTL to RS232 level converter; many from Ebay, some don't work.  More details in Wiki.
//
//    This code shows reading the ecu only.  Processing and displaying data subject to your requirements.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int rpm;
int kmh;
int temp_water = 0;
unsigned int press_oil = 0;
float volt = 0;
unsigned int percent_fuel = 0;

long currentMillis = 0;
long wolf1previousMillis = 0;
long wolf1interval = 100;
int wolf1update = 0;
//byte wolf1request[4] = {0x52, 0x00, 0x00, 0x1f};  //Wolf V500
byte wolf1request[4] = {0x51, 0x00, 0x00, 0x1f};  //Wolf V550

long wolf2previousMillis = 0;
long wolf2interval = 500;
int wolf2update = 0;

//byte wolf2request[4] = {0x52, 0x00, 0x40, 0x1f}; //Wolf V500
byte wolf2request[4] = {0x51, 0x00, 0x40, 0x1f}; //Wolf V550

//byte AuxLS_base = 8; // Wolf v500
byte AuxLS_base = 10; // Wolf v550

byte wolfdata [64];
byte readbyte = 0;
int wolfcounter = 0;
long wolfcount = 0; 

void setup() {
  
    Serial.begin(9600); //for serial debug
    Serial1.begin(38400); //serial connection to Wolf V500/V550 via RS232 plug

}

void loop() {
  
//---------------------------------Get data from wolf ecu---------------------------------------
      currentMillis = millis();
      if (Serial1.available() > 0 && wolf1update == 0 && wolf2update == 0) {
        while (Serial1.available() > 0)  {
          readbyte = Serial1.read(); 
        }
      }
      //send request for packet 0 data
      if ((currentMillis - wolf1previousMillis > wolf1interval) && wolf2update == 0 && wolfcount > 30) {  
        wolf1previousMillis = millis();
        wolf1update = 1;
        Serial1.write(wolf1request, 4);
      } 
       // Read packet 0 response from ECU
       if (Serial1.available() > 31 && wolf1update == 1)  {
        wolfcounter = 0; 
        while (Serial1.available() > 0) {
          readbyte = Serial1.read();
          wolfdata[wolfcounter] = readbyte;
          wolfcounter++; 
       }
    
         rpm = (wolfdata[0] * 256) + wolfdata[1]; //calculate RPM from hex data
         //load = wolfdata[2]; //calculate load from hex data
         //throttle = wolfdata[3] / 2.3923; //calculate throttle position from hex data
         //PSIboost = ((wolfdata[4] * 256.0) + wolfdata[5]) / 10.0; //calculate psi boost from hex data
         //injms = ((wolfdata[6] * 256.00) + wolfdata[7]) / 125.00; //calculate injection duration from hex data
         //injduty = wolfdata[10] / 2.52545; //calculate injector duty from hex data
         //ignadv = (wolfdata[11] / 2.845) - 11.243; //calculate ignition advance from hex data
         //manipressg = ((wolfdata[12] / 2.3935) * 2) - 100; //calculate MAP pressure from hex data
         //airtemp = wolfdata[14] - 64; //calculate intake air temp from hex data
         temp_water = wolfdata[15] - 64; //calculate coolant temp from hex data
         volt = wolfdata[16] / 10.0; //calculate voltage from hex data
         //afr = wolfdata[18] / 10.0; //calculate wideband afr from hex data
         //idlevlv = ((wolfdata[24] * 256) + wolfdata[25]) * 0.39216; //calculate idle valve position from hex data
        
          kmh = ((wolfdata[20] * 256) + wolfdata[21]);// Wolf Speed config page: speed/frequency1 calibration2 232000 (trial & error, function of pulses/km & skipped pulses,) 8 pulses/rev skip 1, 1992mm / rev, AUXLS1 input pin
    
         wolf1update = 0;
         wolfcounter = 0;
         wolfcount = 0;
      }
     
    
      if (Serial1.available() > 0 && wolf1update == 0 && wolf2update == 0) {
       while (Serial1.available() > 0) {
        readbyte = Serial1.read(); 
       }
      }
     
      //send request for packet 2 data
      if ((currentMillis - wolf2previousMillis > wolf2interval) && wolf1update == 0) {
       wolf2previousMillis = millis(); 
       wolf2update = 1;
       Serial1.write(wolf2request, 4);   
      }
    
     if (Serial1.available() > 31 && wolf2update == 1)  {
       wolfcounter = 0;
       while (Serial1.available() > 0) {
        readbyte = Serial1.read();
        wolfdata[wolfcounter] = readbyte; 
        wolfcounter++; 
       }
       wolf2update = 0;
       wolfcounter = 0;
       wolfcount = 0;

       if (wolfdata[12] < 4) {
          press_oil = ((wolfdata[AuxLS_base + 2] * 256) + wolfdata[AuxLS_base + 3])* 75.0 / 930.0;   // Read AuxLS2 pin & convert to PSI
       }
       if (wolfdata[14] < 4) {
          percent_fuel = ((wolfdata[AuxLS_base + 4] * 256) + wolfdata[AuxLS_base + 5])* 100.0 / 930.0;   // Read AuxLS3 pin & convert to %fuel
       }
     }
wolfcount++;

Serial.print(rpm);Serial.print("|");Serial.print(kmh);Serial.print("|");Serial.println(press_oil); // Apply out code here

}
