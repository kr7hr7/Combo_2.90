void settings()  {                                                       
    

  boardID               = "17";
  machineID             = "45";
  gateDelaySeconds      =  15;
 
  eepromUpdate          = "false";










  mqtt_server          = "192.168.7.168";
  mqtt_serverAlt       = "192.168.4.99";
  maxInterval          = 15000;  // this is the max between static pressure sample uploads - default 900000 = 15 minutes
  interval             = 500;       //  This variable sets the frequency of static pressure data being sampled
  staticDelta          = 100;    //   The ammount change required to trigger a mqtt sample
  ssid                 = "SDFWA";
  password             = "woodworking";
  ssidAlt              = "kr7hr8";
  passwordAlt          = "8584354826";
  trigger              = 400;  //A/D threshold from current sensor pin to determine tool-on condition
  triggerDelta         = 200;
  maxMissedSteps       = 2000; // the max number of steps that can be missed on homing before stopping the stepper
  delayTime            = 85;   // hold time between steps lower=faster higher=slower. 200-400 for 4988 drivers,  75-125 for TMC2209 drivers
  //GOOGLE_SCRIPT_ID     = "AKfycbzJ7wqIV0Y871usWsQ4I-cSjY12VithGFmC8DQ8NDvPOIHuyim5OgCcv1YrahxUaLiYUw";
  //Google_Script_Master = "AKfycbxt3ViqmoN6a3ZsLRBLeR80lRBOrx2rP3Jw6JYQK1qWLCCHJqc_n0wL4b9DX25XWP6K";
  GOOGLE_SCRIPT_Boot   = "https://script.google.com/macros/s/AKfycbw6yvrJNlk_fhMKMrYYJstpQlU0F_4UzOlHljUQJ1ZrX3WMtHGIj-ECYuk-GU-9L1iM/exec?";
 }
 
