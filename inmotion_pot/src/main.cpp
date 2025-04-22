#define BLYNK_TEMPLATE_ID "TMPL6CUC1G2R0"
#define BLYNK_TEMPLATE_NAME "Smart Plant Pot"
#define BLYNK_AUTH_TOKEN "uD7ATH_fTFTLtTbiTgQOOSpBYO1gq5KP"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h> 
#include <Fonts/FreeMono9pt7b.h>
#include <BlynkSimpleEsp32.h>
#include <Buzzer_run.h>
#include <animation.h>

char ssid[] = "Meo";
char pass[] = "meomeomeow";

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define FRAME_DELAY (66)
#define FRAME_WIDTH (64)
#define FRAME_HEIGHT (64)
#define FRAME_COUNT 24

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SOIL_MOISTURE_PIN 32
#define BUZZER_PIN 16
#define RELAY_PIN 18

#define MOISTURE_DRY 2400
#define MOISTURE_LOW 2000
#define MOISTURE_MEDIUM 1500
#define MOISTURE_FULL 1200

BlynkTimer timer;
TaskHandle_t animationTaskHandle;
TaskHandle_t pumpTaskHandle;

int frame = 0;
unsigned long lastFrameTime = 0;
unsigned long stateStartTime = 0;
int lastMoistureState = -1;
bool showAnimation = true;
bool showText = false;
int currentMoistureValue = 0;
bool relayChanged = false;
int currentRelayState = LOW;
bool isWatering = false;
unsigned long wateringStartTime = 0;
unsigned long lastWaterCheckTime = 0;
bool manualWatering = false;

void playSound(int state);

void sendMoistureData() {
  int moistureValue = analogRead(SOIL_MOISTURE_PIN);
  int moisturePercent = map(moistureValue, 4095, 0, 0, 100);
  Blynk.virtualWrite(V0, moisturePercent);
  Blynk.virtualWrite(V2, (currentRelayState == HIGH) ? "ON" : "OFF");
  Serial.print("Moisture Value: ");
  Serial.println(moistureValue);
  Serial.print("Moisture: ");
  Serial.print(moisturePercent);
  Serial.println("%");
}

BLYNK_WRITE(V3) {
  int switchState = param.asInt();
  manualWatering = (switchState == 1);
  if (manualWatering) {
    digitalWrite(RELAY_PIN, HIGH);
    currentRelayState = HIGH;
  } else {
    digitalWrite(RELAY_PIN, LOW);
    currentRelayState = LOW;
  }
}

void displayStartup() {
  display.clearDisplay();
  display.setFont(&FreeMono9pt7b);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 25);
  display.println("Smart Plant");
  display.setCursor(0, 55);
  display.println("Starting...");
  display.display();
}

void animationTask(void *parameter) {
  for (;;) {
    if (!isWatering) {
      int moistureValue = currentMoistureValue;
      const char *text;
      int soundState;
      int animationState;

      if (moistureValue >= MOISTURE_DRY) {
        text = "Dry and Watering";
        soundState = 4;
        animationState = 0;
      } else if (moistureValue >= MOISTURE_LOW) {
        text = "Sad: Low";
        soundState = 3;
        animationState = 3;
      } else if (moistureValue >= MOISTURE_MEDIUM) {
        text = "Smiling: Medium";
        soundState = 2;
        animationState = 2;
      } else {
        text = "Happy! Full";
        soundState = 1;
        animationState = 1;
      }

      if (showAnimation && animationState != 0) {
        if (millis() - lastFrameTime >= FRAME_DELAY) {
          lastFrameTime += FRAME_DELAY;
          display.clearDisplay();
          frame = (frame + 1) % FRAME_COUNT;

          switch (animationState) {
            case 1:
              display.drawBitmap(32, 0, animation_happy[frame], FRAME_WIDTH, FRAME_HEIGHT, SH110X_WHITE);
              break;
            case 2:
              display.drawBitmap(32, 0, animation_smile[frame], FRAME_WIDTH, FRAME_HEIGHT, SH110X_WHITE);
              break;
            case 3:
              display.drawBitmap(32, 0, animation_sad[frame], FRAME_WIDTH, FRAME_HEIGHT, SH110X_WHITE);
              break;
            case 4:
              display.clearDisplay();
              break;
          }

          display.display();
        }

        if (millis() - stateStartTime >= 5000) {
          showAnimation = false;
          showText = true;
          stateStartTime = millis();
          display.clearDisplay();
        }
      } else if (showText) {
        playSound(soundState);
        display.setFont(&FreeMono9pt7b);
        display.setCursor(0, 45);
        display.setTextColor(SH110X_WHITE);
        display.println(text);
        display.display();

        if (millis() - stateStartTime >= 3000) {
          showText = false;
          showAnimation = true;
          stateStartTime = millis();
          frame = 0;
        }
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void pumpTask(void *parameter) {
  static bool relayState = LOW;
  for (;;) {
    if (!manualWatering) {
      int moistureValue = analogRead(SOIL_MOISTURE_PIN);
      currentMoistureValue = moistureValue;

      if (moistureValue > MOISTURE_DRY && !isWatering) {
        relayState = HIGH;
        digitalWrite(RELAY_PIN, relayState);
        currentRelayState = relayState;
        isWatering = true;
        wateringStartTime = millis();
      }

      if (isWatering && millis() - wateringStartTime >= 4000) {
        relayState = LOW;
        digitalWrite(RELAY_PIN, relayState);
        currentRelayState = relayState;
        isWatering = false;
      }
    }
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  if (!display.begin()) {
    Serial.println(F("SH1106 init failed"));
    while (true);
  }

  displayStartup();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(2000L, sendMoistureData);

  pinMode(SOIL_MOISTURE_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  stateStartTime = millis();
  lastFrameTime = millis();
  currentMoistureValue = analogRead(SOIL_MOISTURE_PIN);
  lastMoistureState = (currentMoistureValue < MOISTURE_LOW) ? 3 :
                      (currentMoistureValue < MOISTURE_MEDIUM) ? 2 :
                      (currentMoistureValue < MOISTURE_FULL) ? 1 : 0;
  currentRelayState = LOW;

  xTaskCreatePinnedToCore(animationTask, "AnimationTask", 4096, NULL, 1, &animationTaskHandle, 1);
  xTaskCreatePinnedToCore(pumpTask, "PumpTask", 2048, NULL, 1, &pumpTaskHandle, 0);
}

void loop() {
  Blynk.run();
  timer.run();

  if (isWatering) {
    Blynk.virtualWrite(V1, "Your plant needs water!");
  }
}
