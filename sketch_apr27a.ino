#include "DHT.h"
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <Temboo.h>
#include "TembooAccount.h" // Contains Temboo account information
#define DHTPIN 2 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

WiFiClient client;

int numRuns = 1;   // Execution count, so this doesn't run forever
int maxRuns = 1000000;   // Maximum number of times the Choreo should be executed

void setup() {
  Serial.begin(9600);
  
  // For debugging, wait until the serial console is connected
  delay(4000);
  while(!Serial);

  int wifiStatus = WL_IDLE_STATUS;

  // Determine if the WiFi Shield is present
  Serial.print("\n\nShield:");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("FAIL");

    // If there's no WiFi shield, stop here
    while(true);
    //delay(10000);
    dht.begin();
  }

  Serial.println("OK");

  // Try to connect to the local WiFi network
  while(wifiStatus != WL_CONNECTED) {
    Serial.print("WiFi:");
    wifiStatus = WiFi.begin("TP-LINK_37C598","25729559");

    if (wifiStatus == WL_CONNECTED) {
      Serial.println("OK");
    } else {
      Serial.println("FAIL");
    }
    //delay(5000);
  }

  Serial.println("Setup complete.\n");
}

void loop() {

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C \n");
  
  if (numRuns <= maxRuns) {
    Serial.println("Running AppendRow - Run #" + String(numRuns++));

    TembooChoreo AppendRowChoreo(client);

    // Invoke the Temboo client
    AppendRowChoreo.begin();

    // Set Temboo account credentials
    AppendRowChoreo.setAccountName(TEMBOO_ACCOUNT);
    AppendRowChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    AppendRowChoreo.setAppKey(TEMBOO_APP_KEY);

    // Set Choreo inputs
    String SpreadsheetTitleValue = "PSO_Assignment";
    AppendRowChoreo.addInput("SpreadsheetTitle", SpreadsheetTitleValue);
    String RowDataValue = "" + String(t) + "," + String(h);
    //String RowDataValue += "," ;
    //String RowDataValue += String(h);
    AppendRowChoreo.addInput("RowData", RowDataValue);
    String RefreshTokenValue = "1/WIpFPAi3hG0oNz7ArsNxBhD5v4f6KCY8xKyefwxseRI";
    AppendRowChoreo.addInput("RefreshToken", RefreshTokenValue);
    String ClientSecretValue = "qc_gbMBZbqqr-saPOmCyclor";
    AppendRowChoreo.addInput("ClientSecret", ClientSecretValue);
    String ClientIDValue = "223113982225-rftg93el7j3i3er0h1o4kiboemkch03n.apps.googleusercontent.com";
    AppendRowChoreo.addInput("ClientID", ClientIDValue);

    // Identify the Choreo to run
    AppendRowChoreo.setChoreo("/Library/Google/Spreadsheets/AppendRow");

    // Run the Choreo; when results are available, print them to serial
    AppendRowChoreo.run();

    while(AppendRowChoreo.available()) {
      char c = AppendRowChoreo.read();
      Serial.print(c);
    }
    AppendRowChoreo.close();
  }

  Serial.println("\nWaiting...\n");
  delay(5000); // wait 30 seconds between AppendRow calls
}
