#pragma once

#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WebSockets.h>
#include <WebSocketsClient.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>

#ifndef D0
// #define ESP01
#endif

#ifdef ESP01
static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2;
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t D9   = 3;
static const uint8_t D10  = 1;
#endif

void debugPrint(const String& str);

namespace sceleton {

class Sink {
public:
    virtual void showMessage(const char* s, int totalMsToShow) {}
    virtual void showTuningMsg(const char* s) {}
    virtual void setAdditionalInfo(const char* s) {}
    virtual void switchRelay(uint32_t id, bool val) {}
    virtual boolean relayState(uint32_t id) { return false; } 
    virtual void setBrightness(int percents) {}
    virtual void setTime(uint32_t unixTime) {}
    virtual void setLedStripe(std::vector<uint32_t> colors) {}
    virtual void reboot() {}
    virtual void enableScreen(const boolean enabled) {}
    virtual boolean screenEnabled() { return false; }
};

void stringToFile(const String& fileName, const String& value) {
    File f = SPIFFS.open(fileName.c_str(), "w");
    f.write((uint8_t*)value.c_str(), value.length());
    f.close();
}

String fileToString(const String& fileName) {
    if (SPIFFS.exists(fileName.c_str())) {
        File f = SPIFFS.open(fileName.c_str(), "r");
        std::vector<uint8_t> buf(f.size() + 1, 0);
        if (f && f.size()) {
        f.read((uint8_t*)(&buf[0]), buf.size());
        }
        f.close();
        return String((const char*)(&buf[0]));
    }
    return String();
}

const String typeKey("type");

const char* firmwareVersion = "00.20";

std::auto_ptr<AsyncWebServer> setupServer;
std::auto_ptr<WebSocketsClient> webSocketClient;

long vccVal = 0;
long rebootAt = 0x7FFFFFFF;

void send(const String& toSend) {
    webSocketClient->sendTXT(toSend.c_str(), toSend.length());
}

class DevParam {
public:
    const char* _name;
    const char* _description;
    String _value;
    boolean _password;

    DevParam(const char* name, const char* description, String value, boolean pwd=false) :
        _name(name),
        _description(description),
        _value(value),
        _password(pwd) {
    }

