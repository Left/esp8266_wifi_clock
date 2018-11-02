#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WebSockets.h>
#include <WebSocketsServer.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>
#include <ESP8266httpUpdate.h>

void debugPrint(const String& str);

namespace sceleton {

std::function<void(const char*)> showMessageSink = [](const char* s) {}; // Do nothing by default
std::function<void(int, bool)> switchRelaySink = [](int, bool) {}; // Do nothing by default
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

const char* firmwareVersion = "00.11";

std::auto_ptr<AsyncWebServer> setupServer;
std::auto_ptr<WebSocketsServer> webSocket;

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
};

DevParam deviceName("device.name", "Device Name", String("ESP_") + ESP.getChipId());
DevParam wifiName("wifi.name", "WiFi SSID", "");
DevParam wifiPwd("wifi.pwd", "WiFi Password", "", true);
DevParam ntpTime("ntpTime", "Get NTP time", "true");
DevParam invertRelayControl("invertRelay", "Invert relays", "false");

DevParam* devParams[] = { &deviceName, &wifiName, &wifiPwd, &ntpTime, &invertRelayControl }; 

void setup() {
    Serial.begin(115200);
    SPIFFS.begin();

    // Read initial settings
    for (DevParam* d : devParams) {
        String readVal = fileToString(String(d->_name));
        if (readVal.length() > 0) {
            d->_value = readVal;
        }
    }

    if (wifiName._value.length() > 0 && wifiPwd._value.length() > 0) {
        WiFi.begin(wifiName._value.c_str(), wifiPwd._value.c_str());
        WiFi.waitForConnectResult();
    }

    if (WiFi.status() == WL_CONNECTED) {
        IPAddress ip = WiFi.localIP();
        Serial.println("Connected to WiFi " + ip.toString());
    } else {
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();

        String networks("[");
        int n = WiFi.scanNetworks();
        for (int i = 0; i < n; i++) {
            if (networks.length() > 1) {
                networks += ", ";
            }
            networks += "\"" + WiFi.SSID(i) + "\"";
        }

        WiFi.mode(WIFI_AP);
        String chidIp = String(ESP.getChipId(), HEX);
        String wifiAPName = ("ESP8266_Remote_") + chidIp;
        String wifiPwd = String("pwd") + chidIp;
        WiFi.softAP(wifiAPName.c_str(), wifiPwd.c_str());

        IPAddress accessIP = WiFi.softAPIP();
        Serial.println(String("ESP AccessPoint name       : ") + wifiAPName);
        Serial.println(String("ESP AccessPoint password   : ") + wifiPwd);
        Serial.println(String("ESP AccessPoint IP address : ") + accessIP.toString());
    }

    webSocket.reset(new WebSocketsServer(8081, "*"));
    webSocket->onEvent([&](uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
        switch (type) {
            case WStype_DISCONNECTED: {
                // Serial.printf("[%u] Disconnected!\n", num);
                break;
            }
            case WStype_CONNECTED: {
                IPAddress ip = webSocket->remoteIP(num);
                // Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

                webSocket->sendTXT(num, String("{ \"type\":\"hello\", \"firmware\":\"") + firmwareVersion + "\" }");

                // send message to client
                debugPrint("Connected client " + String(num, DEC) + " (" + sceleton::deviceName._value +  "), firmware ver = " + firmwareVersion);
                break;
            }
            case WStype_TEXT: {
                // Serial.printf("[%u] get Text: %s\n", num, payload);
                DynamicJsonDocument jsonBuffer;

                DeserializationError error = deserializeJson(jsonBuffer, payload);

                if (error) {
                    // Serial.println("parseObject() failed");
                    webSocket->sendTXT(num, "{ \"errorMsg\":\"Failed to parse JSON\" }");
                    return;
                }

                const JsonObject &root = jsonBuffer.as<JsonObject>();

                String type = root[typeKey];
                if (type == "ping") {
                    String res = "{ \"result\":\"OK\", \"pingid\":\"";
                    res += (const char*)(root["pingid"]);
                    res += "\" }";
                    webSocket->sendTXT(num, res);
                } else if (type == "switch") {
                    // Serial.println("switch!");
                    webSocket->sendTXT(num, "{ \"result\":\"Doing\" }");
                    switchRelaySink(atoi(root["id"]), root["on"] == "true");
                    webSocket->sendTXT(num, "{ \"result\":\"OK\" }");
                } else if (type == "firmware-update") {
                    showMessageSink("Update");
                    t_httpUpdate_return ret = ESPhttpUpdate.update("192.168.121.38", 8080, "/esp8266/update");
                    Serial.println("Update 3 " + String(ret, DEC));
                    switch(ret) {
                        case HTTP_UPDATE_FAILED:
                            Serial.println(String("[update] Update failed.") + ESPhttpUpdate.getLastErrorString());
                            webSocket->sendTXT(num, "{ \"result\":\"Fail\", \"description\": \"" + ESPhttpUpdate.getLastErrorString() + "\" }");
                            break;
                        case HTTP_UPDATE_NO_UPDATES:
                            Serial.println("[update] Update no Update.");
                            webSocket->sendTXT(num, "{ \"result\":\"No update\" }");
                            break;
                        case HTTP_UPDATE_OK:
                            Serial.println("[update] Update ok."); // may not called we reboot the ESP
                            break;
                    }
                } else if (type == "show") {
                    showMessageSink(root["text"]);
                }

                break;
            }
            case WStype_BIN: {
                // Serial.printf("[%u] get binary length: %u\n", num, length);
                // hexdump(payload, length);
                debugPrint("Received binary packet of size " + String(length, DEC));

                // send message to client
                // webSocket.sendBIN(num, payload, length);
                break;
            }
        }
    });
    webSocket->begin();

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
                        stringToFile(String(d->_name), d->_value);
                        needReboot = true;
                    }
                }
            }
        }
        
        if (needReboot) {
            request->send(200, "text/html", "Settings changed, rebooting...");  
            ESP.reset();
        } else {
            request->send(200, "text/html", "Nothing changed.");  
        }
    });
    setupServer->on("/", [](AsyncWebServerRequest *request) {
        String content = "<!DOCTYPE HTML>\r\n<html><body>";
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
        content += "</html>";
        request->send(200, "text/html", content);  
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
        showMessageSink("Updating...");
    });
    ArduinoOTA.onEnd([]() {
        // Serial.println("End OTA");  //  "Завершение OTA-апдейта"
        showMessageSink("Done...");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        // Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
            //  "Ошибка при аутентификации"
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed"); 
            //  "Ошибка при начале OTA-апдейта"
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
            //  "Ошибка при подключении"
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
            //  "Ошибка при получении данных"
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
            //  "Ошибка при завершении OTA-апдейта"
        }
    });
    ArduinoOTA.begin();
    // Serial.println("ArduinoOTA.begin");
}

int lastConnected = millis();

void loop() {
    ArduinoOTA.handle();
    webSocket->loop();

    if (millis() % 1000 == 0 && WiFi.status() != WL_CONNECTED) {
        if (millis() - lastConnected > 15000) {
            ESP.reset();
        }
    } else {
        lastConnected = millis();
    }
}

} // namespace

void debugPrint(const String& str) {
  if (sceleton::webSocket.get()) {
    String toSend;
    toSend = "{ \"type\": \"log\", \"val\": \"" + str + "\" }";
    sceleton::webSocket->broadcastTXT(toSend.c_str(), toSend.length());
  }
}
