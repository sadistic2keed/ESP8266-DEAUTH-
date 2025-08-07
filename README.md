# 📡 ESP8266-Deauther

A powerful tool that deauthenticates 2.4GHz WiFi networks through a serial console.

---

## 🚀 Key Features

* ⚙️ **Simple Setup** – Ready in minutes.
* 💡 **LED Indicator** – LED blinks when deauth attack begins.
* 🧠 **User-Friendly** – Easy to use and manage.

---

## 🔧 Setup Guide

1. 📥 [Download Arduino IDE](https://www.arduino.cc/en/software) based on your OS.
2. 💽 Install it.
3. ⚙️ Navigate to `File` → `Preferences` → `Additional Boards Manager URLs`.
4. 📋 Paste the following URL:

   ```
   https://raw.githubusercontent.com/SpacehuhnTech/arduino/main/package_spacehuhn_index.json
   ```
5. ✅ Click `OK`.
6. 🧭 Go to `Tools` → `Board` → `Board Manager`.
7. ⌛ Wait a moment and search for `deauther` by **Spacehuhn Technologies**.
8. 📦 Install the package.
9. 🔁 Restart the Arduino IDE.

---

## 📲 Installation (via Arduino IDE)

1. 📂 Download or clone this repository.
2. 📄 Open the folder and double-click `Deauther.ino`.
3. 🛠️ Compile the sketch.
4. 📍 Select your board:

   * Navigate to `Tools` → `Board` → `Deauther ESP8266 Boards`
   * Choose **NodeMCU**
5. 🔌 Select the correct **PORT**.
6. ⬆️ Upload the code to your device.

---

## 🧪 How to Use

1. 🌐 Open the [Serial Console](https://wirebits.github.io/SerialConsole/).
2. 🔧 Set **baud rate** to `115200`.
3. 🖱️ Click `Connect`.
4. ✅ When you see: `Connected! Go On!`, you're ready!

   * ❌ If not, repeat steps 2 and 3.
5. ⌨️ Type `help` to see available commands.

---

> ⚠️ **Disclaimer**: This tool is intended for educational and authorized security testing purposes only. Unauthorized use is illegal.
