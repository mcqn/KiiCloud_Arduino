// (c) Copyright 2013 MCQN Ltd.
// Released under Apache License, version 2.0
//
// Simple example to authenticate a user with the given app
// in KiiCloud

#include <SPI.h>
#include <HttpClient.h>
#include <GSM.h>

// Your phone SIM PIN Number
#define PINNUMBER ""

// APN data
#define GPRS_APN       "GPRS_APN"  // replace your GPRS APN
#define GPRS_LOGIN     "login"     // replace with your GPRS login
#define GPRS_PASSWORD  "password"  // replace with your GPRS password

// initialize the library instance
GSM gsmAccess;     // include a 'true' parameter to enable debugging
GPRS gprs;

// Details of your KiiCloud app and a user account for that app
// Note: not the user details for your KiiCloud account!
#define KII_APP_ID "YOUR KII APP ID"
#define KII_APP_KEY "YOUR KII APP KEY"
#define KII_USERNAME "YOUR DEVICE USERNAME"
#define KII_PASSWORD "YOUR DEVICE PASSWORD"

// Name of the server we want to connect to
const char kHostname[] = "api.kii.com";
// Path to download (this is the bit after the hostname in the URL
// that you want to download
const char kPath[] = "/api/oauth2/token";

const int kAccessTokenSize = 50;
char access_token[kAccessTokenSize];

void setup()
{
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println("KiiClound authentication");

  // connection state
  boolean notConnected = true;
  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while(notConnected)
  {
    if((gsmAccess.begin(PINNUMBER)==GSM_READY) &
      (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD)==GPRS_READY))
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }
  Serial.println("Modem initialised");
}

void loop()
{
  int err =0;
  
  GSMClient c;
  HttpClient http(c);
  
  http.beginRequest();
  err = http.post(kHostname, kPath);
  if (err == 0)
  {
    Serial.println("startedRequest ok");
    
    // Need to send some additional headers
    http.sendHeader("Content-Type", "application/json");
    http.sendHeader("x-kii-appid", KII_APP_ID);
    http.sendHeader("x-kii-appkey", KII_APP_KEY);
    
    // Work out how much data we'll be posting
    // THIS NEEDS TO EXACTLY MATCH THE STUFF WE SEND LATER ON
    int content_len = 0;
    content_len += strlen("{ \"username\":\"");
    content_len += strlen(KII_USERNAME);
    content_len += strlen("\", ");
    content_len += strlen("\"password\":\"");
    content_len += strlen(KII_PASSWORD);
    content_len += strlen("\" }");
    // And tell the server
    http.sendHeader("Content-Length", content_len);
    // We've finished sending the headers now
    http.endRequest();
    
    // Now send the data over
    // THIS NEEDS TO EXACTLY MATCH THE STUFF WE CALCULATED THE LENGTH OF ABOVE
    http.print("{ \"username\":\"");
    http.print(KII_USERNAME);
    http.print("\", ");
    http.print("\"password\":\"");
    http.print(KII_PASSWORD);
    http.print("\" }");
    
    err = http.responseStatusCode();
    if (err >= 200 && err < 300)
    {
      Serial.println("Successful response.  Now finding the authentication token");

      err = http.skipResponseHeaders();
      if (err >= 0)
      {
        if (http.find("\"access_token\""))
        {
          // We've got to the access token in the JSON response
          // Skip to the start of the token itself
          if (http.find("\""))
          {
            // Found the start of the token
            int len = http.readBytesUntil('\"', access_token, kAccessTokenSize);
            access_token[len] = '\0';
            Serial.print("Got access token of: ");
            Serial.println(access_token);
          }
          else
          {
            Serial.println("Error, malformed response");
          }
        }
        else
        {
          Serial.println("Error, couldn't find access_token in response");
        }
      }
      else
      {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    }
    else
    {    
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else
  {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop();

  // And just stop, now that we've tried a download
  while(1);
}