    void save() {
        stringToFile(String(_name), _value);
    }
};

DevParam deviceName("device.name", "Device Name", String("ESP_") + ESP.getChipId());
DevParam deviceNameRussian("device.name.russian", "Device Name (russian)", "");
DevParam wifiName("wifi.name", "WiFi SSID", "");
DevParam wifiPwd("wifi.pwd", "WiFi Password", "", true);
DevParam websocketServer("websocket.server", "WebSocket server", "192.168.121.38");
DevParam websocketPort("websocket.port", "WebSocket port", "8080");
#ifndef ESP01
DevParam invertRelayControl("invertRelay", "Invert relays", "false");
DevParam hasScreen("hasScreen", "Has screen", "false");
DevParam hasScreen180Rotated("hasScreen180Rotated", "Screen is rotated on 180", "false");
DevParam hasHX711("hasHX711", "Has HX711 (weight detector)", "false");
DevParam hasIrReceiver("hasIrReceiver", "Has infrared receiver", "false");
DevParam hasDS18B20("hasDS18B20", "Has DS18B20 (temp sensor)", "false");
#endif
DevParam hasBME280("hasBME280", "Has BME280 (temp & humidity sensor)", "false");
DevParam hasLedStripe("hasLedStripe", "Has RGBW Led stripe", "false");
#ifndef ESP01
DevParam hasButton("hasButton", "Has button on D7", "false");
DevParam brightness("brightness", "Brightness [0..100]", "0");
DevParam hasEncoders("hasEncoders", "Has encoders", "false");
DevParam relayNames("relay.names", "Relay names, separated by ;", "");
#endif

DevParam* devParams[] = { 
    &deviceName, 
    &deviceNameRussian,
    &wifiName, 
    &wifiPwd, 
    &websocketServer, 
    &websocketPort, 
#ifndef ESP01
    &invertRelayControl, 
    &hasScreen, 
    &hasScreen180Rotated,
    &hasHX711,
    &hasIrReceiver,
    &hasDS18B20,
#endif
    &hasBME280,
    &hasLedStripe,
    &hasEncoders,
#ifndef ESP01
    &hasButton, 
    &brightness,
    &relayNames
#endif
}; 
Sink* sink = new Sink();
boolean initializedWiFi = false;
uint32_t lastReceived = millis();
uint32_t reconnectWebsocketAt = 0x7FFFFFFF; // Never by def
uint32_t reportedGoingToReconnect = millis();

void reportRelayState(uint32_t id) {
    send("{ \"type\": \"relayState\", \"id\": " + String(id, DEC) + ", \"value\":" + (sink->relayState(id) ? "true" : "false") + " }");
}

void onDisconnect(const WiFiEventStationModeDisconnected& event) {
    // Serial.println("WiFi On Disconnect.");
    // Serial.println(event.reason);
}

std::vector<uint32_t> decodeRGBWString(const char* val) {
    std::vector<uint32_t> resArr;
    for (;;) {
        uint32_t toAdd = 0;
        for (int i = 0; i<8; ++val, ++i) {
            if (*val == 0) {
                return resArr;
            }

            const char c = *val;
            int x = (c >= '0' && c <= '9') ? (c - '0') : (c - 'A' + 10);
            toAdd |= (x << (28 - i*4));
        }
        resArr.push_back(toAdd);
    }
    return resArr;
}

void setup(Sink* _sink) {
    sink = _sink;
    // Serial1.setDebugOutput(true);
    // Serial1.begin(2000000);
    // Serial.begin(2000000);
    Serial.begin(230400);
    SPIFFS.begin();

    long was = millis();
    // Read initial settings
    for (DevParam* d : devParams) {
        String readVal = fileToString(String(d->_name));
        if (readVal.length() > 0) {
            d->_value = readVal;
        }
    }
    Serial.println();
    Serial.println("Initialized in " + String(millis() - was, DEC));

    WiFi.persistent(false);
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(false);

    if (wifiName._value.length() > 0 && wifiPwd._value.length() > 0) {
        WiFi.mode(WIFI_STA);
        WiFi.hostname("ESP_" + deviceName._value);
        WiFi.onStationModeDisconnected(onDisconnect);
        WiFi.begin(wifiName._value.c_str(), wifiPwd._value.c_str());
        // WiFi.waitForConnectResult();
    }
/*
    if (WiFi.status() == WL_CONNECTED) {
        IPAddress ip = WiFi.localIP();
        initializedWiFi = true;
        Serial.println("Connected to WiFi " + ip.toString());
    } else {
        WiFi.mode(WIFI_AP);

        String chidIp = String(ESP.getChipId(), HEX);
        String wifiAPName = ("ESP") + chidIp; // + String(millis() % 0xffff, HEX)
        String wifiPwd = String("pass") + chidIp;
        WiFi.softAP(wifiAPName.c_str(), wifiPwd.c_str(), 3); // , millis() % 5 + 1
        // WiFi.softAPConfig(IPAddress(192, 168, 4, 22), IPAddress(192, 168, 4, 9), IPAddress(255, 255, 255, 0));

        IPAddress accessIP = WiFi.softAPIP();
        Serial.println(String("ESP AccessPoint name       : ") + wifiAPName);
        Serial.println(String("ESP AccessPoint password   : ") + wifiPwd);
        Serial.println(String("ESP AccessPoint IP address : ") + accessIP.toString());

        // sink->showMessage((String("WiFi: ") + wifiAPName + ", password: " + wifiPwd + ", " + accessIP.toString()).c_str(), 0xffff);
    }
*/

    if (websocketServer._value.length() > 0) {
        webSocketClient.reset(new WebSocketsClient());
        auto wsHandler = [&](WStype_t type, uint8_t *payload, size_t length) {
            switch (type) {
                case WStype_CONNECTED: {
                    Serial.println("Connected to server");
                    lastReceived = millis();

                    String devParamsStr = "{ ";
                    bool first = true;
                    for (DevParam* d : devParams) {
                        if (!d->_password) {
                            if (!first) { 
                                devParamsStr += ",";
                            }
                            first = false;
                            devParamsStr += "\"",
                            devParamsStr += d->_name;
                            devParamsStr += "\": \"",
                            devParamsStr += d->_value;
                            devParamsStr += "\" ";
                        }
                    }
                    devParamsStr += "}";

                    // Let's say hello and show all we can
                    send(String("{ ") +
                        "\"type\":\"hello\", " +
                        "\"firmware\":\"" + firmwareVersion + "\", " +
                        "\"afterRestart\": " + millis() + ", " + 
                        "\"devParams\": " + devParamsStr + ", " + 
                        "\"screenEnabled\": " + sink->screenEnabled() + ", " + 
                        "\"deviceName\":\"" + sceleton::deviceName._value + "\"" + 
                        " }");

                    // send message to client
                    // debugPrint("Hello server " + " (" + sceleton::deviceName._value +  "), firmware ver = " + firmwareVersion);
                    Serial.println("Hello sent");

                    #ifndef ESP01
                    int cnt = 0;
                    for (const char* p = sceleton::relayNames._value.c_str(); *p != 0; ++p) {
                        if (*p == ';') {
                            if (cnt == 0) {
                                cnt = 1;
                            }

                            cnt++;
                        }
                    }
                    for (int id = 0; id < cnt; ++id) {
                        send("{ \"type\": \"relayState\", \"id\": " + String(id, DEC) + ", \"value\":" + (sink->relayState(id) ? "true" : "false") + " }");
                    }
                    #endif

                    break;
                }
                case WStype_TEXT: {
                    // Serial.printf("[%u] get Text: %s\n", payload);
                    DynamicJsonDocument jsonBuffer;

                    DeserializationError error = deserializeJson(jsonBuffer, payload);

                    if (error) {
                        // Serial.println("parseObject() failed");
                        send("{ \"errorMsg\":\"Failed to parse JSON\" }");
                        return;
                    }
                    lastReceived = millis();

                    const JsonObject &root = jsonBuffer.as<JsonObject>();

                    String type = root[typeKey];
                    if (type == "ping") {
                        Serial.print(String(millis(), DEC) + ":");Serial.print("Ping "); Serial.print((const char*)(root["pingid"])); Serial.print(" "); Serial.println(WiFi.status());
                        String res = "{ \"type\": \"pingresult\", \"pid\":\"";
                        res += (const char*)(root["pingid"]);
                        res += "\" }";
                        send(res);
                    } else if (type == "switch") {
                        // Serial.println("switch!");
                        bool sw = root["on"] == "true";
                        uint32_t id = atoi(root["id"]);
                        sink->switchRelay(id, sw);
                        reportRelayState(id);
                    } else if (type == "setProp") {
                        for (DevParam* d : devParams) {
                            if (String(d->_name) == root["prop"]) {
                                d->_value = (const char*)(root["value"]);
                                d->save();
                            }
                        }
                    } else if (type == "show") {
                        sink->showMessage(root["text"], root["totalMsToShow"].as<int>());
                    } else if (type == "tune") {
                        sink->showTuningMsg(root["text"]);
                    } else if (type == "unixtime") {
                        sink->setTime(root["value"].as<int>());
                    } else if (type == "ledstripe") {
                        const char* val = (const char*)(root["value"]);
                        sink->setLedStripe(decodeRGBWString(val));
                    #ifndef ESP01
                    } else if (type == "screenEnable") {
                        int val = root["value"].as<boolean>();
                        sink->enableScreen(val);
                        brightness.save();
                    } else if (type == "brightness") {
                        int val = root["value"].as<int>();
                        val = std::max(std::min(val, 100), 0);
                        sink->setBrightness(val);
                        brightness._value = String(val, DEC);
                        brightness.save();
                    #endif
                    } else if (type == "additional-info") {
                        // 
                        sink->setAdditionalInfo(root["text"]);
                    } else if (type == "reboot") {
                        debugPrint("Let's reboot self");
                        sink->reboot();
                    }
                    break;
                }
                case WStype_BIN: {
                    // Serial.printf("[%u] get binary length: %u\n", length);
                    // hexdump(payload, length);
                    debugPrint("Received binary packet of size " + String(length, DEC));

                    // send message to client
                    // webSocketClient.sendBIN(payload, length);
                    break;
                }
                case WStype_DISCONNECTED: {
                    // Serial.print(String(millis(), DEC) + ":"); Serial.printf("Disconnected [%u]!\n", WiFi.status());
                    // Serial.println("Disconnected from server");
                    if (WiFi.status() == WL_CONNECTED) {
                        reconnectWebsocketAt = millis() + 100;
                    }
                    break;
                }
            }
        };

        webSocketClient->onEvent(wsHandler);
    } else {
        // Serial.println("Please configure server to connect");
    }

    setupServer.reset(new AsyncWebServer(80));
    setupServer->on("/http_settup", [](AsyncWebServerRequest *request) {
        bool needReboot = false;
        for (DevParam* d : devParams) {
            if (request->hasParam(d->_name)) {
                // Param is set
                String val = request->getParam(d->_name)->value();
                if (!d->_password || val.length() > 0) {
                    if (d->_value != val) {
                        d->_value = val;
                        d->save();
                        needReboot = true;
                    }
                }
            }
        }
        
        if (needReboot) {
            request->send(200, "text/html", "Settings changed, rebooting...");  
            sink->reboot();
        } else {
            request->send(200, "text/html", "Nothing changed.");  
        }
    });
    setupServer->on("/", [](AsyncWebServerRequest *request) {
        String content = "<!DOCTYPE HTML>\r\n<html>";
        content += "<head>";
        content += "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">";
        content += "</head>";
        content += "<body>";
        content += "<p>";
        content += "<form method='get' action='http_settup'>";
        for (DevParam* d : devParams) {
            content += "<label class='lbl'>";
                content += d->_description;
                content += ":</label>";
            content +="<input name='";
                content += d->_name;
                content += "' value='";
                content += d->_password ? String("") : d->_value;
                content += "' length=32/><br/>";
        }
        content += "<input type='submit'></form>";
        content += "<form action='/reboot'><input type='submit' value='Reboot'/></form>";
        content += "</html>";
        request->send(200, "text/html", content);  
    });
    setupServer->on("/reboot", [](AsyncWebServerRequest *request) {
        rebootAt = millis() + 100;
    });
    setupServer->onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "Not found: " + request->url());
    });
    setupServer->begin();

    // ArduinoOTA.setPort(8266);
    // set host name
    ArduinoOTA.setHostname(deviceName._value.c_str());

    ArduinoOTA.onStart([]() {
        // Serial.println("Start OTA");  //  "Начало OTA-апдейта"
        sink->showMessage("Updating...", 10000);
    });
    ArduinoOTA.onEnd([]() {
        // Serial.println("End OTA");  //  "Завершение OTA-апдейта"
        sink->showMessage("Done...", 10000);
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        // Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            // Serial.println("Auth Failed");
            //  "Ошибка при аутентификации"
        } else if (error == OTA_BEGIN_ERROR) {
            // Serial.println("Begin Failed"); 
            //  "Ошибка при начале OTA-апдейта"
        } else if (error == OTA_CONNECT_ERROR) {
            // Serial.println("Connect Failed");
            //  "Ошибка при подключении"
        } else if (error == OTA_RECEIVE_ERROR) {
            // Serial.println("Receive Failed");
            //  "Ошибка при получении данных"
        } else if (error == OTA_END_ERROR) {
            // Serial.println("End Failed");
            //  "Ошибка при завершении OTA-апдейта"
        }
    });
    // Serial.println("ArduinoOTA.begin");
}

