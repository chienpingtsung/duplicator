#include <SPI.h>
#include <MFRC522.h>

#define BTN_PIN 3
#define RST_PIN 8
#define LED_PIN 9
#define SDA_PIN 10

MFRC522 mfrc522(SDA_PIN, RST_PIN);

bool work = false;
bool save = false;

int led = 0;

byte newUid[4];

void setup() {
  Serial.begin(9600);

  SPI.begin();

  mfrc522.PCD_Init();

  pinMode(BTN_PIN, INPUT_PULLUP);
}

void loop() {
  if (work) {
    if (save) {
      analogWrite(LED_PIN, 1);
    } else {
      analogWrite(LED_PIN, led);
      led = led ? 0 : 1;
    }
  } else {
    analogWrite(LED_PIN, 0);
  }

  if (digitalRead(BTN_PIN) == LOW) {
    work = true;
    save = false;
  }

  if (work) {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
      delay(100);
      return;
    }

    if (save) {
      if (mfrc522.MIFARE_SetUid(newUid, (byte)4, true)) {
        work = false;
        save = false;

        mfrc522.PICC_HaltA();
        if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
          return;
        }

        if (Serial) {
          Serial.println(F("SET NEW ID: "));
          mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
        }
      } else {
        if (Serial) {
          Serial.println(F("ERROR"));
        }
      }
    } else {
      save = true;

      if (Serial) {
        Serial.println(F("GET NEW ID: "));
        mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
      }

      for (byte i = 0; i < (byte)4; i++) {
        newUid[i] = mfrc522.uid.uidByte[i];
      }
    }
  }

  delay(100);
}
