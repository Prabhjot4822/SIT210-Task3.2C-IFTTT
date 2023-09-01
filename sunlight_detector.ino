#include "arduino_secrets.h"
#include <WiFiNINA.h>
#include <BH1750.h>
#include <Wire.h>

// Create an instance of the BH1750 light sensor
BH1750 lightMeter;

// Please enter your sensitive data in the Secret tab
char ssid[] = SECRET_SSID;                // your network SSID (name)
char pass[] = SECRET_PASS;                // your network password (use for WPA, or use as key for WEP)

// Create two WiFi client instances for making HTTP requests
WiFiClient client1;
WiFiClient client2;

// Define the host name and paths for HTTP requests
char HOST_NAME[] = "maker.ifttt.com";
String PATH_1_NAME = "/trigger/sunlight_received/with/key/E9zLCH0UANFRAPOjrsLwob8c0wZ1BiqYlLU_Soo3gW";
String PATH_2_NAME = "/trigger/sunlight_gone/with/key/E9zLCH0UANFRAPOjrsLwob8c0wZ1BiqYlLU_Soo3gW";
String queryString = "?value1=57&value2=25";

void setup()
{
  // Initialize WiFi and Serial communication
  WiFi.begin(ssid, pass);
  Serial.begin(9600);
  Wire.begin();
  lightMeter.begin();

  // Wait for WiFi connection to be established
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop()
{
  // Read the light level from the BH1750 sensor
  float lux = lightMeter.readLightLevel();
  
  // Check if the light level is above a threshold (50 lux)
  if (lux > 50) 
  {
    // Call the sunlightMail function to send an HTTP request
    sunlightMail();
    delay(3000);

    // Wait until the light level goes below the threshold
    while(lux > 50)
    {
      lux = lightMeter.readLightLevel();
      Serial.print("Light: ");
      Serial.print(lux);
      Serial.println(" lx");
      delay(3000);
    }
  }

  // Check if the light level is below the threshold (50 lux)
  if (lux < 50) 
  {
    // Call the noSunlightMail function to send an HTTP request
    noSunlightMail();
    delay(3000);

    // Wait until the light level goes above the threshold
    while(lux < 50)
    {
      lux = lightMeter.readLightLevel();
      Serial.print("Light: ");
      Serial.print(lux);
      Serial.println(" lx");
      delay(3000);
    }
  } 
}

// Function to send an HTTP request when sunlight is detected
void sunlightMail()
{
  // Connect to the web server on port 80:
  if (client1.connect(HOST_NAME, 80))
  {
    // If connected:
    Serial.println("Connected to server");
  }
  else
  { // If not connected:
    Serial.println("Connection failed");
  }

  // Make an HTTP request:
  // Send HTTP header
  client1.println("GET " + PATH_1_NAME + queryString + " HTTP/1.1");
  client1.println("Host: " + String(HOST_NAME));
  client1.println("Connection: close");
  client1.println(); // End HTTP header

  while (client1.connected())
  {
    if (client1.available())
    {
      // Read an incoming byte from the server and print it to the serial monitor:
      char c = client1.read();
      Serial.print(c);
    }
  }

  // The server's disconnected, stop the client:
  client1.stop();
  Serial.println("Disconnected");
}

// Function to send an HTTP request when no sunlight is detected
void noSunlightMail()
{
  // Connect to the web server on port 80:
  if (client2.connect(HOST_NAME, 80))
  {
    // If connected:
    Serial.println("Connected to server");
  }
  else
  { // If not connected:
    Serial.println("Connection failed");
  }

  // Make an HTTP request:
  // Send HTTP header
  client2.println("GET " + PATH_2_NAME + queryString + " HTTP/1.1");
  client2.println("Host: " + String(HOST_NAME));
  client2.println("Connection: close");
  client2.println(); // End HTTP header

  while (client2.connected())
  {
    if (client2.available())
    {
      // Read an incoming byte from the server and print it to the serial monitor:
      char c = client2.read();
      Serial.print(c);
    }
  }

  // The server's disconnected, stop the client:
  client2.stop();
  Serial.println("Disconnected");
}
