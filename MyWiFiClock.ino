#include "sceleton.h"

#include <ESP8266HTTPClient.h>

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#include <WiFiUdp.h>
#include <core_esp8266_waveform.h>
#include <SoftwareSerial.h>

#include "worklogic.h"
#include "lcd.h"

unsigned int localPort = 2390;      // local port to listen for UDP packets

IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

const uint64_t dayInMs = 24*60*60*1000;

void updateTime();
unsigned long sendNTPpacket(IPAddress& address);

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

LcdScreen screen;
MAX72xx screenController(screen, D5, D7, D6);

// #define BEEPER_PIN D2 // Beeper

IRrecv irrecv(D2); // 

int testCntr = 0;

decode_results results;

struct Key {
  const char* bin;
  const char* value;

  Key(const char* bin_, const char* value_) : bin(bin_), value(value_) {}
};

struct Remote {
  const char* name;
  const std::vector<Key> keys;

  Remote(const char* _name, const std::vector<Key>& _keys) : name(_name), keys(_keys) {
  }
}; 

const Remote tvtuner( "tvtuner", 
  std::vector<Key> {
    Key("101000000000101010001000101000001000000000000010001010101", "n0"),
    Key("1010000000001010100010001010001000000000000000001010101010", "n1"),
    Key("1010000000001010100010001010001010001000000000000010001010101", "n2"),
    Key("10100000000010101000100010100010100010100000000000100000101010", "n3"),
    Key("10100000000010101000100010100010001000000000000010001010101010", "n4"),
    Key("1010000000001010100010001010001000001000000000001010001010", "n5"),
    Key("1010000000001010100010001010001000100010000000001000100010101", "n6"),
    Key("10100000000010101000100010100000101000000000001000001010101", "n7"),
    Key("1010000000001010100010001010000010001000000000100010001010", "n8"),
    Key("1010000000001010100010001010000010000010000000100010100010101", "n9"),

    Key("1010000000001010100010001010001010000000000000000010101", "tvfm"),
    Key("10100000000010101000100010100010101000000000000000001010101", "source"),
    Key("10100000000010101000100010100000001010100000001010000000101010", "scan"),
    Key("10100000000010101000100010100000101010100000001000000000101010", "power"),
    Key("10100000000010101000100010100010100000100000000000101000101", "recall"),
    Key("1010000000001010100010001010000000000010000000101010100010101", "plus_100"),
    Key("1010000000001010100010001010001010101010000000000000000010101", "channel_up"),
    Key("1010000000001010100010001010001010100010000000000000100010101", "channel_down"),
    Key("1010000000001010100010001010000010100010000000100000100010101", "volume_up"),
    Key("1010000000001010100010001010000000100010000000101000100010101", "volume_down"),
    Key("1010000000001010100010001010000000001010000000101010000010101", "mute"),
    Key("1010000000001010100010001010001000000010000000001010100010101", "play"),
    Key("1010000000001010100010001010000000001000000000101010001010101", "stop"),
    Key("1010000000001010100010001010000000000000000000101010101010101", "record"),
    Key("1010000000001010100010001010000010001010000000100010000010101", "freeze"),
    Key("1010000000001010100010001010001000001010000000001010000010101", "zoom"),
    Key("1010000000001010100010001010000000100000000000101000101010101", "rewind"),
    Key("1010000000001010100010001010000010101000000000100000001010101", "function"),
    Key("1010000000001010100010001010000000101000000000101000001010101", "wind"),
    Key("1010000000001010100010001010001000101000000000001000001010101", "mts"),
    Key("10100000000010101000100010100010001010100000000010000000101010", "reset"),
    Key("10100000000010101000100010100010101010000000000000000010101010", "min")
  }
);

