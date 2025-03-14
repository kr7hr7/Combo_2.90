/*-------------------------------------------------------------------

    Stepper Control Mac Version
    Dallas Keck
    ESP32 Based Controller
                                                                                                  */
char* ver = "Combo_2.90";                                                                        /*
  

  2.90     01.09.25 EEPROM function is solid, setup error codes and OTA recovery
  2.82     01.09.25 table  to auto match gateType to machine ID COrrected several bugs in EEPROM functions
  2.80     01.07.25 EEPROM is working
  2.733    01.06.25 Yet more work on the EEPROM --Its close
  2.732    01.06.25 Continue working on EEPROM Focus on write function
  2.731    01.05.25 working on EEPROM function.  Thats not working yet
  2.73     01.04.25 Fixed bug assigning values to the tools array
  2.72     01.01.25 Rewrote boardConfig function
  2.71     01.01 25 Simplifying the settings using EEPROM
  2.70     12.31.24 Production Version
  2.696    12.30.24 Abandoned 2.695  New fork starting with 2.694
  2.694    12.28.24 fork from 2.692 skip 2.693
  2.692    12.25.24 PRODUCTION!   Abandoned 2.691 and started new debug
  2.691    12.25.24 debugging errors when running v5 boards
  2.69     12.19.24 Production --  Incorporates all changes below.
  2.683    12.19.24 Fixing bugs on the manual configuation
  2.681    12.18.24 Testing methods to fix the issues with data upload to the Google Sheets boot log sheet
  2.68     12.16.24 Added "P" configuration.  USed to monitor useage by tool that dont have a blast Gate.
  2.676    12.11.24 Skipped back to 2.673, added setup for v11 board created variables for EEPROM
  2.673    12.08.24 Added auto logon to kr7hr8
  2.672    11.28.24 Corrected several bugs. Added gateTypes C&D. These keep the DC on during countdown to allow for cleanup
  2.651    11.09.24 Moved the Loop to a tab
  2.65     11.08.24 Changed the "S" close rotine to keep the relay closed during countdown
  2.62     11.01.24 Added Pressure Sensor Configuation "S"
  2.60     10.12.24 Remaking Mac2.54 into the combo software for all controllers
  2.54     10.01.24 Cleaned up the declarations to be more readable
  2.52     09.01.24 in setup set sensor in to pin 32 for boards >=32
  2.51f    08.16.24
  2.51e    08.02.24 Re-added tabs to keep the sketch managable
  2.51e    08.02.24 Added pin 18 as an output to the relay.  It accomdates v8+ boards.
  2.51     07.24.34 Cleaned up some bugs and readied for production
  2.50.04  07.23.24 Deleted the upload to Google Sheets on gate close
  2.50.03  07.23.24 Added bootlog function
  2.50.02  05.16.24 Removed the tabs to create a main progran to work with VS Code
  2.50.01  05.04.24 Write to Google sequenced within the close countdown
  2.49     04 20 24 Added gateType to simplify config and cleaned up some unused variables
  2.48a    03.23.24 added OTA handle in the reset function which cycles every 5 seconds
  2.48     03.20.24 added gateState as the MQTT ping payload
  2.47     03.18.24 Corrected issues with the reset function
  2.46     03.17.24 Added Max missed steps for homing & moved tools declarations to the main tab
  2.45     03.16.24 Added reset funstionallity
  2.44     03.13.24 Additional Cosmedic corrections
  2.43     03.13.24 diabled the stepper when in error state
  2.42     03.6.24 Added LWT functionality rest the steps back to 23,000
  2.41     03.01.24 reduced steps to 22,000 and created test for homing errors.
  2.40     More mqtt. Installed successful mqtt functions from P0.041
  2.36     Continue debugging MQTT and WiFi persistance 1/18/24
  2.35     more debugging on MQTT and WiFi persistance
  2.34     continue to debug MQTT
  2.32     Working on getting MQTT timeout isues corrected
  2.31     Corrected even more data logging issues
  2.30     Corrected issues with Google data logging.  It seems to be working well.
  2.25     Corrected several bugs MQTT and Google Sheets disabled
  2.20     Bundled all the recent upgrades for production
  2.16     Automated the naming convention in the settings tab
  2.15     Revised the display layout
  2.142    started with 2.11 then added MQTT componants
  2.11     Created a Keep WiFi Alive function.  It contuously monitors WiFi and reconnects when needed
  2.02     Simplified the configuation process.   Settings are assigned by boardID.
  2.01:    (Includes the short lived 2.00 version)
           Added a tab "Settings" in simplify the configration process.
           Removed the SPFF files and auto config function.   The configuration info is entered on the
           settings tab prior to loading the firmware.   OTA update
  1.50:    Status relay to toggle as soon as power is detected and go low as soon as power is off.
  1.37:    Changes the status reed relay toogle to change once the tool is off rather than after the
           tool is off plus a delay.
  1.352    Added quarter-step capability
  1.351    Reduced the speed to increase torgue
  1.35:    Created flag (combo) to identify early verion boards to automatically assign pins
  1.32:
           Fixed bug with the link function: renamed triggerPin to linkPin
           Enable OTA updates
           Added HOST Name to WiFi

  --------------------------------------------------------------------*/



