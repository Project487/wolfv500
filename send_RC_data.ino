//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Code to enable Wolf V500/V550 data captured from ecu to be transmitted to an Android device running RaceChrono app.
//
//  Loop code should be placed in the main Wolf loop after data obtained.  
//
//  Checksum subroutine is place outside main loop
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint16_t RCcount=0;
long prevRCMillis = 0;
long RCinterval = 50;   //send RaceChrono Bluetooth message to Android device every interval (milliseconds) 

void setup() {
  
  Serial2.begin(9600);  //Open serial2 on Mega 2560 to send to HC05 Bluetooth transmitter

}

void loop() {
 if (currentMillis - prevRCMillis > RCinterval) {     
          // Put it into RaceChrono format. [d] are digital and [a] are analog outputs. Begin with $RC2 and end with *.  
 
          String str = String("$RC2,,") + String(RCcount++) + ",,,," + String(rpm) + "," + String(brake_on) + ",";  // $RC2,[time],[count],[xacc],[yacc],[zacc],[rpm/d1],[d2],
          str += String(kmh) + "," + String(temp_water) + "," + String(press_oil) + ",,,,,,*"; //[a1],[a2],[a3],[a4],[a5],[a6],[a7],[a8]* 
          
          // Send it via serial line and Bluetooth to RaceChrono on your Phone, must end with a checksum
          Serial2 .println(str + checksum( str ));
          prevRCMillis = currentMillis;
     }


}

String checksum(String s)   // Checksum calculation for RaceChrono GPS
{
  byte cs=0x00;
  char *buf=s.c_str();
  for (unsigned int i = 1; i < s.length()-1; i++) cs = cs ^ buf[i];   // XOR 
  return String((cs <= 0x0f ? "0" : "") + String(cs, HEX));
}