const Remote canonCamera("CanonCamera",
  std::vector<Key> {
    Key("01010000000000010101000000010101010100000000000000000101010101010", "power"),
    Key("01010000000000010101000000010101000000000101000001010101000001010", "photo"),
    Key("01010000000000010101000000010101000001010100000001010000000101010", "volume_up"),
    Key("01010000000000010101000000010101010001010100000000010000000101010", "volume_down"),
    Key("01010000000000010101000000010101000101000000010001000001010100010", "func"),
    Key("01010000000000010101000000010101010001000001000000010001010001010", "menu"),
    Key("01010000000000010101000000010101000000010000010001010100010100010", "playlist"),
    Key("01010000000000010101000000010101000000000001000001010101010001010", "up"),
    Key("01010000000000010101000000010101010100000001000000000101010001010", "left"),
    Key("01010000000000010101000000010101000100000001000001000101010001010", "right"),
    Key("01010000000000010101000000010101010000000001000000010101010001010", "down"),
    Key("01010000000000010101000000010101000001000001000001010001010001010", "set"),
    Key("01010000000000010101000000010101000000000100010001010101000100010", "prev"),
    Key("01010000000000010101000000010101000000000100000001010101000101010", "next"),
    Key("01010000000000010101000000010101010100010000010000000100010100010", "rewind"),
    Key("01010000000000010101000000010101010001010000010000010000010100010", "forward"),
    Key("01010000000000010101000000010101010000000000000000010101010101010", "play"),
    Key("01010000000000010101000000010101000001000000000001010001010101010", "pause"),
    Key("01010000000000010101000000010101010101000100000000000001000101010", "stop"),
    Key("01010000000000010101000000010101000101010000010001000000010100010", "disp"),
  }
);

const Remote prologicTV("prologicTV",
  std::vector<Key> {
    Key("00000000000000000101010101010101010001010000010000010000010100010", "power"),
    Key("00000000000000000101010101010101000101000100000001000001000101010", "mute"),
    Key("00000000000000000101010101010101010000010000000000010100010101010", "n1"),
    Key("00000000000000000101010101010101010001010100000000010000000101010", "n2"),
    Key("00000000000000000101010101010101010101010100000000000000000101010", "n3"),
    Key("00000000000000000101010101010101010001010000000000010000010101010", "n4"),
    Key("00000000000000000101010101010101010000010100000000010100000101010", "n5"),
    Key("00000000000000000101010101010101010100010100000000000100000101010", "n6"),
    Key("00000000000000000101010101010101010000000100000000010101000101010", "n7"),
    Key("00000000000000000101010101010101010001000100000000010001000101010", "n8"),
    Key("00000000000000000101010101010101010101000100000000000001000101010", "n9"),
    Key("00000000000000000101010101010101000100000100000001000101000101010", "n0"),
    Key("00000000000000000101010101010101000000000000010001010101010100010", "fullscreen"),
    Key("00000000000000000101010101010101000100010000000001000100010101010", "volume_down"),
    Key("00000000000000000101010101010101000101010100000001000000000101010", "volume_up"),
    Key("00000000000000000101010101010101010001000000000000010001010101010", "channel_up"),
    Key("00000000000000000101010101010101000100000000000001000101010101010", "channel_down"),
    Key("00000000000000000101010101010101000001000100000001010001000101010", "ent"),
    Key("00000000000000000101010101010101000001010000010001010000010100010", "record"),
    Key("00000000000000000101010101010101000001000100010001010001000100010", "av_source"),
    Key("00000000000000000101010101010101000000000100000001010101000101010", "stop"),
    Key("00000000000000000101010101010101000001010000000001010000010101010", "time_shift"),
    Key("00000000000000000101010101010101000001010100000001010000000101010", "clear")
  }
);