#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>`
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <HTTPClient.h>
#include <math.h>
#include <EEPROM.h>

void closeGate();
void homePosition();
void openGate();
void checkSwitchState();
void printStat();
void displayStat();
void errorState();
void settings();
void prepNames();
void MQTTconnect();
void reportStaticPressue();
void keepMqttAlive(void* parameters);
void checkSwitchState();
void pingBroker(void* parameters);
void keepWiFiAlive(void* parameters);
void printStat();
void displayStat();
void writeToBootLog();
//void write_to_google_sheet();
void reconnect();
void resetGate(void* parameters);
void tools();
void loop();
void closeGate();
void openGate();
void homePosition();
void setupError();
void boardconfiguration();
void openGate();
void toolOn();
void toolOff();
void manualGateOpen();
void manualGateClose();
void eepromWrite();
void gatePinConfig();
void gateTypeConfig();
void OTA();
void WiFiConnect();

WiFiClient espClient;
PubSubClient client(espClient);

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET 17     // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
#define WiFiTimeOutMS 5000
#define EEPROM_SIZE 20 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//                                     Pin Assignements
int       ANALOG_PIN_IN   = A0;
byte      flash;                 // 200 = flag that the board data has been stored
byte      boardIdByte;           // Integer to store in EEPROM
byte      boardConfig;           // Character to store in EEPROM
// 1= mac style stepper
// 2= Manual Gate
// 3= CNC & Cleanup Stepper
// 4= Presure Sensor Monitor
// 5= No gate control. Data logging only
byte      boardVer;
byte      analogPinInByte;
byte      limitSwitchPin       = 19;
byte      reedRelayPin         = 23;
byte      greenLEDpin          = 25;
byte      redLEDpin            = 26;
byte      gateOn               = 16;  // Controls H-Bridge
byte      gateOff              = 04;  // Controls H-Bridge
byte      linkPin              = 13;  // use pin 33 for ver 1-3   Use pin 13 for v4 plus
byte      stepPin              = 16;  // 16 for combo boards   04 for earlier versions
byte      dirPin               = 04;  // 04 for combo boards   02 for earlier versions
byte      enablePin            = 17;  // 17 for combo boards  16 for early versions

//------- MQTTconnect variables-------------------
int       toolInt;
char      checkInID[30];  //  mID + " Checking In!" ;
int       mIDlen;
char      machineIDChar[3];
char      mID[4];  // The 3 digit machine Id   i.e. "M21"
String    mIDstring;
char      gateName[50];
char*     BGstatus;
String    gateNameString;
String    gateTypeString;
int       keepAlive = 70;
char      availabilityTopic[50];
char      BGtopic[40];
String    machineIDString;
String    setupID;
char*     gateState;
bool      rotation;  // To accomodate Rear or Front mount versions.  Mac(rear mounted) =false  Front mount(near the gate)=true
String    urlFinal;
bool      eepromUpdate = false;   // flag used to trigger EEPROM update 

//     Working Variables
int           sensorIn = 0;
int           sensor = 0;
int           stepPosition = 103;
String        trace = "test";
int           countDown;
String        toolString;
String        ipa;  //IP address;
String        verString;
String        machineIDstring;
int           fullRunSteps;
byte          eCode                 = 0;

//     Paramenters
const int     waitTime = 250;
const int     backSteps = 50;  //steps required to open switch after homing
const int     stepsPerRevolution = 200;
const int     holdTime = 500;  // time to stablize each cycle (microseconds)
const int     displayTime = 000;
const int     timeout = 120;

unsigned long closeDelayTime = 60;
unsigned long currentTime;
unsigned long gateOpenTime;
unsigned long startTime;
unsigned long closeTime;
unsigned long x;
unsigned      runTime = 999;
unsigned long currentMillis;
unsigned long lastMsgTime;
unsigned int  interval;
unsigned int  maxInterval;
unsigned int  staticDelta;
int           lastSample;
float         staticPressure;
float         inchesH2O;
String        inchesH2O_str;
char          buffer[10];
int           delta;

//Status Flags
volatile bool manualState;
volatile bool limitSwitchState;
volatile bool gateCloseState;
volatile bool gateOpenState;
volatile bool moveState;
volatile bool otaOn =false ;
int           manualStatus = 0;
int           flag = 0;

//Data
long          toolRunTime = 0;
long          toolTimer = 0;
int           toolCycles = 0;
int           loopCount = 0;

String        boardID;
int           intboardID;
String        machineID;
int           gateDelaySeconds;
//const char* gateType;
//char*         gateType;
String        gateType;
const char*   mqtt_server;
const char*   mqtt_serverAlt;
const char*   ssid;
const char*   password;
const char*   ssidAlt;
const char*   passwordAlt;
int           trigger;  //A/D threshold from current sensor pin to determine tool-on condition
int           triggerDelta;
int           maxMissedSteps;   // the max number of steps that can be missed on homing before stopping the stepper
int           delayTime = 125;  // hold time between steps lower=faster higher=slower. 200-400 for 4988 drivers,  75-125 for TMC2209 drivers
String        GOOGLE_SCRIPT_ID;
String        Google_Script_Master;
String        GOOGLE_SCRIPT_Boot;
