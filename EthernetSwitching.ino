/*

EthernetSwitching02

Switch relays from the ethernet or the internet, using your
mobile, tablet or computer with a nice graphical user interface
You need only a browser.

By tcvella
Xewkija Gozo, Zebbug Malta

Original sketch here
http://www.instructables.com/id/Ethernet-Switching-with-Arduino/?ALLSTEPS
This is not the updated version!
The original version works only on Mega because it takes a lot of RAM

Modifications by Zoomx
Al strings are in FLASH so it fit also Arduino UNO RAM
need to add the updates from here
https://www.dropbox.com/s/19rrxua51v9hhrz/WebServerSwitchingV04_06.ino
and here
https://www.dropbox.com/s/sbu3s2qh6274ieq/RELAYCONTROLWITHAUTH.ino
the last one is from drewpalmer04
*/


#include <Ethernet.h>
#include <SPI.h>
#include <EEPROM.h>

////////////////////////////////////////////////////////////////////////
//CONFIGURATION
////////////////////////////////////////////////////////////////////////

//IP manual settings
byte ip[] = {
  192, 168, 137, 100
}; //Manual setup only
byte gateway[] = {
  192, 168, 1, 1
}; //Manual setup only
byte subnet[] = {
  255, 255, 255, 0
}; //Manual setup only
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

//Ethernet Port
EthernetServer server = EthernetServer(80); //default html port 80

//The number of outputs going to be switched.
int outputQuantity = 12; //should not exceed 10

//Invert the output of the leds
boolean outputInverted = false; //true or false
// This is done in case the relay board triggers the relay on negative, rather then on positive supply

//Html page refresh
int refreshPage = 15; //default is 10sec.
//Beware that if you make it refresh too fast, the page could become inacessable.

//Display or hide the "Switch on all Pins" buttons at the bottom of page
int switchOnAllPinsButton = false; //true or false

int outputAddress[16] = {
  2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13, 13, 13, 13
}; //Allocate 10 spaces and name the output pin address.

String buttonText[16] = {
  "01. TEST01", "02. TEST02", "03. TEST03", "04. TEST04", "05. TEST05", "06. TEST06", "07. TEST07", "08. TEST08", "09. TEST09", "10. TEST10", "11. TEST11", "12. TEST12", "13. TEST13", "14. TEST14", "15. TEST15", "16. TEST16"
};

// Set the output to retain the last status after power recycle.
int retainOutputStatus[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //1-retain the last status. 0-will be off after power cut.

////////////////////////////////////////////////////////////////////////
//VARIABLES DECLARATION
////////////////////////////////////////////////////////////////////////

int outp = 0;
boolean printLastCommandOnce = false;
boolean printButtonMenuOnce = false;
boolean initialPrint = true;
String allOn = "";
String allOff = "";
boolean reading = false;
boolean outputStatus[16]; //Create a boolean array for the maximum ammount.

String rev = "V4.06b";

unsigned long timeConnectedAt;
boolean writeToEeprom = false;

/////////////////////////////////////////////////
// Temperature Related Reading
const int tempInPin = A1;
int tempInValue = 0; //temperature read
int tempScaleOutValue = 0; //temperature formatted
int tempOutValue = 0; //temperature formatted
float tempOutDeg = 0.0;

////////////////////////////////////////////////////////////////////////
//RUN ONCE
////////////////////////////////////////////////////////////////////////

//Beginning of Program
void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println(F("EthernetSwitching01"));
  delay(1000);
  Serial.println(F("initEepromValues"));
  initEepromValues();
  delay(1000);
  Serial.println(F("readEepromValues"));
  readEepromValues();
  delay(1000);
  //Set pins as Outputs
  boolean currentState = false;
  int var;
  for (int i = 0; i < outputQuantity; i++) {
    pinMode(outputAddress[i], OUTPUT);
    var = outputAddress[i];
    //Switch all outputs to either on or off on Startup
    if (outputInverted == true) {
      //digitalWrite(outputAddress[var], HIGH);
      if (outputStatus[i] == 0) {
        currentState = true;
      }
      else {
        currentState = false;
      } //check outputStatus if off, switch output accordingly
      digitalWrite(var, currentState);
    }
    else {
      //digitalWrite(outputAddress[var], LOW);
      if (outputStatus[i] == 0) {
        currentState = false;
      }
      else {
        currentState = true;
      }//check outputStatus if off, switch output accordingly
      digitalWrite(var, currentState);
    }
  }
  //Setting up the IP address. Comment out the one you dont need.
  //Ethernet.begin(mac); //for DHCP address. (Address will be printed to serial.)
  //Ethernet.begin(mac, ip, gateway, subnet); //for manual setup. (Address is the one configured above.)
  Serial.println(F("Trying to get an IP address using DHCP"));
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("Failed to configure Ethernet using DHCP"));
    // initialize the ethernet device not using DHCP:
    Ethernet.begin(mac, ip, gateway, subnet);
  }
  server.begin();
  Serial.print(F("Server started at "));
  Serial.println(Ethernet.localIP());

}

