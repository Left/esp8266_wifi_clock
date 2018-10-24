#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WebSockets.h>
#include <WebSocketsServer.h>
#include <ESPAsyncWebServer.h>

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

const char* wifiFileName = "wifi.name";
const char* wifiPwdName = "wifi.pwd";

std::auto_ptr<AsyncWebServer> setupServer;
std::auto_ptr<WebSocketsServer> webSocket;

void setup() {
    Serial.begin(115200);
    SPIFFS.begin();

    String wifiName = fileToString(wifiFileName);
    String wifiPwd = fileToString(wifiPwdName);

    if (wifiName.length() > 0 && wifiPwd.length() > 0) {
        WiFi.begin(wifiName.c_str(), wifiPwd.c_str());
        WiFi.waitForConnectResult();
    }

    if (WiFi.status() == WL_CONNECTED) {
        IPAddress ip = WiFi.localIP();
        // Serial.println("Connected to WiFi " + ip.toString());
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
        // Serial.println("ESP AccessPoint IP address: " + accessIP.toString());
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

                // send message to client
                debugPrint("Connected client " + String(num, DEC));
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
                if (type == "wificredentials") {
                    stringToFile(wifiFileName, root["ssid"]);
                    stringToFile(wifiPwdName, root["pwd"]);

                    webSocket->sendTXT(num, "{ \"result\":\"OK, will reboot\" }");
                    ESP.reset();
                } if (type == "ping") {
                    String res = "{ \"result\":\"OK\", \"pingid\":\"";
                    res += (const char*)(root["pingid"]);
                    res += "\" }";
                    webSocket->sendTXT(num, res);
                } if (type == "switch") {
                    // Serial.println("switch!");
                    webSocket->sendTXT(num, "{ \"result\":\"Doing\" }");
                    switchRelaySink(atoi(root["id"]), root["on"] == "true");
                    webSocket->sendTXT(num, "{ \"result\":\"OK\" }");
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
    setupServer->on("/", [](AsyncWebServerRequest *request) {
        String content = "<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 at ";
        content += "<p>";
        content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><input type='submit'></form>";
        content += "</html>";
        request->send(200, "text/html", content);  
    });
    setupServer->onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "Not found: " + request->url());
    });
    setupServer->begin();
}

int lastConnected = millis();

void loop() {
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
