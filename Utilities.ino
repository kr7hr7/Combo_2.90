//----------------------------------------
void prepNames() {

  //           --   BG Topic  --
  String BGtopicString = "home-assistant/blastgates/M" + machineID + "/state";
  for (int i = 0; i < BGtopicString.length(); i++) {
    BGtopic[i] = BGtopicString[i];
  }
  BGtopic[BGtopicString.length()] = '\0';  // Null-terminate the char array
  //Serial.println(BGtopic);

  //           --   Availability Topic  --
  String availabilityTopicString = "home-assistant/blastgates/M" + machineID + "/availability";
  for (int i = 0; i < availabilityTopicString.length(); i++) {
    availabilityTopic[i] = availabilityTopicString[i];
  }
  availabilityTopic[availabilityTopicString.length()] = '\0';  // Null-terminate the char array
  //Serial.println(availabilityTopic);

  //        --  Setup Identification Board ID + M + Machine ID  -----

  machineIDstring = String(machineID);
  setupID = (boardID + "M" + machineID);
  //Serial.print("setupID= ");
  //Serial.print(setupID);

  //tools();
  String gateNameString;
  int toolInt;

  Serial.print("  Tool Name= ");
  Serial.print(toolString);
  gateNameString = (setupID + "_" + toolString);
  const char* gateNameChar = gateNameString.c_str();
  strcpy(gateName, gateNameChar);
  Serial.print("  OTA gateName = ");
  Serial.println(gateNameString);

  if (EEPROM.read(3) == 200) {
    gateNameString = (setupID + "_" + toolString + "_F_" ) ;
  }
  const char* gateNameChar2 = gateNameString.c_str();
  strcpy(gateName, gateNameChar2);
  Serial.print("  gateNameString = ");
  Serial.println(gateNameString);


  verString = String(ver);

  //        --  Setup MQTT Logon Variables  -----

  //convert machineID to char variable for MQTT logon

  mIDlen = machineID.length();
  //machineID.toCharArray(machineIDChar, (mIDlen + 1));
  machineID.toCharArray(machineIDChar, 3);
  mIDstring = "M" + machineID;
  //mIDstring.toCharArray(mID, sizeof(mID));
  mIDstring.toCharArray(mID, 4);


  const char* mqttName = mID;
  const char* mqttUser = machineIDChar;
  const char* mqttTopic = BGtopic;
}
//--------------------------------------------------------------------------
void reportStaticPressue() {

  Serial.print("reportStaticPressue:  ");
  //client.publish ("home-assistant/blastgates", "Checking In");
  client.publish(availabilityTopic, "online");
  client.publish(BGtopic, gateState, false);
  Serial.println(sensorIn);
}

//--------------------------------------------------------------------------
void keepMqttAlive(void* parameters) {

  const char* mqttName = mID;
  const char* mqttUser = machineIDChar;

  for (;;) {

    //Serial.println("keepMqttAlive Line 72");
    if (client.connected()) {
      //Serial.println(" keepMqttAlive  MQTT Connected");
    }
    if (!client.connected()) {
      Serial.println("MQTT Not Connected!  Attempting to connect to broker");
      client.connect(mqttName, mqttUser, "LetBGin!", availabilityTopic, 0, true, "offline");
      delay(2000);

      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("MQTT Re-Connected");
        displayStat();
      }
    }
    vTaskDelay(60000 / portTICK_PERIOD_MS);  // wait 1 minutes before checking the status again
  }
}


//------------------------------------------------------------------
void MQTTconnect() {
  const char* mqttName = mID;
  const char* mqttUser = machineIDChar;
  client.connect(mqttName, mqttUser, "LetBGin!", availabilityTopic, 0, true, "offline");
  delay(2000);
  if (client.connected()) {
    Serial.println("MQTT Connected!");
    client.publish(availabilityTopic, "online");
  }

  if (!client.connected()) {
    Serial.println("MQTT  NOT  Connected!");
  }
}

//------------------------------------------------------------------------------
void checkSwitchState() {

  ArduinoOTA.handle();
  if (digitalRead(limitSwitchPin) == HIGH) {
    limitSwitchState = false;
    gateCloseState = false;
  }

  else {
    limitSwitchState = true;
    gateOpenState = false;
    gateCloseState = true;
    digitalWrite(greenLEDpin, LOW);
    digitalWrite(redLEDpin, HIGH);
  }
}
//--------------------------------------------------------------------------
void pingBroker(void* parameters) {

  for (;;) {
    if (moveState == false)
      ;
    {
      //Serial.println("Ping The Mosquitto Broker");
      //client.publish ("home-assistant/blastgates", "Checking In");
      client.publish(availabilityTopic, "online");
      client.publish(BGtopic, gateState, false);
      vTaskDelay(60000 / portTICK_PERIOD_MS);  //wait one minute before checking the status again
    }
  }
}

