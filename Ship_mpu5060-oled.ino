#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#define OLED_SDA 23
#define OLED_SCL 18

#define MPU_SDA 26
#define MPU_SCL 27

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

#define STAR_COUNT 16
#define ASTEROID_COUNT 4
#define LASER_COUNT 3

TwoWire oledWire = TwoWire(0);
TwoWire mpuWire = TwoWire(1);

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &oledWire,
  OLED_RESET
);

Adafruit_MPU6050 mpu;

struct Star {
  int x;
  float y;
  float speed;
};

struct Asteroid {
  float x;
  float y;
  float speed;
  int radius;
};

struct Laser {
  float x;
  float y;
  bool active;
};

Star stars[STAR_COUNT];
Asteroid asteroids[ASTEROID_COUNT];
Laser lasers[LASER_COUNT];

float shipX = 64;
float shipVelocity = 0;

int score = 0;
int health = 3;

bool gameOver = false;
unsigned long gameOverTime = 0;
unsigned long lastFrame = 0;
unsigned long lastLaserTime = 0;

int limitValue(int value, int minValue, int maxValue) {
  if (value < minValue) return minValue;
  if (value > maxValue) return maxValue;
  return value;
}

void resetStar(int index, bool firstStart) {
  stars[index].x = random(0, 128);
  stars[index].y = firstStart ? random(10, 64) : random(-15, -2);
  stars[index].speed = (float)random(4, 13) / 10.0;
}

void resetAsteroid(int index, bool firstStart) {
  asteroids[index].x = random(8, 120);
  asteroids[index].y = firstStart ? random(10, 54) : random(-25, -5);
  asteroids[index].speed = (float)random(7, 16) / 10.0;
  asteroids[index].radius = random(3, 6);
}

void resetGame() {
  score = 0;
  health = 3;
  gameOver = false;

  shipX = 64;
  shipVelocity = 0;

  for (int i = 0; i < STAR_COUNT; i++) {
    resetStar(i, true);
  }

  for (int i = 0; i < ASTEROID_COUNT; i++) {
    resetAsteroid(i, true);
  }

  for (int i = 0; i < LASER_COUNT; i++) {
    lasers[i].active = false;
  }
}

void fireLaser() {
  for (int i = 0; i < LASER_COUNT; i++) {
    if (!lasers[i].active) {
      lasers[i].active = true;
      lasers[i].x = shipX;
      lasers[i].y = 46;
      return;
    }
  }
}

void showError() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(15, 22);
  display.println("MPU6050 NOT FOUND");

  display.setCursor(12, 36);
  display.println("Check wiring");

  display.display();
}

void showGameOver() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(16, 15);
  display.println("GAME OVER");

  display.setTextSize(1);
  display.setCursor(35, 42);
  display.print("Score: ");
  display.println(score);

  display.display();
}

void setup() {
  Serial.begin(115200);

  oledWire.begin(OLED_SDA, OLED_SCL, 100000);
  delay(1000);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    while (true) {
      delay(100);
    }
  }

  mpuWire.begin(MPU_SDA, MPU_SCL, 100000);
  delay(500);

  if (!mpu.begin(0x68, &mpuWire)) {
    showError();

    while (true) {
      delay(100);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  randomSeed(micros());
  resetGame();
}

void loop() {
  if (millis() - lastFrame < 30) {
    return;
  }

  lastFrame = millis();

  if (gameOver) {
    showGameOver();

    if (millis() - gameOverTime > 2000) {
      resetGame();
    }

    return;
  }

  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;

  mpu.getEvent(&accel, &gyro, &temp);

  // Tilt left and right to control the spaceship
  shipVelocity += accel.acceleration.x * 0.10;
  shipVelocity *= 0.80;
  shipX += shipVelocity;

  shipX = limitValue(shipX, 7, 120);

  float difficulty = 1.0 + (score * 0.04);

  // Move stars
  for (int i = 0; i < STAR_COUNT; i++) {
    stars[i].y += stars[i].speed * difficulty;

    if (stars[i].y > 64) {
      resetStar(i, false);
    }
  }

  // Automatic laser fire
  if (millis() - lastLaserTime > 350) {
    fireLaser();
    lastLaserTime = millis();
  }

  // Move lasers
  for (int i = 0; i < LASER_COUNT; i++) {
    if (lasers[i].active) {
      lasers[i].y -= 3.5;

      if (lasers[i].y < 10) {
        lasers[i].active = false;
      }
    }
  }

  // Move asteroids and check ship collision
  for (int i = 0; i < ASTEROID_COUNT; i++) {
    asteroids[i].y += asteroids[i].speed * difficulty;

    if (asteroids[i].y > 70) {
      resetAsteroid(i, false);
    }

    float shipDX = shipX - asteroids[i].x;
    float shipDY = 52 - asteroids[i].y;

    int hitDistance = asteroids[i].radius + 5;

    if ((shipDX * shipDX + shipDY * shipDY) <
        (hitDistance * hitDistance)) {
      health--;
      resetAsteroid(i, false);

      shipX = 64;
      shipVelocity = 0;

      if (health <= 0) {
        gameOver = true;
        gameOverTime = millis();
      }
    }
  }

  // Laser and asteroid collision
  for (int i = 0; i < LASER_COUNT; i++) {
    if (!lasers[i].active) {
      continue;
    }

    for (int j = 0; j < ASTEROID_COUNT; j++) {
      float dx = lasers[i].x - asteroids[j].x;
      float dy = lasers[i].y - asteroids[j].y;

      int hitDistance = asteroids[j].radius + 2;

      if ((dx * dx + dy * dy) <
          (hitDistance * hitDistance)) {
        lasers[i].active = false;
        resetAsteroid(j, false);
        score++;
        break;
      }
    }
  }

  // Draw frame
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.print("S:");
  display.print(score);

  display.setCursor(45, 0);
  display.print("HP:");
  display.print(health);

  display.setCursor(88, 0);
  display.print("SPACE");

  display.drawLine(0, 9, 127, 9, SSD1306_WHITE);

  // Draw stars
  for (int i = 0; i < STAR_COUNT; i++) {
    display.drawPixel(stars[i].x, (int)stars[i].y, SSD1306_WHITE);
  }

  // Draw lasers
  for (int i = 0; i < LASER_COUNT; i++) {
    if (lasers[i].active) {
      display.drawLine(
        (int)lasers[i].x,
        (int)lasers[i].y,
        (int)lasers[i].x,
        (int)lasers[i].y + 4,
        SSD1306_WHITE
      );
    }
  }

  // Draw asteroids
  for (int i = 0; i < ASTEROID_COUNT; i++) {
    display.drawCircle(
      (int)asteroids[i].x,
      (int)asteroids[i].y,
      asteroids[i].radius,
      SSD1306_WHITE
    );

    display.drawPixel(
      (int)asteroids[i].x - 1,
      (int)asteroids[i].y - 1,
      SSD1306_WHITE
    );
  }

  // Draw spaceship
  display.fillTriangle(
    (int)shipX, 44,
    (int)shipX - 6, 56,
    (int)shipX + 6, 56,
    SSD1306_WHITE
  );

  display.drawLine(
    (int)shipX - 2, 57,
    (int)shipX + 2, 57,
    SSD1306_WHITE
  );

  display.display();
}