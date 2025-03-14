void setup() {
  settings();
  delay(100);
  Serial.begin(115200);
  //Serial.println("Booting");

  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0X3C);
  trace = "Start";
  displayStat();

  WiFiConnect();

  // Initialize EEPROM with defined size
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("Failed to initialize EEPROM");
    return;
  }
  boardconfiguration();
  // Serial.print ("                                                Setup line 18 gateName = ");
  //Serial.println (gateName);
  prepNames();
  // Serial.print ("                                                Setup line 21 gateName = ");
  //Serial.println (gateName);

  pinMode(ANALOG_PIN_IN, INPUT);
  pinMode(reedRelayPin, OUTPUT);
  pinMode(greenLEDpin, OUTPUT);
  pinMode(redLEDpin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(limitSwitchPin, INPUT_PULLUP);
  pinMode(gateOn, OUTPUT);
  pinMode(gateOff, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(linkPin, INPUT_PULLUP);
  digitalWrite(greenLEDpin, HIGH);
  digitalWrite(redLEDpin, HIGH);
  digitalWrite(reedRelayPin, LOW);
  digitalWrite(enablePin, HIGH);
  digitalWrite(linkPin, HIGH);
  digitalWrite(gateOn, LOW);
  digitalWrite(gateOff, LOW);
  //digitalWrite(limitSwitchPin, LOW);

  //Serial.println("Starting");

  trace = "Booting";
  displayStat();

  closeDelayTime = 1000 * gateDelaySeconds;

  client.setKeepAlive(keepAlive);
  client.setServer(mqtt_server, 1883);
  MQTTconnect();

  runTime = 0;
  ipa = WiFi.localIP().toString() + "....Boot";
  //write_to_google_sheet();
  ipa = WiFi.localIP().toString();

  xTaskCreatePinnedToCore(
    pingBroker,
    "Ping Mosquitto Broker",
    5000,
    NULL,
    1,
    NULL,
    CONFIG_ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    keepWiFiAlive,
    "Keep WiFi Alive",
    5000,
    NULL,
    3,
    NULL,
    CONFIG_ARDUINO_RUNNING_CORE);

  xTaskCreatePinnedToCore(
    keepMqttAlive,
    "Keep MQTT Alive",
    5000,
    NULL,
    4,
    NULL,
    CONFIG_ARDUINO_RUNNING_CORE);


  //Serial.println("Setup line 151");

  Serial.println("");
  Serial.print("Gate Type           = ");
  Serial.println(gateType);
  Serial.println("");

  Serial.print("BoardIdByte         = ");
  Serial.println(boardIdByte);

  Serial.print("Board ID            = ");
  Serial.println(boardID);

  Serial.print("Board Version       = ");
  Serial.println(boardVer);

  Serial.print("Board Configuration = ");
  Serial.println(boardConfig);

  //Serial.print("flashed??           = ");
  //Serial.println(flash);

  Serial.print("ANALOG_PIN_IN       = ");
  Serial.println(ANALOG_PIN_IN);

  Serial.print("limitSwitchPin      = ");
  Serial.println(limitSwitchPin);

  Serial.print("reedRelayPin        = ");
  Serial.println(reedRelayPin);

  Serial.print("greenLEDpin         = ");
  Serial.println(greenLEDpin);

  Serial.print("redLEDpin           = ");
  Serial.println(redLEDpin);

  Serial.print("gateOn              = ");
  Serial.println(gateOn);

  Serial.print("gateOff             = ");
  Serial.println(gateOff);

  Serial.print("stepPin             = ");
  Serial.println(stepPin);

  Serial.print("dirPin              = ");
  Serial.println(dirPin);

  Serial.print("enablePin           = ");
  Serial.println(enablePin);
  Serial.println("");
  Serial.println(WiFi.localIP());
  Serial.println("");
  Serial.print(" Setup line 197 Gate Type           = ");
  Serial.println(gateType);
  Serial.println("");


  writeToBootLog();
  Serial.println("");
  Serial.println("Setup Line 185");
  OTA();
  ArduinoOTA.handle();

                                          /*
     -------------------------------------
                Initialize
     -------------------------------------*/
  Serial.println("");
  Serial.print(" Setup line 205 Gate Type           = ");
  Serial.println(gateType);
  Serial.println("");

  digitalWrite(reedRelayPin, LOW);

  Serial.println("");
  Serial.print(" Setup line 212 Gate Type           = ");
  Serial.println(gateType);
  Serial.println("");


  if ((gateType != "A") && (gateType != "B") && (gateType != "C") && (gateType != "D") && (gateType != "S") && (gateType != "P") && (gateType != "M")) {

    Serial.println("");
    Serial.print("Gate Type           = ");
    Serial.println(gateType);
    Serial.println("");
    Serial.print("ERROR  Check gateType  ");
    Serial.println(gateType);
    eCode = 3;
    errorState();
  }

  const char* mqttTopic = BGtopic;
  client.publish(availabilityTopic, "online");

  //Serial.println ("Setup line 160 ");

  //*****************************************
  if (gateType == "S") {
    lastMsgTime = 0;
    lastSample = 0;
    return;
  }

  Serial.println ("Setup  line line 216");

  //*****************************************
  if (gateType == "P") {
    lastMsgTime = 0;
    lastSample = 0;
    sensorIn = (analogRead(ANALOG_PIN_IN));

    if (sensorIn < trigger) {
      toolOff();
    } else {
      trace = "ON";
      toolOn();
    }
    return;
  }
  Serial.println ("Setup  line line 232");

  //*****************************************
  if (gateType == "M") {
    sensorIn = (analogRead(ANALOG_PIN_IN));
    sensorIn = 4095 - sensorIn;
    //Serial.println (sensorIn);
    if (sensorIn > (trigger + triggerDelta)) {
      //openGate();
      trace = "On";
      displayStat();
    }
    if (sensorIn < trigger) {
      //homePosition();
      trace = "Off";
      gateOpenTime = 0;
      displayStat();
      gateState = "closed";
      client.publish(BGtopic, gateState, false);
    }
    return;
  }

  Serial.println ("Setup  line line 255 ");

  //*****************************************
  //        All Other Controllers
  Serial.println ("Setup  line line 269");

  sensorIn = (analogRead(ANALOG_PIN_IN));

  Serial.println ("Setup  line line 245");
  if (sensorIn > (trigger + triggerDelta)) {
    Serial.println ("Setup  line line 247");
    openGate();

    Serial.println ("Setup  line line 250");
    trace = "OPEN";
    displayStat();
  }
  if (sensorIn < trigger) {

    Serial.println ("Setup line 256");

    homePosition();
    trace = "CLOSED";
    gateOpenTime = 0;
    displayStat();
    gateState = "closed";
    client.publish(BGtopic, gateState, false);
  }
  Serial.print("Setup line 283");
}
