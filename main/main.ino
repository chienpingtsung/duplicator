#include <SPI.h>
#include <MFRC522.h>

#define BTN_PIN 3
#define RST_PIN 8
#define LED_PIN 9
#define SDA_PIN 10

MFRC522 mfrc522(SDA_PIN, RST_PIN);

bool save = false;
bool work = false;
int btn;
int led = 0;
int bli = 1;
byte newUid[4];

void setup() {
  Serial.begin(9600);

  SPI.begin();

  mfrc522.PCD_Init();

  pinMode(BTN_PIN, INPUT_PULLUP);

  if (Serial) {
    Serial.println(F("HELLO WORLD"));
  }
}

void loop() {
  btn = digitalRead(BTN_PIN);

  // Serial.print(led);
  // Serial.print(save);
  // Serial.println(work);

  if (work) {
    if (save) {
      analogWrite(LED_PIN, 3);
    } else {
      analogWrite(LED_PIN, led);
      led += bli;
      if (led >= 1) {
        bli = -1;
      }
      if (led <= 0) {
        bli = 1;
      }
    }
  } else {
    analogWrite(LED_PIN, 0);
  }

  delay(50);

  if (btn == LOW) {
    save = false;
    work = true;
  }

  if (work) {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
      return;
    }

    if (save) {
      if (mfrc522.MIFARE_SetUid(newUid, (byte)4, true)) {
        mfrc522.PICC_HaltA();

        if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
          return;
        }

        if (Serial) {
          Serial.print(F("SET NEW ID: "));
          mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
          Serial.println();
        }

        save = false;
        work = false;
      }
    } else {
      if (Serial) {
        Serial.print(F("GET NEW ID: "));
        mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
        Serial.println();
      }

      for (byte i = 0; i < (byte)4; i++) {
        newUid[i] = mfrc522.uid.uidByte[i];
      }

      save = true;
    }
  }
}
