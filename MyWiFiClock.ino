#include "sceleton.h"

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_NeoPixel.h>

#ifndef ESP01
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#endif

#ifndef ESP01
#include <SoftwareSerial.h>
#endif

#include "worklogic.h"

#ifndef ESP01
#include "lcd.h"
#include <OneWire.h>
#include <Q2HX711.h>
#endif

unsigned int localPort = 2390;      // local port to listen for UDP packets

IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

const uint64_t dayInMs = 24*60*60*1000;

boolean isScreenEnabled = true;

#ifndef ESP01
LcdScreen screen;
MAX72xx*  screenController = NULL;
#endif

Adafruit_BME280* bme = NULL; // I2C

// #define BEEPER_PIN D2 // Beeper

#ifndef ESP01
IRrecv* irrecv = NULL; // 
#endif

int testCntr = 0;

#ifndef ESP01
Q2HX711* hx711 = NULL;
#endif

#ifndef ESP01
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
#endif

#ifndef ESP01
boolean invertRelayState = false;
boolean relayIsInitialized = false;
SoftwareSerial* relay = NULL; 

OneWire* oneWire;

uint32_t lastMsp430Ping = millis();
HardwareSerial* msp430; // RX, TX
#endif

const int NUMPIXELS = 64;
Adafruit_NeoPixel* stripe = NULL;

#define ULONG_MAX 0xffffffff

const long interval = 1000; // Request each second
unsigned long nextRequest = millis();
unsigned long nextRead = ULONG_MAX;
typedef uint8_t DeviceAddress[8];
DeviceAddress deviceAddress;

int interruptCounter = 0;