////////////////////////////////////////////////////////////////////////
//LOOP
////////////////////////////////////////////////////////////////////////
void loop() {
  //Read Temperature Sensor
  tempInValue = analogRead(tempInPin);
  // Connecting a 10K3 Thermistor to the Arduino Input
  // +5V �—————————[10Kohms]—————————[Thermistor]——� 0V
  // To Arduino IP �———————————|

  tempScaleOutValue = map(tempInValue, 0, 1023, 1023, 0); //Arduino value and NTC of the 10K3 Thermistor
  tempOutValue = map(tempScaleOutValue, 130, 870, -170, 730); //range of Arduino Value compared with Temperature
  tempOutValue = tempOutValue - 45; //Adjustments
  tempOutDeg = tempOutValue / 10.0;
  checkForClient();
}

////////////////////////////////////////////////////////////////////////
//checkForClient Function
////////////////////////////////////////////////////////////////////////
//
void checkForClient() {
  EthernetClient client = server.available();
  if (client) {
    boolean currentLineIsBlank = true;
    boolean sentHeader = false;
    int temp, temp1;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '*') {
          printHtmlHeader(client); //call for html header and css
          printLoginTitle(client);
          printHtmlFooter(client);
          break;
        }
        if (!sentHeader) {
          printHtmlHeader(client); //call for html header and css
          printHtmlButtonTitle(client); //print the button title
          sentHeader = true;
        }
        if (reading && c == ' ') {
          reading = false;
        }
        if (c == '?') {
          reading = true; //found the ?, begin reading the info
        }
        if (reading) {
          //if user input is H set output to 1
          if (c == 'H') {
            outp = 1;
          }
          if (c == 'L') {
            outp = 0;
          }
          // Serial.println(c); //print the value of c to serial communication
          //---------------------------------------------------------------------------------------------
          // ? H 1 0
          // ^ ^ ^ ^
          // | | | |____________read 4 ( 10,11,12,13....)
          // | | |______________read 3 ( 1....9)
          // | |________________read 2 if user input is H set output to L
          // |__________________read 1
          //---------------------------------------------------------------------------------------------
          if ( c == '1') {
            char c = client.read();
            switch (c) {
              case '0':
                triggerPin(outputAddress[10], client, outp);
                break;
              case '1':
                triggerPin(outputAddress[11], client, outp);
                break;
              case '2':
                triggerPin(outputAddress[12], client, outp);
                break;
              case '3':
                triggerPin(outputAddress[13], client, outp);
                break;
              case '4':
                triggerPin(outputAddress[14], client, outp);
                break;
              case '5':
                triggerPin(outputAddress[15], client, outp);
                break;
              default:
                char c = client.read();
                triggerPin(outputAddress[1], client, outp);
            }
          }
          else {
            switch (c) {
              case '0':
                triggerPin(outputAddress[0], client, outp);
                break;
              // case '1':
              // triggerPin(outputAddress[1], client, outp);
              // break;
              case '2':
                triggerPin(outputAddress[2], client, outp);
                break;
              case '3':
                //add code here to trigger on 3
                triggerPin(outputAddress[3], client, outp);
                break;
              case '4':
                //add code here to trigger on 4
                triggerPin(outputAddress[4], client, outp);
                break;
              case '5':
                //add code here to trigger on 5
                triggerPin(outputAddress[5], client, outp);
                //printHtml(client);
                break;
              case '6':
                //add code here to trigger on 6
                triggerPin(outputAddress[6], client, outp);
                break;
              case '7':
                //add code here to trigger on 7
                triggerPin(outputAddress[7], client, outp);
                break;
              case '8':
                //add code here to trigger on 8
                triggerPin(outputAddress[8], client, outp);
                break;
              case '9':
                //add code here to trigger on 9
                triggerPin(outputAddress[9], client, outp);
                break;
            } //end of switch case
          }
        }//end of switch switch the relevant output
        //if user input was blank
        if (c == '\n' && currentLineIsBlank) {
          printLastCommandOnce = true;
          printButtonMenuOnce = true;
          triggerPin(777, client, outp); //Call to read input and print menu. 777 is used not to update any outputs
          break;
        }
      }
    }
    printHtmlFooter(client); //Prints the html footer
  }
  else {
    if (millis() > (timeConnectedAt + 60000)) {
      if (writeToEeprom == true) {
        writeEepromValues(); //write to EEprom the current output statuses
        Serial.println(F("No Clients for more then a minute - Writing statuses to Eeprom."));
        writeToEeprom = false;
      }
    }
  }
}// END
////////////////////////////////////////////////////////////////////////
//triggerPin Function
////////////////////////////////////////////////////////////////////////
//
void triggerPin(int pin, EthernetClient client, int outp) {
  if (pin != 777) {
    // Serial.println(pin);
    if (outp == 1) {
      if (outputInverted == false) {
        digitalWrite(pin, HIGH);
      }
      else {
        digitalWrite(pin, LOW);
      }
    }
    if (outp == 0) {
      if (outputInverted == false) {
        digitalWrite(pin, LOW);
      }
      else {
        digitalWrite(pin, HIGH);
      }
    }
  }
  //Refresh the reading of outputs
  readOutputStatuses();
  //Prints the buttons
  if (printButtonMenuOnce == true) {
    printHtmlButtons(client);
    printButtonMenuOnce = false;
  }
}
////////////////////////////////////////////////////////////////////////
//printHtmlButtons Function
////////////////////////////////////////////////////////////////////////
//print the html buttons to switch on/off channels
void printHtmlButtons(EthernetClient client) {
  //Start to create the html table
  client.println(F(""));
  //client.println(F("<p>"));
  client.println(F("<FORM>"));
  client.println(F("<table border=\"0\" align=\"center\">"));
  //Printing the Temperature
  client.print(F("<tr>\n"));
  client.print(F("<td><h4>"));
  client.print(F("Temperature"));
  client.print(F("</h4></td>\n"));
  client.print(F("<td></td>"));
  client.print(F("<td>"));
  client.print(F("<h3>"));
  client.print(tempOutDeg);
  client.print(F(" °C</h3></td>\n"));
  client.print(F("<td></td>"));
  client.print(F("</tr>"));
  //Start printing button by button
  for (int var = 0; var < outputQuantity; var++) {
    //set command for all on/off
    allOn += "H";
    allOn += outputAddress[var];
    allOff += "L";
    allOff += outputAddress[var];
    //Print begining of row
    client.print(F("<tr>\n"));
    //Prints the button Text
    client.print(F("<td><h4>"));
    client.print(buttonText[var]);
    client.print(F("</h4></td>\n"));
    //Prints the ON Buttons+++++++++++++++++++++++++++++++++++++++++++++++
    client.print(F("<td>"));
    client.print(F("<INPUT TYPE=\"button\" VALUE=\"ON "));
    client.print(F("\" onClick=\"parent.location='/?H"));
    client.print(var);
    client.print(F("'\"></td>\n"));
    //Prints the OFF Buttons ---------------------------------------------
    client.print(F(" <td><INPUT TYPE=\"button\" VALUE=\"OFF"));
    client.print(F("\" onClick=\"parent.location='/?L"));
    client.print(var);
    client.print(F("'\"></td>\n"));
    //Invert the LED display if output is inverted.
    if (outputStatus[var] == true ) { //If Output is ON
      if (outputInverted == false) { //and if output is not inverted
        client.print(F(" <td><div class='green-circle'><div class='glare'></div></div></td>\n")); //Print html for ON LED
      }
      else { //else output is inverted then
        client.print(F(" <td><div class='black-circle'><div class='glare'></div></div></td>\n")); //Print html for OFF LED
      }
    }
    else //If Output is Off
    {
      if (outputInverted == false) { //and if output is not inverted
        client.print(F(" <td><div class='black-circle'><div class='glare'></div></div></td>\n")); //Print html for OFF LED
      }
      else { //else output is inverted then
        client.print(F(" <td><div class='green-circle'><div class='glare'></div></div></td>\n")); //Print html for ON LED
      }
    }
    //Print end of row
    client.print(F("</tr>\n"));
  }
  //Display or hide the Print all on Pins Button
  if (switchOnAllPinsButton == true ) {
    //Prints the ON All Pins Button
    client.print(F("<tr>\n<td><INPUT TYPE=\"button\" VALUE=\"Switch ON All Pins"));
    client.print(F("\" onClick=\"parent.location='/?"));
    client.print(allOn);
    client.print(F("'\"></td>\n"));
    //Prints the OFF All Pins Button
    client.print(F("<td><INPUT TYPE=\"button\" VALUE=\"Switch OFF All Pins"));
    client.print(F("\" onClick=\"parent.location='/?"));
    client.print(allOff);
    client.print(F("'\"></td>\n<td></td>\n<td></td>\n</tr>\n"));
  }
  //Closing the table and form
  client.println(F("</table>"));
  client.println(F("</FORM>"));
  //client.println(F("</p>"));
}
////////////////////////////////////////////////////////////////////////
//readOutputStatuses Function
////////////////////////////////////////////////////////////////////////
//Reading the Output Statuses
void readOutputStatuses() {
  for (int var = 0; var < outputQuantity; var++) {
    outputStatus[var] = digitalRead(outputAddress[var]);
    //Serial.print(outputStatus[var]);
  }
}
////////////////////////////////////////////////////////////////////////
//readEepromValues Function
////////////////////////////////////////////////////////////////////////
//Read EEprom values and save to outputStatus
void readEepromValues() {
  for (int adr = 0; adr < outputQuantity; adr++) {
    outputStatus[adr] = EEPROM.read(adr);
  }
}
////////////////////////////////////////////////////////////////////////
//writeEepromValues Function
////////////////////////////////////////////////////////////////////////
//Write EEprom values
void writeEepromValues() {
  for (int adr = 0; adr < outputQuantity; adr++) {
    EEPROM.write(adr, outputStatus[adr]);
  }
}
////////////////////////////////////////////////////////////////////////
//initEepromValues Function
////////////////////////////////////////////////////////////////////////
//Initialiaze EEprom values
//if eeprom values are not the correct format ie not euqual to 0 or 1 (thus greater then 1) initialize by putting 0
void initEepromValues() {
  for (int adr = 0; adr < outputQuantity; adr++) {
    if (EEPROM.read(adr) > 1) {
      EEPROM.write(adr, 0);
    }
  }
}
////////////////////////////////////////////////////////////////////////
//htmlHeader Function
////////////////////////////////////////////////////////////////////////
//Prints html header
void printHtmlHeader(EthernetClient client) {
  // Serial.print(F("Serving html Headers at ms -"));
  timeConnectedAt = millis(); //Record the time when last page was served.
  // Serial.print(timeConnectedAt); // Print time for debbugging purposes
  writeToEeprom = true; // page loaded so set to action the write to eeprom
  // send a standard http response header
  client.println(F("HTTP/1.1 200 OK"));
  client.println(F("Content-Type: text/html"));
  client.println(F("Connnection: close"));
  client.println();
  client.println(F("<!DOCTYPE HTML>"));
  client.println(F("<head>"));
  // add page title
  client.println(F("<title>Ethernet Switching</title>"));
  client.println(F("<meta name=\"description\" content=\"Ethernet Switching\"/>"));
  // add a meta refresh tag, so the browser pulls again every x seconds:
  client.print(F("<meta http-equiv=\"refresh\" content=\""));
  client.print(refreshPage);
  client.println(F("; url=/\">"));
  // add other browser configuration
  client.println(F("<meta name=\"apple-mobile-web-app-capable\" content=\"yes\">"));
  client.println(F("<meta name=\"apple-mobile-web-app-status-bar-style\" content=\"default\">"));
  client.println(F("<meta name=\"viewport\" content=\"width=device-width, user-scalable=no\">"));
  //inserting the styles data, usually found in CSS files.
  client.println(F("<style type=\"text/css\">"));
  client.println(F(""));
  //This will set how the page will look graphically
  client.println(F("html { height:100%; }"));
  client.println(F(" body {"));
  client.println(F(" height: 100%;"));
  client.println(F(" margin: 0;"));
  client.println(F(" font-family: helvetica, sans-serif;"));
  client.println(F(" -webkit-text-size-adjust: none;"));
  client.println(F(" }"));
  client.println(F(""));
  client.println(F("body {"));
  client.println(F(" -webkit-background-size: 100% 21px;"));
  client.println(F(" background-color: #c5ccd3;"));
  client.println(F(" background-image:"));
  client.println(F(" -webkit-gradient(linear, left top, right top,"));
  client.println(F(" color-stop(.75, transparent),"));
  client.println(F(" color-stop(.75, rgba(255,255,255,.1)) );"));
  client.println(F(" -webkit-background-size: 7px;"));
  client.println(F(" }"));
  client.println(F(""));
  client.println(F(".view {"));
  client.println(F(" min-height: 100%;"));
  client.println(F(" overflow: auto;"));
  client.println(F(" }"));
  client.println(F(""));
  client.println(F(".header-wrapper {"));
  client.println(F(" height: 44px;"));
  client.println(F(" font-weight: bold;"));
  client.println(F(" text-shadow: rgba(0,0,0,0.7) 0 -1px 0;"));
  client.println(F(" border-top: solid 1px rgba(255,255,255,0.6);"));
  client.println(F(" border-bottom: solid 1px rgba(0,0,0,0.6);"));
  client.println(F(" color: #fff;"));
  client.println(F(" background-color: #8195af;"));
  client.println(F(" background-image:"));
  client.println(F(" -webkit-gradient(linear, left top, left bottom,"));
  client.println(F(" from(rgba(255,255,255,.4)),"));
  client.println(F(" to(rgba(255,255,255,.05)) ),"));
  client.println(F(" -webkit-gradient(linear, left top, left bottom,"));
  client.println(F(" from(transparent),"));
  client.println(F(" to(rgba(0,0,64,.1)) );"));
  client.println(F(" background-repeat: no-repeat;"));
  client.println(F(" background-position: top left, bottom left;"));
  client.println(F(" -webkit-background-size: 100% 21px, 100% 22px;"));
  client.println(F(" -webkit-box-sizing: border-box;"));
  client.println(F(" }"));
  client.println(F(""));
  client.println(F(".header-wrapper h1 {"));
  client.println(F(" text-align: center;"));
  client.println(F(" font-size: 20px;"));
  client.println(F(" line-height: 44px;"));
  client.println(F(" margin: 0;"));
  client.println(F(" }"));
  client.println(F(""));
  client.println(F(".group-wrapper {"));
  client.println(F(" margin: 9px;"));
  client.println(F(" }"));
  client.println(F(""));
  client.println(F(".group-wrapper h2 {"));
  client.println(F(" color: #4c566c;"));
  client.println(F(" font-size: 17px;"));
  client.println(F(" line-height: 0.8;"));
  client.println(F(" font-weight: bold;"));
  client.println(F(" text-shadow: #fff 0 1px 0;"));
  client.println(F(" margin: 20px 10px 12px;"));
  client.println(F(" }"));
  client.println(F(""));
  client.println(F(".group-wrapper h3 {"));
  client.println(F(" color: #4c566c;"));
  client.println(F(" font-size: 12px;"));
  client.println(F(" line-height: 1;"));
  client.println(F(" font-weight: bold;"));
  client.println(F(" text-shadow: #fff 0 1px 0;"));
  client.println(F(" margin: 20px 10px 12px;"));
  client.println(F(" }"));
  client.println(F(""));
  client.println(F(".group-wrapper h4 {")); //Text for description
  client.println(F(" color: #212121;"));
  client.println(F(" font-size: 14px;"));
  client.println(F(" line-height: 1;"));
  client.println(F(" font-weight: bold;"));
  client.println(F(" text-shadow: #aaa 1px 1px 3px;"));
  client.println(F(" margin: 5px 5px 5px;"));
  client.println(F(" }"));
  client.println(F(""));
  client.println(F(".group-wrapper table {"));
  client.println(F(" background-color: #fff;"));
  client.println(F(" -webkit-border-radius: 10px;"));
  client.println(F(" -moz-border-radius: 10px;"));
  client.println(F(" -khtml-border-radius: 10px;"));
  client.println(F(" border-radius: 10px;"));
  client.println(F(" font-size: 17px;"));
  client.println(F(" line-height: 20px;"));
  client.println(F(" margin: 9px 0 20px;"));
  client.println(F(" border: solid 1px #a9abae;"));
  client.println(F(" padding: 11px 3px 12px 3px;"));
  client.println(F(" margin-left:auto;"));
  client.println(F(" margin-right:auto;"));
  client.println(F(" -moz-transform :scale(1);")); //Code for Mozilla Firefox
  client.println(F(" -moz-transform-origin: 0 0;"));
  client.println(F(" }"));
  client.println(F(""));
  //how the green (ON) LED will look
  client.println(F(".green-circle {"));
  client.println(F(" display: block;"));
  client.println(F(" height: 23px;"));
  client.println(F(" width: 23px;"));
  client.println(F(" background-color: #0f0;"));
  //client.println(F(" background-color: rgba(60, 132, 198, 0.8);"));
  client.println(F(" -moz-border-radius: 11px;"));
  client.println(F(" -webkit-border-radius: 11px;"));
  client.println(F(" -khtml-border-radius: 11px;"));
  client.println(F(" border-radius: 11px;"));
  client.println(F(" margin-left: 1px;"));
  client.println(F(" background-image: -webkit-gradient(linear, 0% 0%, 0% 90%, from(rgba(46, 184, 0, 0.8)), to(rgba(148, 255, 112, .9)));@"));
  client.println(F(" border: 2px solid #ccc;"));
  client.println(F(" -webkit-box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px;"));
  client.println(F(" -moz-box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px; /* FF 3.5+ */"));
  client.println(F(" box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px; /* FF 3.5+ */"));
  client.println(F(" }"));
  client.println(F(""));
  //how the black (off)LED will look
  client.println(F(".black-circle {"));
  client.println(F(" display: block;"));
  client.println(F(" height: 23px;"));
  client.println(F(" width: 23px;"));
  client.println(F(" background-color: #040;"));
  client.println(F(" -moz-border-radius: 11px;"));
  client.println(F(" -webkit-border-radius: 11px;"));
  client.println(F(" -khtml-border-radius: 11px;"));
  client.println(F(" border-radius: 11px;"));
  client.println(F(" margin-left: 1px;"));
  client.println(F(" -webkit-box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px;"));
  client.println(F(" -moz-box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px; /* FF 3.5+ */"));
  client.println(F(" box-shadow: rgba(11, 140, 27, 0.5) 0px 10px 16px; /* FF 3.5+ */"));
  client.println(F(" }"));
  client.println(F(""));
  //this will add the glare to both of the LEDs
  client.println(F(" .glare {"));
  client.println(F(" position: relative;"));
  client.println(F(" top: 1;"));
  client.println(F(" left: 5px;"));
  client.println(F(" -webkit-border-radius: 10px;"));
  client.println(F(" -moz-border-radius: 10px;"));
  client.println(F(" -khtml-border-radius: 10px;"));
  client.println(F(" border-radius: 10px;"));
  client.println(F(" height: 1px;"));
  client.println(F(" width: 13px;"));
  client.println(F(" padding: 5px 0;"));
  client.println(F(" background-color: rgba(200, 200, 200, 0.25);"));
  client.println(F(" background-image: -webkit-gradient(linear, 0% 0%, 0% 95%, from(rgba(255, 255, 255, 0.7)), to(rgba(255, 255, 255, 0)));"));
  client.println(F(" }"));
  client.println(F(""));
  //and finally this is the end of the style data and header
  client.println(F("</style>"));
  client.println(F("</head>"));
  //now printing the page itself
  client.println(F("<body>"));
  client.println(F("<div class=\"view\">"));
  client.println(F(" <div class=\"header-wrapper\">"));
  client.println(F(" <h1>Ethernet Switching</h1>"));
  client.println(F(" </div>"));
  //////
} //end of htmlHeader
////////////////////////////////////////////////////////////////////////
//htmlFooter Function
////////////////////////////////////////////////////////////////////////
//Prints html footer
void printHtmlFooter(EthernetClient client) {
  //Set Variables Before Exiting
  printLastCommandOnce = false;
  printButtonMenuOnce = false;
  allOn = "";
  allOff = "";
  //printing last part of the html
  client.println(F("\n<h3 align=\"center\">Arduino porting - Zoomx <br> 12 - February - 2015 - V5.0"));
  client.println(F("\n<h3 align=\"center\">Development - Chinh Truc <br> 01 - August - 2014 - V5.0"));
  client.println(F("\n<h3 align=\"center\">© Author - Claudio Vella <br> Malta - October - 2012 - "));
  client.println(rev);
  client.println(F("</h3></div>\n</div>\n</body>\n</html>"));
  delay(1); // give the web browser time to receive the data
  client.stop(); // close the connection:
  Serial.println(F(" - Done, Closing Connection."));
  delay (2); //delay so that it will give time for client buffer to clear and does not repeat multiple pages.
} //end of htmlFooter
////////////////////////////////////////////////////////////////////////
//printHtmlButtonTitle Function
////////////////////////////////////////////////////////////////////////
//Prints html button title
void printHtmlButtonTitle(EthernetClient client) {
  client.println(F("<div class=\"group-wrapper\">"));
  client.println(F(" <h2>Switch Device output.</h2>"));
  client.println();
}
////////////////////////////////////////////////////////////////////////
//printLoginTitle Function
////////////////////////////////////////////////////////////////////////
//Prints html button title
void printLoginTitle(EthernetClient client) {
  // client.println(F("<div class=\"group-wrapper\">"));
  client.println(F(" <h2>Please enter the user data to login.</h2>"));
  client.println();
}

