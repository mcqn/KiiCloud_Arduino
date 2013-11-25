// (c) Copyright 2013 MCQN Ltd.
// Released under Apache License, version 2.0
//
// Simple example to create an object in the given bucket in KiiCloud
// 
// This example will create an object containing a data item
// called "sample data" with the value "some data value"
// Amend the code which sends that JSON to store different data/value pairs

#include <SPI.h>
#include <HttpClient.h>
#include <Ethernet.h>
#include <EthernetClient.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Details of your KiiCloud app and a user account for that app
// Note: not the user details for your KiiCloud account!
#define KII_APP_ID "YOUR KII APP ID"
#define KII_APP_KEY "YOUR KII APP KEY"
#define KII_USERNAME "YOUR DEVICE USERNAME"
#define KII_PASSWORD "YOUR DEVICE PASSWORD"
#define KII_BUCKET_NAME "YOUR BUCKET NAME"

// Name of the server we want to connect to
const char kHostname[] = "api.kii.com";
// Path to download (this is the bit after the hostname in the URL
// that you want to download
const char kAuthPath[] = "/api/oauth2/token";
const char kObjectPath[] = "/api/apps/" KII_APP_ID "/buckets/" KII_BUCKET_NAME "/objects";

const int kAccessTokenSize = 50;
char access_token[kAccessTokenSize];

void setup()
{
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println("KiiClound object creation");

  while (Ethernet.begin(mac) != 1)
  {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(15000);
  }  
}

int getAuthToken()
{
  int err =0;
  EthernetClient c;
  HttpClient http(c);

  http.beginRequest();
  err = http.post(kHostname, kAuthPath);
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
    http.sendHeader(HTTP_HEADER_CONTENT_LENGTH, content_len);
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
            Serial.println();
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
  
  return err;
}

void loop()
{
  int err =0;
  
  err = getAuthToken();
  if (err == 0)
  {
    // Now we're have an access token, we can create an object
    EthernetClient c;
    HttpClient http(c);

    http.beginRequest();
    err = http.post(kHostname, kObjectPath);
    if (err == 0)
    {
      Serial.println("startedRequest ok");
    
      // Build the authorization header info
      char auth_header[kAccessTokenSize + 7]; // enough space for "Bearer "+access_token
      auth_header[0] = '\0';
      strcat(auth_header, "Bearer ");
      strcat(auth_header, access_token);
    
      // Need to send some additional headers
      http.sendHeader("Authorization", auth_header);
      http.sendHeader("Content-Type", "application/vnd." KII_APP_ID ".mydata+json");
      http.sendHeader("x-kii-appid", KII_APP_ID);
      http.sendHeader("x-kii-appkey", KII_APP_KEY);
    
      // Work out how much data we'll be posting
      // THIS NEEDS TO EXACTLY MATCH THE STUFF WE SEND LATER ON
      int content_len = 0;
      content_len += strlen("{ \"sample data\":\"");
      content_len += strlen("some data value");
      content_len += strlen("\" }");
      // And tell the server
      http.sendHeader(HTTP_HEADER_CONTENT_LENGTH, content_len);
      // We've finished sending the headers now
      http.endRequest();
    
      // Now send the data over
      // THIS NEEDS TO EXACTLY MATCH THE STUFF WE CALCULATED THE LENGTH OF ABOVE
      http.print("{ \"sample data\":\"");
      http.print("some data value");
      http.print("\" }");
    
      err = http.responseStatusCode();
      if (err >= 200 && err < 300)
      {
        Serial.println("Bucket created.  Finding the objectID.");

        err = http.skipResponseHeaders();
        if (err >= 0)
        {
          if (http.find("\"objectID\""))
          {
            // We've got to the object ID in the JSON response
            // Skip to the start of the ID itself
            if (http.find("\""))
            {
              // Found the start of the ID
              char objectID[100];
              int len = http.readBytesUntil('\"', objectID, 100);
              objectID[len] = '\0';
              Serial.print("Got objectID of: ");
              Serial.println(objectID);
              Serial.println();
            }
            else
            {
              Serial.println("Error, malformed response in bucket creation");
            }
          }
          else
          {
            Serial.println("Error, couldn't find objectID in response");
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
  
  }

  // And just stop, now that we've tried a download
  while(1);
}