uint32_t timeRetreivedInMs = 0;
uint32_t initialUnixTime = 0;
uint32_t timeRequestedAt = 0;
uint32_t restartAt = ULONG_MAX;
uint32_t nextPotentiometer = 0;
uint32_t potentiometerValues[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
uint32_t potentiometerIndex = 0;
int32_t reportedPotentiometer = -1;

std::vector<uint32_t> ledStripe;

void handleInterrupt() {
  interruptCounter++;
}

#ifndef ESP01
boolean encoderPinChanged = false;

class Encoder {
public:
  Encoder(const char* name, int a, int b, int button):
    encName(name), pinA(a), pinB(b), pinButton(button) {
  }

  void process() {
    if (encoderPinChanged) {
      int pA = digitalRead(pinA);
      int pB = digitalRead(pinB);
      int pBtn = digitalRead(pinButton);

      String s = "encoder_";
      s += encName;
      if (pA != _pA || pB != _pB) {
        if (_pA == 0 && _pB == 1 && pA == 1 && pB == 1) {
          String toSend = String("{ \"type\": \"ir_key\", ") + 
            "\"remote\": \"" + s + "\", " + 
            "\"key\": \"" + "rotate_cw" + "\", " +  
            "\"timeseq\": "  + String(millis(), DEC)  + " " +  
            "}";

          sceleton::send(toSend);
        } else if (_pA == 1 && _pB == 0 && pA == 1 && pB == 1) {
          String toSend = String("{ \"type\": \"ir_key\", ") + 
            "\"remote\": \"" + s + "\", " + 
            "\"key\": \"" + "rotate_ccw" + "\", " +  
            "\"timeseq\": "  + String(millis(), DEC)  + " " +  
            "}";

          sceleton::send(toSend);
        }
        _pA = pA;
        _pB = pB;
      }
      if (pBtn != _pBtn) {
        if (_pBtn == 0 && pBtn == 1) {
          String toSend = String("{ \"type\": \"ir_key\", ") + 
            "\"remote\": \"" + s + "\", " + 
            "\"key\": \"" + "click" + "\", " +  
            "\"timeseq\": "  + String(millis(), DEC)  + " " +  
            "}";

          sceleton::send(toSend);
        }
        _pBtn = pBtn;
      }
    }
  }

  static void cont() {
    encoderPinChanged = false;
  }

  void init() {
    const int pins[] = { pinA, pinB, pinButton };
    for (int i = 0; i < __countof(pins); ++i) {
      pinMode(pins[i], INPUT_PULLUP);
      attachInterrupt(digitalPinToInterrupt(pins[i]), pinChange, CHANGE);
    }
    _pA = digitalRead(pinA);
    _pB = digitalRead(pinB);
    _pBtn = digitalRead(pinButton);
  }

private:
  const char* encName;
  const int pinA;
  const int pinB;
  const int pinButton;
  int _pA, _pB, _pBtn;
  
  static void pinChange() {
    encoderPinChanged = true;
  }
};

Encoder encoders[] = {
  Encoder("left", D1, D2, D3),
  Encoder("right", D5, D6, D7),
};
#endif // ESP01

void setup() {
  class SinkImpl : public sceleton::Sink {
  private:
    int currRelayState; // All relays are off by default

  public:
    SinkImpl() : currRelayState(0) {

    }

    virtual boolean relayState(uint32_t id) { 
      int bit = 1 << id;
      return (currRelayState & bit) != 0;
    } 

    virtual void switchRelay(uint32_t id, bool val) {
      // debugSerial.println(String("switchRelaySink: ") + (val ? "true" : "false"));
#ifndef ESP01
      int bit = 1 << id;
      currRelayState = currRelayState & ~bit;
      if (val) {
        currRelayState = currRelayState | bit;
      }

      if (!relayIsInitialized) {
        relay = new SoftwareSerial(D1, D0); // RX, TX
        relay->enableRx(false); // We don't want to receive from it
        relay->begin(9600);
        delay(100);
        relay->write(0x50);
        delay(100);
        relay->write(0x51);
        delay(100);
        relayIsInitialized = true;
      }
      
      relay->write('0' | (sceleton::invertRelayControl._value == "true" ? ~currRelayState : currRelayState));
#endif
      if (sceleton::hasGPIO1Relay._value == "true") {
        digitalWrite(2, val);
      }
    }

    virtual void showMessage(const char* dd, int totalMsToShow) {
#ifndef ESP01
      // 
      screen.showMessage(dd, totalMsToShow);
#endif
    }

    virtual void showTuningMsg(const char* dd) {
#ifndef ESP01
      screen.showTuningMsg(dd);
#endif
    }

    virtual void setAdditionalInfo(const char* dd) {
#ifndef ESP01
      // 
      screen.setAdditionalInfo(dd);
#endif
    }

    virtual void setBrightness(int percents) {
#ifndef ESP01
      if (screenController != NULL) {
        screenController->setBrightness(percents);
      }
#endif
    }

    virtual void setTime(uint32_t unixTime) {
        initialUnixTime = unixTime;
        timeRetreivedInMs = millis();
    }

    virtual void setLedStripe(std::vector<uint32_t> colors) {
      if (stripe != NULL) {
        for (uint16_t i=0; i < colors.size(); i++) {
          const uint32_t clr = colors[i];
          stripe->setPixelColor(i, stripe->Color(
            (clr >> 24) & 0x000000ff, 
            (clr >> 16) & 0x000000ff, 
            (clr >> 8) & 0x000000ff, 
            (clr >> 0) & 0x000000ff) );
        }
        ledStripe.swap(colors);
        stripe->show();
      }
    }

    virtual const std::vector<uint32_t>& getLedStripe() {
      return ledStripe;
    }

    int restartReportedAt = 0;

    virtual void reboot() {
      if (restartAt - millis() >= 200) { 
        if (restartReportedAt < millis()) {
          restartReportedAt = millis() + 300;
          #ifndef ESP01
          // debugSerial.println("Rebooting");
          if (screenController != NULL) {
            debugPrint("Rebooting");
            screen.clear();
            screen.showTuningMsg("Ребут");

            screenController->refreshAll();
          }
          sceleton::webSocketClient->disconnect();
          #endif
        }
        restartAt = millis() + 200;
      }
    }

    virtual void enableScreen(const boolean enabled) {
      isScreenEnabled = enabled;
    }

    virtual boolean screenEnabled() { 
      return isScreenEnabled; 
    }
  };

  sceleton::setup(new SinkImpl());

  if (sceleton::hasLedStripe._value == "true") {
    stripe = new Adafruit_NeoPixel(NUMPIXELS, 0, NEO_GRBW + NEO_KHZ800);
    stripe->begin();
    for (int i = 0; i < NUMPIXELS; ++i) {
      stripe->setPixelColor(i, stripe->Color(0, 0, 0, 0));
    }
    stripe->show();
  }

#ifndef ESP01
  if (sceleton::hasIrReceiver._value == "true") {
    irrecv = new IRrecv(D2);
    irrecv->enableIRIn();  // Start the receiver
  }
#endif

#ifndef ESP01
  if (sceleton::hasDS18B20._value == "true") {
    oneWire = new OneWire(D1);

    oneWire->reset_search();
	  oneWire->search(deviceAddress);
  }
#endif

  if (sceleton::hasBME280._value == "true") {
    Wire.begin(D4, D3);
    Wire.setClock(100000);

    bme = new Adafruit_BME280();
    bool res = bme->begin(0x76);
    if (!res) {
      delete bme;
      bme = NULL;
    }
  }

#ifndef ESP01
  if (sceleton::hasHX711._value == "true") {
    hx711 = new Q2HX711(D5, D6);
  }
#endif

  // Initialize comms hardware
  // pinMode(BEEPER_PIN, OUTPUT);
#ifndef ESP01
  if (sceleton::hasScreen._value == "true") {
    screenController = new MAX72xx(screen, D5, D7, D6, sceleton::hasScreen180Rotated._value == "true");
    screenController->setup();
  }

  if (sceleton::hasButton._value == "true") {
    pinMode(D7, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(D7), handleInterrupt, CHANGE);
  }

  if (sceleton::hasEncoders._value == "true") {
  // if (false) {
    for (int i=0; i < __countof(encoders); ++i) {
      encoders[i].init();
    }

    debugSerial.println("PINS initialized");
  }

  if (sceleton::hasPotenciometer._value == "true") {
    nextPotentiometer = millis() + 100;
  }

  if (sceleton::hasMsp430._value == "true") {
    msp430 = &Serial;
    msp430->begin(9600);
    debugSerial.println("Initialized MSP430");
    pinMode(D2, OUTPUT);
    digitalWrite(D2, 1);
  }

#endif
  if (sceleton::hasGPIO1Relay._value == "true") {
  }
}

unsigned long oldMicros = micros();
uint32_t lastScreenRefresh = millis();

uint16_t hours = 0;
uint16_t mins = 0;
uint64_t nowMs = 0;

const int updateTimeEachSec = 600; // By default, update time each 600 seconds

WiFiClient client;

uint32_t lastWeighteningStarted = millis();
uint32_t lastTemp = millis();
long lastWeight = 0;
uint32_t wrongTempValueReceivedCnt = 0;
long lastStripeFrame = millis();

long lastLoop = millis();
long lastLoopEnd = millis();

void loop() {
  if (millis() - lastLoop > 50) {
        debugSerial.println(String("Long main loop: ") + String(millis() - lastLoop, DEC) + " " + String(millis() - lastLoopEnd, DEC));
  }
  lastLoop = millis();

  long st = millis();

  if (restartAt < st) {
    // debugSerial.println("ESP.reset");
    ESP.reset();
    ESP.restart();
  }

  if (interruptCounter > 0) {
    debugSerial.println("1");
    String toSend = String("{ \"type\": \"button\", ") + 
        "\"value\": " + (digitalRead(D7) == LOW ? "true" : "false") + ", " +  
        "\"timeseq\": "  + String((uint32_t)millis(), DEC)  + " " +  
        "}";

    sceleton::send(toSend);
    interruptCounter = 0;
  }

#ifndef ESP01
  if (sceleton::hasHX711._value == "true" && (millis() - lastWeighteningStarted) > 100 && hx711->readyToSend()) {
    debugSerial.println("3");
    lastWeighteningStarted = millis();

    // debugSerial.println();
    long val = hx711->read();

    String toSend = String("{ \"type\": \"weight\", ") + 
      "\"value\": "  + String(val, DEC)  + ", " +  
      "\"timeseq\": "  + String((uint32_t)millis(), DEC)  + " " +  
      "}";

    sceleton::send(toSend);

    lastWeight = val;
  }
#endif

  if (bme != NULL && ((millis() - lastTemp) > 2000)) {
    debugSerial.println("4");
    lastTemp = millis();
    float hum = bme->readHumidity();
    float temp = bme->readTemperature();
    float pressure = bme->readPressure();

    struct {
        const char* name;
        float value;
    } toSendArr[] = {
      { "temp", temp },
      { "humidity", hum },
      { "pressure", pressure },
    };
    for (int i = 0; i < sizeof(toSendArr)/sizeof(toSendArr[0]); ++i) {
      String toSend = String("{ \"type\": \"") + String(toSendArr[i].name) + String("\", ") + 
          "\"value\": "  + String(toSendArr[i].value)  + ", " +  
          "\"timeseq\": "  + String((uint32_t)millis(), DEC)  + " " +  
          "}";
      sceleton::send(toSend);

    }
    // debugSerial.printf("[%f] [%f] [%f]\n", h, t, p);
  }

#ifndef ESP01
  if (oneWire != NULL) {
    debugSerial.println("5");
    if (millis() > nextRequest) {
      oneWire->reset();
      oneWire->write(0xCC);   //Обращение ко всем датчикам
      oneWire->write(0x44);   //Команда на конвертацию
      nextRead = millis() + interval;
      nextRequest = millis() + interval*2;
    } else if (millis() > nextRead) {
      // debugSerial.println("Temp reading");
      oneWire->reset();
      oneWire->select(deviceAddress);
      oneWire->write(0xBE);            //Считывание значения с датчика
      uint32_t byte1 = oneWire->read();
      uint32_t byte2 = oneWire->read();
      if (byte1 == 0xff && byte2 == 0xff) {
        wrongTempValueReceivedCnt++;
        if (wrongTempValueReceivedCnt % 10 == 0) {
          debugPrint("Wrong temp: " + String(wrongTempValueReceivedCnt, DEC));
        }
        if (wrongTempValueReceivedCnt == 40) {
          // debugSerial.println("Rebooting because of bad temp");
          sceleton::sink->reboot();
        }
      } else {
        wrongTempValueReceivedCnt = 0;
        uint32_t temp = (byte1 << 3 | byte2 << 11); //Принимаем два байта температуры
        float val = (float) temp * 0.0078125;

        // debugPrint("Temp: " + String(byte1, HEX) + " " + String(byte2, HEX) + " -> " + String(val));

        String toSend = String("{ \"type\": \"temp\", ") + 
          "\"value\": "  + String(val)  + ", " +  
          "\"timeseq\": "  + String((uint32_t)millis(), DEC)  + " " +  
          "}";
        sceleton::send(toSend);
      }

      nextRead = ULONG_MAX;
    }
  }
#endif

  oldMicros = micros();
  testCntr++;

#ifndef ESP01
  if (screenController != NULL) {
    if (millis() > (lastScreenRefresh + 20)) {
      lastScreenRefresh = millis();
      screen.clear();

      if (sceleton::initializedWiFi && timeRetreivedInMs != 0) {
        if (isScreenEnabled) {
          // UTC is the time at Greenwich Meridian (GMT)
          // print the hour (86400 equals secs per day)
          nowMs = initialUnixTime * 1000ull + ((uint64_t)millis() - (uint64_t)timeRetreivedInMs);
          nowMs += 3*60*60*1000; // Timezone (UTC+3)

          uint32_t epoch = nowMs/1000ull;
          hours = (epoch % 86400L) / 3600;
          mins = (epoch % 3600) / 60;

          screen.showTime(nowMs / dayInMs, nowMs % dayInMs);
        }
        screenController->refreshAll();
      } else {
        screen.clear();
        screen.set(0, 0, OnePixelAt(Rectangle(0, 0, 32, 8), (millis() / 30) % (32*8)), true);
        screenController->refreshAll();
      }
    }
  }
#endif

  sceleton::loop();

#ifndef ESP01
  if (irrecv != NULL) {
    decode_results results;
    if (irrecv->decode(&results)) {
      if (results.rawlen > 30) {
        int decodedLen = 0;
        char decoded[300] = {0};
        int prevL = 0;
        for (int i = 0; i < results.rawlen && i < sizeof(decoded); ++i) {
          char c = -1;
          int val = results.rawbuf[i];
          
          String valStr = String(val, DEC);
          for (;valStr.length() < 4;) valStr = " " + valStr;

          if (val > 1000) {
            continue;
          } else if ((prevL + val) > 150 && (prevL + val) < 500) {
            c = '0';
          } else if ((prevL + val) > 600 && (prevL + val) < 900) {
            c = '1';
          } else {
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
              //debugSerial.println(String(recognized->value));

              recognizedRemote = &remote;
              kk = k;

              String keyVal(remote.keys[k].value);
              debugSerial.println(keyVal);

              String toSend = String("{ \"type\": \"ir_key\", ") + 
                "\"remote\": \"" + String(recognizedRemote->name) + "\", " + 
                "\"key\": \"" + keyVal + "\", " +  
                "\"timeseq\": "  + String(millis(), DEC)  + " " +  
                "}";

              sceleton::send(toSend);

              break;
            }
          }
        }

        if (recognized == NULL) {
          // debugPrint(decoded);
          debugSerial.println("Unrecognized");
        }
      }

      irrecv->resume();  // Receive the next value
    }
  }

  if (msp430 != NULL && sceleton::webSocketClient.get() != NULL) {
    for (;msp430->available() > 0;) {
      int ch = msp430->read();
      lastMsp430Ping = millis();
      const char encoders[] = { 'A', 'G', 'O' };
      const char* encoderNames[] = { "left", "middle", "right" };
      if (ch == 'Z') {
        // restart
        debugPrint("MSP430 started");
        debugSerial.println("MSP430 started");
      } else if (ch == '0') {
        // ping
      } else {
        // Encoders
        for (int enc = 0; enc < __countof(encoders); ++enc) {
          const char* ss = NULL;
          if (encoders[enc] + 1 == ch) {
            ss = "rotate_cw";
          } else if (encoders[enc] + 2 == ch) {
            ss = "rotate_ccw";
          } else if (encoders[enc] + 3 == ch) {
            ss = "click";
          }
          if (ss != NULL) {
            String s = "encoder_";
            s += encoderNames[enc];
            String toSend = String("{ \"type\": \"ir_key\", ") + 
              "\"remote\": \"" + s + "\", " + 
              "\"key\": \"" + ss + "\", " +  
              "\"timeseq\": "  + String(millis(), DEC)  + " " +  
              "}";
            // debugSerial.printf("> %s %s\n", s.c_str(), ss);

            sceleton::send(toSend);
          }
        }
      }
    }
    if (millis() - lastMsp430Ping > 3000) {
      // debugPrint("MSP430 didn't ping us for 3seconds, let's restart it");
      digitalWrite(D2, 0);
      debugSerial.println("MSP430 didn't ping us for 3seconds, let's restart it");
      debugPrint("MSP430 didn't ping us for 3seconds, let's restart it");
      delay(50);
      digitalWrite(D2, 1);
      lastMsp430Ping = millis();
    }
  }
#endif
  // debugSerial.println(String(millis(), DEC));
#ifndef ESP01
  // Process encoders
  for (int i = 0; i < __countof(encoders); ++i) {
    encoders[i].process();
  }
  Encoder::cont();

  if (sceleton::hasPotenciometer._value == "true" && (nextPotentiometer < millis())) {
    nextPotentiometer = millis() + 50;
    int readingIn = analogRead(A0);
    potentiometerValues[potentiometerIndex++ % __countof(potentiometerValues)] = readingIn;

    String s = "";
    int32_t total = 0;
    for (uint8_t ind = 0; ind < __countof(potentiometerValues); ++ind) {
      total += potentiometerValues[ind];
      s += " ";
      s += String(potentiometerValues[ind], DEC);
    }

    const int32_t maxVol = 939;
    const int32_t minVol = 830;
    const int32_t distance = (maxVol - minVol);
    readingIn = 100 - (std::min(maxVol, std::max(minVol, (int32_t)(total/__countof(potentiometerValues)))) - minVol) * 100 / distance;

    if (reportedPotentiometer != readingIn) {
      if (sceleton::webSocketClient.get() != NULL) {
        String toSend = String("{ \"type\": \"potentiometer\", ") + 
            "\"value\": \"" + readingIn + "\", " +  
            "\"timeseq\": "  + String(millis(), DEC)  + " " +  
            "}";
        sceleton::send(toSend);
        reportedPotentiometer = readingIn;
      }
    }
  }
#endif

  lastLoopEnd = millis();
}

