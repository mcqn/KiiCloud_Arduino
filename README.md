# KiiCloud_Arduino

Sample code to interface to the KiiCloud http://kii.com from Arduino.  For more documentation on the API, see http://documentation.kii.com/en/guides/rest/.

Requires the HttpClient from https://github.com/amcewen/HttpClient

## Setup

1. Create a KiiCloud account
1. Create an app.  Not sure if it matters what type of app, the HTML5/Javascript option seemed to work well.  Choose the "United States" server location
1. Don't worry about downloading any of the SDKs, etc.  Just go into the app on the dashboard
1. The "Access Keys" option (in the top right-hand corner) will have the important information that you'll need for the examples
1. Create an app user for your device so it can interact with the Kii Cloud:

    curl -X POST -H "Content-Type: application/vnd.kii.RegistrationRequest+json" -H "x-kii-appid:<YOUR APP ID>" -H "x-kii-appkey:<YOUR APP KEY>" "https://api.kii.com/api/apps/<YOUR APP ID>/users" -d '{"loginName":"<DEVICE USER NAME>", "displayName":"<DEVICE DISPLAY NAME>", "country":"US", "password":"<DEVICE PASSWORD>"}'

## Examples

### Ethernet

Examples to run on an Arduino Ethernet or an Arduino plus an Ethernet shield.

<dl>
<dt>EthernetAuthExample</dt><dd>An example to get an access token for an app user. Most other calls would need to do this first.</dd>
<dt>EthernetCreateObjectExample</dt><dd>Example code to first retrieve an access token, and then use it for authentication when creating a new object in the given bucket.</dd>
</dl>

### GSM

Examples to run on an Arduino plus GSM shield or an Eseye Hera200.

<dl>
<dt>GSMAuthExample</dt><dd>An example to get an access token for an app user. Most other calls would need to do this first.</dd>
<dt>GSMCreateObjectExample</dt><dd>Example code to first retrieve an access token, and then use it for authentication when creating a new object in the given bucket.</dd>
</dl>