const Remote transcendPhotoFrame("transcendPhotoFrame",
  std::vector<Key> {
    Key("00000000000101000001010101000001000100000000000001000101010101010", "power"),
    Key("00000000000101000001010101000001010101000100000000000001000101010", "home"),
    Key("00000000000101000001010101000001000001000100010001010001000100010", "photo"),
    Key("00000000000101000001010101000001010001000100010000010001000100010", "music"),
    Key("00000000000101000001010101000001000101000100010001000001000100010", "calendar"),
    Key("00000000000101000001010101000001010000000000000000010101010101010", "settings"),
    Key("00000000000101000001010101000001000101000000000001000001010101010", "slideshow"),
    Key("00000000000101000001010101000001010101000100010000000001000100010", "option"),
    Key("00000000000101000001010101000001010100010100000000000100000101010", "exit"),
    Key("00000000000101000001010101000001000001000000000001010001010101010", "rotate"),
    Key("00000000000101000001010101000001000100010100000001000100000101010", "zoom"),
    Key("00000000000101000001010101000001010100010100010000000100000100010", "ok"),
    Key("00000000000101000001010101000001000000010100000001010100000101010", "left"),
    Key("00000000000101000001010101000001010000010100000000010100000101010", "right"),
    Key("00000000000101000001010101000001000000010100010001010100000100010", "up"),
    Key("00000000000101000001010101000001000100010100010001000100000100010", "down"),
    Key("00000000000101000001010101000001010001000000000000010001010101010", "volume_up"),
    Key("00000000000101000001010101000001000101010100000001000000000101010", "volume_down"),
    Key("00000000000101000001010101000001000001010100000001010000000101010", "prev"),
    Key("00000000000101000001010101000001010001010100000000010000000101010", "next"),
    Key("00000000000101000001010101000001000101010000000001000000010101010", "play"),
    Key("00000000000101000001010101000001010101010100000000000000000101010", "mode"),
    Key("00000000000101000001010101000001010001010000000000010000010101010", "stop"),
    Key("00000000000101000001010101000001000100000100000001000101000101010", "mute")
  }
);


const Remote* remotes[] = { 
  &tvtuner, 
  &canonCamera, 
  &prologicTV,
  &transcendPhotoFrame
};

int lastCanonRemoteCmd = millis();
int lastNumber = millis();
boolean invertRelayState = false;
int currRelayState = 0; // All relays are off by default

boolean relayIsInitialized = false;
SoftwareSerial relay(D1, D0); // RX, TX

void setup() {
  // Initialize comms hardware
  // pinMode(BEEPER_PIN, OUTPUT);
  screenController.setup();
  screen.showMessage("Инициализация...");

  irrecv.enableIRIn();  // Start the receiver

  sceleton::setup();

  sceleton::switchRelaySink = [](int id, bool val) {
    // Serial.println(String("switchRelaySink: ") + (val ? "true" : "false"));
    debugPrint(String("switchRelaySink: ") + (val ? "true" : "false"));

    int bit = 1 << id;
    currRelayState = currRelayState & ~bit;
    if (val) {
      currRelayState = currRelayState | bit;
    }

    if (!relayIsInitialized) {
      relay.begin(9600);
      delay(100);
      relay.write(0x50);
      delay(100);
      relay.write(0x51);
      delay(100);
      relayIsInitialized = true;
    }
    
    relay.write('0' | (sceleton::invertRelayControl._value == "true" ? ~currRelayState : currRelayState));
  };

  sceleton::showMessageSink = [](const char* dd) {
    // 
    screen.showMessage(dd);
  };

  udp.begin(localPort);

  WiFi.hostByName(ntpServerName, timeServerIP);

  // sendHttp("/clock_init");

}

unsigned long oldMicros = micros();

uint32_t timeRetreivedInMs = 0;
uint32_t initialUnixTime = 0;
uint32_t timeRequestedAt = 0;

uint16_t hours = 0;
uint16_t mins = 0;
uint64_t nowMs = 0;
boolean sleeps = false;

const int updateTimeEachSec = 600; // By default, update time each 600 seconds

WiFiClient client;

