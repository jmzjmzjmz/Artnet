/*
This example will receive multiple universes via Artnet and control a strip of ws2811 leds via
Paul Stoffregen's excellent OctoWS2811 library: https://www.pjrc.com/teensy/td_libs_OctoWS2811.html
This example may be copied under the terms of the MIT license, see the LICENSE file for details
*/

#include <Artnet.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include <OctoWS2811.h>

// OctoWS2811 settings
const int ledsPerStrip = 240; // change for your setup
const byte numStrips= 1; // change for your setup
const int numLeds = ledsPerStrip * numStrips;
const int numberOfChannels = numLeds * 3; // Total number of channels you want to receive (1 led = 3 channels)
DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];
const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

// Artnet settings
Artnet artnet;
const int startUniverse = 0; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.
int previousDataLength = 0;


void setup()
{
  Serial.begin(115200);
  Serial.println("Artnet OCTO Sketch");
  Serial.println(maxUniverses);

  // Change ip and mac address for your setup

  byte mac[6];

  artnet.teensyMAC(mac); // if using teensy get real mac//
  //  byte mac[] = {0x04, 0xE9, 0xE5, 0x00, 0x69, 0xEC};   // else create fake mac

  byte ip[] = {192, 168, 0, 198};

  artnet.begin(mac); // for DHCP
  // artnet.begin(mac, ip); // for static ip

  artnet.setName("Teensy Artnet Boy"); // set name for artPoll reply (how it shows up in resolume etc.)
  leds.begin();
  initTest();

  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);
  artnet.setArtSyncCallback(onSync);
}

void loop()
{
  // we call the read function inside the loop
  artnet.read();
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data, IPAddress remoteIP)
{
  // read universe and put into the right part of the display buffer
  for (int i = 0; i < length / 3; i++)
  {
    int led = i + (universe - startUniverse) * (previousDataLength / 3);
    if (led < numLeds)
      leds.setPixel(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
  }
  previousDataLength = length;
}

void onSync(IPAddress remoteIP) {
    leds.show();
}

void initTest()
{
  for (int i = 0 ; i < numLeds ; i++)
    leds.setPixel(i, 127, 0, 0);
  leds.show();
  delay(500);
  for (int i = 0 ; i < numLeds  ; i++)
    leds.setPixel(i, 0, 127, 0);
  leds.show();
  delay(500);
  for (int i = 0 ; i < numLeds  ; i++)
    leds.setPixel(i, 0, 0, 127);
  leds.show();
  delay(500);
  for (int i = 0 ; i < numLeds  ; i++)
    leds.setPixel(i, 0, 0, 0);
  leds.show();
}
