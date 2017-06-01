#include "DHT.h"
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <Temboo.h>
#include "TembooAccount.h" 
#define DHTPIN 2 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

/*
 Fionn Mcguire
 C13316356
 PSO Assignment to use temboo to log data
 */
//Above are librarys included for the sensor & wifi shield & temboo account connectivity
//Below are defined variables to be used throughout the program

WiFiClient client;

int outputPin = 3;
int outputPin2 = 4;
int numRuns = 1;   
int maxRuns = 1000000;   
int maxCalls = 10;


int calls = 0;

//In the setup i have initialised the sensor
//and connected to wifi providing error detection to print in serial monitor

void setup() {
  Serial.begin(9600);
  delay(4000);
  while(!Serial);

  int wifiStatus = WL_IDLE_STATUS;

  Serial.print("\n\nShield:");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("FAIL");

    while(true);
    delay(10000);
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
  }

  Serial.println("Setup complete.\n");
}



void loop() {

  //reading values from sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Providing error detection
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Celsius 
  float hic = dht.computeHeatIndex(t, h, false);

//printing values to serial monitor
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C \n");
  
  if (numRuns <= maxRuns) {
  //max runs = 1,000,000 for the sake of testing i left it on for a few days
  
    TembooChoreo AppendRowChoreo(client);

    // Invoke the Temboo client
    AppendRowChoreo.begin();

    // Set Temboo account credentials
    AppendRowChoreo.setAccountName(TEMBOO_ACCOUNT);
    AppendRowChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    AppendRowChoreo.setAppKey(TEMBOO_APP_KEY);

    // Specifying inputs
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

    // This is specifying which Choreo to run
    AppendRowChoreo.setChoreo("/Library/Google/Spreadsheets/AppendRow");
    AppendRowChoreo.run();

    while(AppendRowChoreo.available()) {
      char c = AppendRowChoreo.read();
      Serial.print(c);
      Serial.println("\nWaiting...\n");
      
      delay(5000);
      
    //Calling the average row identifier
    //This reads a row which is an average of the last 5 rows inputted
    //This gives a constant temperature for the last 10 minutes
    //This triggers an led to turn on if the temperature is outside the optimum range.
    //Red is above 22 degrees for too hot set at pin 3
    //Green is below 18 degrees for too cold set at pin 4
    Serial.println("Calling RetrieveCellValue Choreo...");
    runRetrieveCellValue();

    AppendRowChoreo.close();

  }

  
  }
  delay(5000);
}

void runRetrieveCellValue() {
  TembooChoreo RetrieveCellValueChoreo(client);
  // Set Temboo account credentials for this function RetrieveCellValueChoreo
  RetrieveCellValueChoreo.setAccountName(TEMBOO_ACCOUNT);
  RetrieveCellValueChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  RetrieveCellValueChoreo.setAppKey(TEMBOO_APP_KEY);

  // locally setting inputs
  String RefreshTokenValue = "1/WIpFPAi3hG0oNz7ArsNxBhD5v4f6KCY8xKyefwxseRI";
  RetrieveCellValueChoreo.addInput("RefreshToken", RefreshTokenValue);
  String ClientSecretValue = "qc_gbMBZbqqr-saPOmCyclor";
  RetrieveCellValueChoreo.addInput("ClientSecret", ClientSecretValue);
  String SpreadsheetNameValue = "PSO_Assignment";
  RetrieveCellValueChoreo.addInput("SpreadsheetName", SpreadsheetNameValue);
  String ClientIDValue = "223113982225-rftg93el7j3i3er0h1o4kiboemkch03n.apps.googleusercontent.com";
  RetrieveCellValueChoreo.addInput("ClientID", ClientIDValue);
  String CellLocationValue = "C2";
  RetrieveCellValueChoreo.addInput("CellLocation", CellLocationValue);
  String WorksheetNameValue = "Sheet1";
  RetrieveCellValueChoreo.addInput("WorksheetName", WorksheetNameValue);
  RetrieveCellValueChoreo.setChoreo("/Library/Google/Spreadsheets/RetrieveCellValue");
  unsigned int returnCode = RetrieveCellValueChoreo.run();

  // A return code of zero means everything worked
  if (returnCode == 0) {
    while (RetrieveCellValueChoreo.available()) {
      String name = RetrieveCellValueChoreo.readStringUntil('\x1F');
      name.trim();

      String data = RetrieveCellValueChoreo.readStringUntil('\x1E');
      data.trim();

      //This if controls the parameters of the optimum temperatures
      //The value retrieved is compared to the value specified by the if
      //If it is too hot/cold a pin will be turned high and turn an led on to signify 
      //The too hot/cold message for the customer
      if (name == "CellValue") {
        if (data > "22") {
          digitalWrite(outputPin, HIGH);
        }
        else if (data < "18") {
          Serial.println("Made it this far");
          digitalWrite(outputPin2, HIGH);
        }
      }
    }
  }

  RetrieveCellValueChoreo.close();
}