void loop() {
  if (millis() % 5*60*1000 == 0 && (((millis() - timeRequestedAt) > (timeRetreivedInMs == 0 ? 5 : updateTimeEachSec)*1000))) {
    // debugPrint("Requesting time");
    timeRequestedAt = millis();
    sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  }

  oldMicros = micros();
  testCntr++;

  screen.clear();
  boolean wasSleeping = sleeps;

  if (timeRetreivedInMs) {
    updateTime();
    if (!sleeps) {
      screen.showTime(nowMs / dayInMs, nowMs % dayInMs);

      screenController.refreshAll();
    }

    // int m = (hours*100 + mins);
    // sleeps = m > 2330 || m < 540; // From 22:30 to 5:30 - do not show screen

    if (sleeps) {
      if (!wasSleeping) {
        debugPrint("Falling asleep");
      }
      screenController.refreshAll();
    }
  } else {
    const wchar_t* getTime = L"  Получаем время с сервера...  ";
    screen.printStr((micros() / 1000 / 50) % screen.getStrWidth(getTime), 0, getTime);
    screenController.refreshAll();
  }

  sceleton::loop();

  int cb = udp.parsePacket();
  if (cb >= NTP_PACKET_SIZE) {
    debugPrint("packet received, length=" + String(cb, DEC));
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    String pckt;
    for (int i = 0; i < cb; ++i) {
      if (i > 0) {
        pckt += " ";
      }
      pckt += String(packetBuffer[i], HEX);
    }
    debugPrint(pckt);

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    debugPrint("Seconds since Jan 1 1900 = " + String(secsSince1900, DEC));

    // Sometimes we get broken time. In case 2018 does not appear yet, let's just ignore it silently
    if (secsSince1900 > 118*365*24*60*60) {
      // now convert NTP time into everyday time:
      // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
      const uint64_t seventyYears = 2208988800ULL;
      // subtract seventy years:
      unsigned long epoch2 = secsSince1900 - seventyYears;

      timeRetreivedInMs = millis();
      initialUnixTime = epoch2;
      
      debugPrint("Unix time = " + String(epoch2, DEC));

      // print the hour, minute and second:

      updateTime();
    } else {
      debugPrint("INVALID TIME RECEIVED!");
    }
  }

  if (irrecv.decode(&results)) {
    if (results.rawlen > 30) {
      int decodedLen = 0;
      char decoded[300] = {0};
      String intervals("RAW: ");
      int prevL = 0;
      for (int i = 0; i < results.rawlen && i < sizeof(decoded); ++i) {
        char c = -1;
        int val = results.rawbuf[i];
        
        String valStr = String(val, DEC);
        for (;valStr.length() < 4;) valStr = " " + valStr;
        // intervals += valStr;
        // intervals += " ";

        if (val > 1000) {
          continue;
        } else if ((prevL + val) > 150 && (prevL + val) < 500) {
          c = '0';
        } else if ((prevL + val) > 600 && (prevL + val) < 900) {
          c = '1';
        } else {
          // Serial.print(".");
          prevL += val;
          continue; // skip!
        }
        decoded[decodedLen++] = c;
        prevL = 0;
      }

      String decodedStr(decoded);
      const Remote* recognizedRemote = NULL;
      const Key* recognized = NULL;
      int kk = 0;
      for (int r = 0; r < (sizeof(remotes)/sizeof(remotes[0])); ++r) {
        const Remote& remote = *(remotes[r]);
        for (int k = 0; k < remote.keys.size(); ++k) {
          if (decodedStr.indexOf(remote.keys[k].bin) != -1) {
            // Key pressed!
            recognized = &(remote.keys[k]);
            recognizedRemote = &remote;
            kk = k;

            String toSend = String("{ \"type\": \"ir_key\", ") + 
              "\"remote\": \"" + String(recognizedRemote->name) + "\", " + 
              "\"key\": \""  + String(remote.keys[k].value)  + "\", " +  
              "\"timeseq\": "  + String((uint32_t)millis(), DEC)  + " " +  
              "}";

            sceleton::webSocket->broadcastTXT(toSend.c_str(), toSend.length());

            break;
          }
        }
      }

      if (recognized == NULL) {
        debugPrint(decoded);
        debugPrint(intervals);
      } else {
        if (millis() - lastCanonRemoteCmd > 200) {
          lastCanonRemoteCmd = millis();

          String val(recognized->value);

        }
      }
    }

    irrecv.resume();  // Receive the next value
  }

  // Serial.println(String(millis(), DEC));
}

void updateTime() {
  // UTC is the time at Greenwich Meridian (GMT)
  // print the hour (86400 equals secs per day)
  nowMs = initialUnixTime * 1000ull + ((uint64_t)millis() - (uint64_t)timeRetreivedInMs);
  nowMs += 3*60*60*1000; // Timezone (UTC+3)

  uint32_t epoch = nowMs/1000ull;
  hours = (epoch % 86400L) / 3600;
  mins = (epoch % 3600) / 60;

  // debugPrint("Time = " + String(hours, DEC) + ":" + String(mins, DEC));
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address) {
  if (sceleton::ntpTime._value == "true") {
    debugPrint("sending NTP packet...");
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12]  = 49;
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;

    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    udp.beginPacket(address, 123); //NTP requests are to port 123
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
  }
}