//--------------------------------------------------------------------------
void keepWiFiAlive(void* parameters) {

  for (;;) {
    //Serial.print("keepWiFiAlive");
    if (WiFi.status() == WL_CONNECTED) {
      // Serial.println("Connected");
    }
    if (WiFi.status() != WL_CONNECTED) {
      unsigned long startAttempTime = millis();
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      Serial.println("Attempting to connect");
      delay(2000);

      if (WiFi.status() == WL_CONNECTED) {
        Serial.print("WiFi connected  IP Address= ");
        Serial.println(WiFi.localIP());
        displayStat();
      }
    }
    client.publish(availabilityTopic, "online");
    vTaskDelay(120000 / portTICK_PERIOD_MS);  // wait 2 minutes before checking the status again
  }
}
//------------------------------------------------------------------------------
void printStat() {
  Serial.print(loopCount);
  Serial.print("   ");
  Serial.print(trace);
  Serial.print("   ");
  Serial.print(sensorIn);
  Serial.print("   ");
  //Serial.print(stepPosition);
  Serial.print("   ");
  Serial.print(WiFi.localIP());
  ;
  Serial.print("   ");
  Serial.print(millis());
  Serial.print("   ");
  Serial.print((currentTime));
  Serial.print("   ");
  Serial.print((startTime));
  Serial.print("   ");
  x = currentTime - startTime;
  Serial.print(closeTime);
  Serial.println("   ");
}
//------------------------------------------------------------------------------

void displayStat() {
  ArduinoOTA.handle();
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.setCursor(0, 0);
  display.print(trace);
  display.print("        ");
  display.setTextSize(1);
  display.setCursor(0, 33);
  display.print(setupID);
  display.setCursor(50, 33);
  display.print(ver);
  display.setCursor(120, 33);
  display.print(gateType);
  display.setCursor(0, 43);
  display.print(toolString);
  display.setCursor(110, 53);
  display.print(gateDelaySeconds);
  display.setCursor(0, 53);
  display.print(WiFi.localIP());
  display.display();
}

//----------------------------------------

void errorState() {
  //delay(3000);
  Serial.println("errorState line 230");
  OTA();
  Serial.println(" errorState Line232 ");
  //Serial.print(" LimitSwitch = ");
  //Serial.println (digitalRead(limitSwitchPin));
  digitalWrite(enablePin, HIGH);
  //Serial.print(" LimitSwitch = ");
  //Serial.println (digitalRead(limitSwitchPin));
  client.publish(availabilityTopic, "offline");
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.setCursor(20, 0);
  display.print("Error");
  display.setCursor(0, 33);
  display.print("Code");
  display.setCursor(100, 33);
  display.print(eCode);
  display.display();
  for (;;) {
    delay(500);
    ArduinoOTA.handle();

  }                                                         /*
       Code 1 = excessive missed steps: gate may be blocked
       Code 2 = Board assignment does not match EEPROM
       Code 3 = Gate assignment error:  Check settings
*/
  Serial.println("errorState Line 257");
}

//----------------------------------------

void setupError() {
  WiFiConnect();
  delay(3000);
  OTA();
  digitalWrite(enablePin, HIGH);
  client.publish(availabilityTopic, "offline");
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.setCursor(0, 5);
  display.print("Setup");
  display.setCursor(0, 35);
  display.print("Error");
  display.display();
  for (;;) {
    delay(500);
    ArduinoOTA.handle();
    if (linkPin != linkPin)  {
      break;
    }
  }
}

//----------------------------------------
void writeToBootLog() {

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("write to boot log");
    Serial.println("");
    Serial.print(" write to boot log line 277 Gate Type           = ");
    Serial.println(gateType);
    Serial.println("");
    //String gateTempType = String(gateType);
    static bool flag = false;
    //Serial.println("WiFi Connected!");
    urlFinal = GOOGLE_SCRIPT_Boot + "gate=M" + machineID + "&board=" + boardID + "&ipa=" + ipa + "&ver=" + verString + "&gateType=" + gateType;
    //Serial.println("");
    Serial.println(urlFinal);
    Serial.println("POST data to spreadsheet:  ");
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    Serial.print("httpCode=  ");
    Serial.println (httpCode);

    Serial.println("write to boot log");
    //gateTempType.toCharArray(gateType, 2);

    Serial.println("");
    Serial.print(" write to boot log line 297 Gate Type           = ");
    Serial.println(gateType);
    Serial.println("");
  }
  else {
    Serial.println ("WiFi not connected!");
  }
}


//------------------------------------------------------------------

void reconnect() {
  Serial.print("Attempting MQTT connection...");
  String clientId = "ESP32Client-";
  clientId += String(random(0xffff), HEX);
  // Attempt to connect
  if (client.connect(clientId.c_str())) {
    // Serial.println("connected");
  } else {
    Serial.print("failed, rc=");
    Serial.println(client.state());
    delay(5000);
  }
}


//------------------------------------------------------------------
void OTA() {
  if (otaOn == false) {
    Serial.println("OTA line 342");
    ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else  // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    Serial.println("OTA line 368");
    //Serial.println (gateName);
    ArduinoOTA.setHostname(gateName);
    ArduinoOTA.begin();
    otaOn = true;
  }
}
