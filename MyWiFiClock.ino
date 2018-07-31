#include "sceleton.h"

#include <WiFiUdp.h>
#include <core_esp8266_waveform.h>

#include "lcd.h"

unsigned int localPort = 2390;      // local port to listen for UDP packets

IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;
LcdScreen screen;

// Opcodes for the MAX7221 and MAX7219
// All OP_DIGITn are offsets from OP_DIGIT0
#define	OP_NOOP         0 ///< MAX72xx opcode for NO OP
#define OP_DIGIT0       1 ///< MAX72xx opcode for DIGIT0
#define OP_DIGIT1       2 ///< MAX72xx opcode for DIGIT1
#define OP_DIGIT2       3 ///< MAX72xx opcode for DIGIT2
#define OP_DIGIT3       4 ///< MAX72xx opcode for DIGIT3
#define OP_DIGIT4       5 ///< MAX72xx opcode for DIGIT4
#define OP_DIGIT5       6 ///< MAX72xx opcode for DIGIT5
#define OP_DIGIT6       7 ///< MAX72xx opcode for DIGIT6
#define OP_DIGIT7       8 ///< MAX72xx opcode for DIGIT7
#define OP_DECODEMODE   9 ///< MAX72xx opcode for DECODE MODE
#define OP_INTENSITY   10 ///< MAX72xx opcode for SET INTENSITY
#define OP_SCANLIMIT   11 ///< MAX72xx opcode for SCAN LIMIT
#define OP_SHUTDOWN    12 ///< MAX72xx opcode for SHUT DOWN
#define OP_DISPLAYTEST 15 ///< MAX72xx opcode for DISPLAY TEST

#define CLK_PIN   D5  // or SCK
#define DATA_PIN  D7  // or MOSI
#define CS_PIN    D6  // or SS
#define BEEPER_PIN D2 // Beeper

void sendCmd(int addr, byte cmd, byte data) {
  digitalWrite(CS_PIN, LOW);
  for(int i = NUM_MAX - 1; i >= 0; i--){
    shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, i == addr ? cmd : 0);
    shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, i == addr ? data : 0);
  }
  digitalWrite(CS_PIN, HIGH);
}

void sendCmdAll(byte cmd, byte data) {
  digitalWrite(CS_PIN, LOW);
  for(int i = NUM_MAX - 1; i >= 0; i--){
    shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, cmd);
    shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, data);
  }
  digitalWrite(CS_PIN, HIGH);
}

unsigned long step = 0;

int testCntr = 0;

void setup() {
  // Initialize comms hardware
  pinMode(CS_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(BEEPER_PIN, OUTPUT);

  digitalWrite(CS_PIN, HIGH);
  sendCmdAll(OP_DISPLAYTEST, 0);
  sendCmdAll(OP_SCANLIMIT, 7);
  sendCmdAll(OP_DECODEMODE, 0);
  sendCmdAll(OP_SHUTDOWN, 1);
  sendCmdAll(OP_INTENSITY, 0); // minimum brightness

  sceleton::setup();

  udp.begin(localPort);

  WiFi.hostByName(ntpServerName, timeServerIP); 
}

unsigned long oldMicros = micros();

void refreshAll() {
  for (int line = 0; line < 8; line++) {
    digitalWrite(CS_PIN, LOW);
    for (int chip = NUM_MAX - 1; chip >= 0; chip--){
      shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, OP_DIGIT0 + line);
      shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, screen.line8(chip * 8 + line));
    }
    digitalWrite(CS_PIN, HIGH);
  }
  digitalWrite(CS_PIN, LOW);
}

uint32_t timeRetreivedInSec = 0;
uint32_t initialUnixTime = 0;
uint32_t timeRequestedAt = 0;

uint16_t hours = 0;
uint16_t mins = 0;
uint64_t epoch = 0;
boolean sleeps = false;

const int updateTimeEachSec = 10; // 600

void loop() {
  if (millis() % 5*60*1000 == 0 && (((millis() - timeRequestedAt) > (timeRetreivedInSec == 0 ? 5 : updateTimeEachSec)*1000))) {
    debugPrint("Requesting time");
    timeRequestedAt = millis();
    sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  }

  // long stepLength = (12000 + (30000*64 / (32 + 32 - abs(32 - step % 64))));
  // debugPrint("stepLength="+String(stepLength, DEC));
  oldMicros = micros();
  // nextPoint();
  step++;
  testCntr++;

  screen.clear();

  if (timeRetreivedInSec) {
    updateTime();
    if (!sleeps) {
      uint64_t micr = 1000000ul*epoch + micros()%1000000ul;
      screen.showTime(micr);

      refreshAll();
    }

    int m = (hours*100 + mins);
    sleeps = m > 2359 || m < 530; // From 22:30 to 5:30 - do not show screen

    if (sleeps) {
      refreshAll();
    }
  } else {
    const wchar_t* getTime = L"  Получаем время с сервера...  ";
    screen.printStr((micros() / 1000 / 50) % screen.getStrWidth(getTime), 0, getTime);
    refreshAll();
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
      const unsigned long seventyYears = 2208988800UL;
      // subtract seventy years:
      unsigned long epoch2 = secsSince1900 - seventyYears;

      timeRetreivedInSec = millis()/1000;
      initialUnixTime = epoch2;
      
      debugPrint("Unix time = " + String(epoch2, DEC));

      // print the hour, minute and second:

      updateTime();
    } else {
      debugPrint("INVALID TIME RECEIVED!");
    }
  }
}

void updateTime() {
  // UTC is the time at Greenwich Meridian (GMT)
  // print the hour (86400 equals secs per day)
  epoch = initialUnixTime + (millis()/1000 - timeRetreivedInSec);
  epoch += 3*60*60; // Timezone (UTC+3)

  hours = (epoch % 86400L) / 3600;
  mins = (epoch % 3600) / 60;

  // debugPrint("Time = " + String(hours, DEC) + ":" + String(mins, DEC));
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address) {
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
