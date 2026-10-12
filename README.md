# 🚀 Motion-Controlled Space Game with ESP32 & OLED

A mini space game built using an **ESP32, MPU6050 motion sensor, and OLED display**. Control your spaceship by tilting the device and enjoy a simple motion-controlled gaming experience!

## 🎮 Project Overview

This project combines embedded systems, motion sensing, and a small OLED display to create a fun interactive space game.

Tilt the device to control the spaceship, avoid obstacles, and explore a mini universe on a tiny screen.

## 🧰 Components Required

- ESP32 development board
- MPU6050 accelerometer and gyroscope sensor
- 0.96-inch OLED display (SSD1306, 128×64)
- Jumper wires
- Breadboard (optional)
- USB cable for programming

## ⚙️ How It Works

1. The MPU6050 detects the device's tilt and movement.
2. The ESP32 reads the sensor data over I2C.
3. The ESP32 processes the movement and updates the game.
4. The OLED display shows the spaceship and game graphics.
5. The player controls the spaceship by tilting the device.

## 🔌 Connections

### MPU6050 to ESP32

| MPU6050 | ESP32 |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 26 |
| SCL | GPIO 27 |

### OLED Display to ESP32

| OLED | ESP32 |
|---|---|
| VCC | 3.3V* |
| GND | GND |
| SDA | GPIO 23 |
| SCL | GPIO 19 |

*These GPIO assignments are example connections for an ESP32 Lite setup. Confirm the actual pins used in your project. Both I2C devices can share SDA and SCL when their addresses do not conflict.*

## 💻 Software Requirements

- Arduino IDE
- ESP32 board package
- Appropriate MPU6050 library
- Adafruit SSD1306 and Adafruit GFX libraries, if used by your code

## 🚀 Getting Started

1. Install the required ESP32 board package and libraries.
2. Connect the MPU6050 and OLED display.
3. Open the Arduino sketch in Arduino IDE.
4. Select your ESP32 board and the correct serial port.
5. Upload the code.
6. Tilt the device and start playing!

## 🌟 Features

- Motion-based spaceship control
- Compact OLED graphics
- ESP32-powered gameplay
- DIY electronics and embedded programming
- Expandable game mechanics

## 🔮 Future Improvements

- Score counter and high-score tracking
- More obstacles and increasing difficulty
- Sound effects and a buzzer
- Start, pause, and restart menus
- Improved spaceship and enemy animations

## 🎥 Watch the Project

Check out the project video on my social media and follow for more DIY electronics, ESP32, robotics, and interactive gaming projects.

## 🤝 Contributing

Ideas and suggestions are welcome! Feel free to fork this project and experiment with your own features.

## 📜 License

This project is shared for educational and personal experimentation. Add a license file if you want to specify reuse permissions.

---

**Built with  using ESP32, MPU6050, and OLED.**

⭐ If you like this project, consider starring the repository!