int32_t lastEachSecond = millis() / 1000;
int32_t lastWiFiState = millis();
int32_t lastLoop = millis();

int32_t oldStatus = WiFi.status();

void loop() {
    if (millis() - lastLoop > 50) {
         Serial.println(String("Long loop: ") + String(millis() - lastLoop, DEC));
    }
    lastLoop = millis();

    if (WiFi.status() != WL_CONNECTED && millis() % 4000 == 0) {
        Serial.println(String("WiFi.status() check: ") + WiFi.status());
        bool ret = WiFi.reconnect();
        Serial.println(String("Reconnect returned ") + ret);
    }

    if (oldStatus != WiFi.status()) {
        oldStatus = WiFi.status();
        Serial.println(String("WiFi.status(): ") + WiFi.status());

        if (WiFi.status() == WL_IDLE_STATUS || WiFi.status() == WL_DISCONNECTED) {
            long l = millis();
            Serial.println("Reconnecting");
            WiFi.reconnect();
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println(String("Connected, IP:") + WiFi.localIP().toString());
            reconnectWebsocketAt = millis(); // Wait 1 ms and connect to websocket
            ArduinoOTA.begin(); // Begin OTA immediately
            initializedWiFi = true;
        }
    }

    if (millis() >= reconnectWebsocketAt) {
        reconnectWebsocketAt = 0x7FFFFFFF;
        if (webSocketClient.get() != NULL) {
            Serial.println("webSocketClient connecting " + String(reconnectWebsocketAt, DEC) + " " + String(millis(), DEC));
            webSocketClient->begin(websocketServer._value.c_str(), websocketPort._value.toInt(), "/esp");
        }
    }

    if (initializedWiFi) {
        ArduinoOTA.handle();
        if (webSocketClient.get() != NULL) {
            webSocketClient->loop();
        }
    }

#ifndef ESP01
    if (millis() / 1000 != lastEachSecond) {
        lastEachSecond = millis() / 1000;
        // Set brightness if saved
        sink->setBrightness(brightness._value.toInt());
        // vccVal = ESP.getVcc();
    }
#endif

    if (initializedWiFi) {
        if (millis() - lastReceived > 16000) {
            //Serial.println("Rebooting...");
            if (reportedGoingToReconnect <= lastReceived) {
                sink->showMessage("16 секунд без связи с сервером, перезагружаемся", 30000);
                reportedGoingToReconnect = millis();
            }

            // rebootAt = millis();
        }

        if (rebootAt <= millis()) {
            sink->reboot();
        }
    }
}

} // namespace

void debugPrint(const String& str) {
    if (sceleton::webSocketClient.get() != NULL) {
        String toSend;
        toSend = "{ \"type\": \"log\", \"val\": \"" + str + "\" }";

        sceleton::send(toSend);
    }
}
