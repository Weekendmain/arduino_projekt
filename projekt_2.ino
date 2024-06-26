//#include <TimerOne.h>
#include <Arduino.h>
#include <DHT.h>
#include <Encoder.h>
#include <SI114X.h>
#include <SPI.h>
#include <Si115X.h>
#include <U8g2lib.h>
#include <UnoWiFiDevEd.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiNINA.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include <Wire.h>

#include "Si115X.h"
Si115X si1151;

U8G2_SSD1306_128X64_ALT0_F_HW_I2C u8g2(
    U8G2_R0, /* reset=*/U8X8_PIN_NONE);  // SSD1306 and SSD1308Z are compatible
// Lichtsensor
SI114X SI1145 = SI114X();

char ssid[] = "iPhone (Artem)";  // your network SSID (name)
char pass[] =
    "12345678";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;  // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(80);

#define DHTTYPE DHT11
//#define DHTTYPE DHT20
#define DHTPIN A0
#define MoisturePin A1
#define MoistureValue 0
#define ButtonPin 2
#define ButtonClick
#define EncoderPin1 3
#define EncoderPin2 4
#define WaterflowPin 5
#define RelayPin 6
#define OneSecond 1000
#define DataUpdateInterval 20000
#define RelayOn HIGH
#define RelayOff LOW
#define NoWaterTimeOut 3

unsigned int uiWaterVolume = 0;
// unsigned char WaterflowFlag = 0;
unsigned int WaterflowRate = 0;
unsigned char EncoderFlag = 0;
unsigned long StartTime = 0;
unsigned char ButtonFlag = 0;
unsigned long counter = 0;
volatile int flow_frequency;  // Measures flow sensor pulsesunsigned

int l_hour;                    // Calculated litres/hour
unsigned char flowsensor = 2;  // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;

void flow()  // Interrupt function

{
  flow_frequency++;
}

DHT dht(DHTPIN, DHTTYPE);
float SensorHumidity = 0;
float SensorTemperature = 0;
float SensorMoisture = 0;
float UVIndex = 0;
char buffer[30];

void setup() {
  // Initialisiere den Serial Monitor
  Serial.begin(9600);
  Wire.begin();
  u8g2.begin();           // initialize u8g2 oled display
  u8g2.clearDisplay();    // clear the oled display
  u8g2.refreshDisplay();  // set standart mode oled display

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);  // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP
    // network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();  // start the web server on port 80
  printWifiStatus();

  // Water Flow Sensor
  pinMode(flowsensor, INPUT);
  digitalWrite(flowsensor, HIGH);  // Optional Internal Pull-Up
  Serial.begin(9600);
  attachInterrupt(0, flow, RISING);  // Setup Interrupt
  sei();                             // Enable interrupts
  currentTime = millis();
  cloopTime = currentTime;

  // encoder test
  // Serial.println("DHTxx test!");
  // dht.begin();

  // initialize the button
  pinMode(ButtonPin, INPUT);
  // attachInterrupt(0, ButtonClick, FALLING);

  // initialize the encoder
  pinMode(EncoderPin1, INPUT);
  pinMode(EncoderPin2, INPUT);
  //  attachInterrupt(1, EncoderRotate, RISING);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, OUTPUT);
  // Setze den Pin für das Relay der Pumpe als Ausgang
  pinMode(4, OUTPUT);

  // Setze den Pin für das Relay des Lüfters als Ausgang
  pinMode(5, OUTPUT);

  pinMode(8, INPUT);
  digitalWrite(8, HIGH);
}

void loop() {
  // Display funсtion
  /* int OledDisplay {
    // u8g2.clearBuffer();                  // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB08_tr);  // choose a suitable font
    u8g2.drawStr(0, 10,
                 "Hello World!");  // write something to the internal memory
    u8g2.sendBuffer();             // transfer internal memory to the display
    delay(1000);
    }
  */

  // Temp & Humidity Sensor function
  /*  int TempHum {
      float temp_hum_val[2] = {0};
      // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow
      // sensor)

      if (!dht.readTempAndHumidity(temp_hum_val)) {
        debug.print("Humidity: ");
        debug.print(temp_hum_val[0]);
        debug.print(" %\t");
        debug.print("Temperature: ");
        debug.print(temp_hum_val[1]);
        debug.println(" *C");
      } else {
        debug.println("Failed to get temprature and humidity value.");
      }

      delay(1500);
    }
  */
  /*// Water Flow Sensor
    currentTime = millis();  // Every second, calculate and print litres/hour
    if (currentTime >= (cloopTime + 1000)) {
    cloopTime = currentTime;  // Updates cloopTime
    // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
    l_hour = (flow_frequency * 60 /
              7.5);  // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
    flow_frequency = 0;         // Reset Counter
    Serial.print(l_hour, DEC);  // Print litres/hour
    Serial.println(" L/hour");
    }*/

  //  Button funсtion
  /*int ButtonClick {
    if (digitalRead(ButtonPin) == 0) {
      delay(10);
      if (digitalRead(ButtonPin) == 0) {
        ButtonFlag = 1;
      }
    }
    }*/

  /* // Wasserpumpe
    void WaterPumpOn { digitalWrite(RelayPin, RelayOn); }

    void WaterPumpOff() { digitalWrite(RelayPin, RelayOff); }
  */
  WiFiClient client = server.available();  // listen for incoming clients

  if (client) {                    // if you get a client,
    Serial.println("new client");  // print a message out the serial port
    String currentLine =
        "";  // make a String to hold incoming data from the client
    while (client.connected()) {  // loop while the client's connected
      if (client.available()) {   // if there's bytes to read from the client,
        char c = client.read();   // read a byte, then
        Serial.write(c);          // print it out to the serial monitor
        if (c == '\n') {          // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a
          // row. that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200
            // OK) and a content-type so the client knows what's coming, then a
            // blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head>");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("</head>");

            client.println("<p>");

            client.print("Temperature: <span style=\"color: red;\">");
            client.println("&deg;C</span>");

            client.println("</p>");
            client.println("</html>");
            client.flush();
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage
                                 // return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

float readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  } else {
    Serial.println(t);
    return t;
  }
}
float readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  } else {
    Serial.println(h);
    return h;
  }
}
