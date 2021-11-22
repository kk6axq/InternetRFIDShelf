#include <SoftwareSerial.h>

#define ENC_A 31
#define ENC_B 33
#define ENC_BUTTON 35
#define MODE_ADD 0
#define MODE_REMOVE 1
#define MODE_HYBRID 2
int mode = MODE_HYBRID;

int count = 0;
int min_inventory = 4;
#define BUZZER_PIN 37
//Front sensor enable pin
#define RFID_1_ENABLE 42
//Rear sensor enable pin
#define RFID_2_ENABLE 44
#define RFID_2_RX 66
#define SCAN_TIME 500

SoftwareSerial rfid2(RFID_2_RX, 3);

//Adapted from https://raw.githubusercontent.com/ellensp/rrd-glcd-tester/master/rrd-glcd-test.ino
#define DOGLCD_CS       16
#define DOGLCD_MOSI     17
#define DOGLCD_SCK      23
#include <SPI.h>
#include <U8glib.h>
U8GLIB_ST7920_128X64_1X u8g(DOGLCD_SCK, DOGLCD_MOSI, DOGLCD_CS);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Start");
  Serial1.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  pinMode(ENC_BUTTON, INPUT_PULLUP);
  //Serial1.print("Test");
  initRFID();
  u8g.setFont(u8g_font_helvR08);
  u8g.setColorIndex(1);
}
byte inventory_status[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
byte inventory_items = 15;
bool inventory_update = true;
void loop() {

  int inbound_tag = checkRFID2(); //Check for new tag being added
  int outbound_tag = checkRFID1(); //Check for tag being removed

  if (inbound_tag != -1) {
    Serial.print("Scanning in tag ");
    Serial.println(inbound_tag);
    //New tag read
    if (inventory_status[inbound_tag] != 1) {
      inventory_update = true;
      inventory_status[inbound_tag] = 1;
    }
  }

  if (outbound_tag != -1) {
    //Outbound tag read
    if (inventory_status[outbound_tag] != 0) {
      inventory_update = true;
      inventory_status[outbound_tag] = 0;
    }
  }

  if (inventory_update) {
    inventory_update = false;
    Serial.println("Inventory updated:");
    count = 0;
    for (int i = 0; i < inventory_items + 1; i++) {
      count += inventory_status[i];
    }
    Serial.print(count);
    Serial.println(" items");
    sendInventory(count, min_inventory);
    drawScreen(count);
  }
  /**
    if (Serial.available() > 0) {
    byte b = Serial.read();
    if (b == 'a' || b == 'A') {
      Serial.println("Add mode");
      mode = MODE_ADD;
    } else if (b == 'r' || b == 'R') {
      Serial.println("Remove mode");
      mode = MODE_REMOVE;
    }
    }
    int new_tag = checkRFID1();
    if (new_tag != -1) {
    //A tag was scanned.
    if (mode == MODE_ADD) {
      if (inventory_status[new_tag] != 1) {
        inventory_update = true;
      }
      inventory_status[new_tag] = 1;
    } else if (mode == MODE_REMOVE) {
      if (inventory_status[new_tag] != 0) {
        inventory_update = true;
      }
      inventory_status[new_tag] = 0;
    }
    }

    if (inventory_update) {
    inventory_update = false;
    Serial.println("Inventory updated:");
    count = 0;
    for (int i = 0; i < inventory_items+1; i++) {
      count += inventory_status[i];
    }
    Serial.print(count);
    Serial.println(" items");
    sendInventory(count, min_inventory);
    drawScreen(count);
    }

    if (digitalRead(ENC_BUTTON) == 0) {
    //Button pressed
    if (mode == MODE_REMOVE) {
      Serial.println("Add mode");
      mode = MODE_ADD;
      drawScreen(count);

    } else if (mode == MODE_ADD) {
      Serial.println("Remove mode");
      mode = MODE_REMOVE;
      drawScreen(count);

    }
    delay(15);
    while (digitalRead(ENC_BUTTON) == 0);
    }
  **/
}


void initRFID() {
  Serial3.begin(2400);
  rfid2.begin(2400);
  pinMode(RFID_1_ENABLE, OUTPUT);
  digitalWrite(RFID_1_ENABLE, HIGH);
  pinMode(RFID_2_ENABLE, OUTPUT);
  digitalWrite(RFID_2_ENABLE, HIGH);
}

byte tag[13];

//Front scanner
int checkRFID1() {
  //Enable the RFID sensor and give it time to power up
  digitalWrite(RFID_1_ENABLE, LOW);
  Serial3.flush();
  delay(SCAN_TIME);
  if (Serial3.available() >= 12) {
    Serial3.readBytes(tag, 12);
    Serial.print("Got tag on front scanner: ");
    for (int i = 0; i < 12; i++) {
      Serial.print(tag[i], HEX);
    }
    Serial.println();
    Serial.print("Tag is: ");
    int result = tag_search();
    Serial.println(result);
    beep();
    digitalWrite(RFID_1_ENABLE, HIGH);
    return result;
  }
  digitalWrite(RFID_1_ENABLE, HIGH);
  return -1;
}

int checkRFID2() {
  //Enable the RFID sensor and give it time to power up
  digitalWrite(RFID_2_ENABLE, LOW);
  rfid2.flush();
  delay(SCAN_TIME);
  if (rfid2.available() >= 12) {
    rfid2.readBytes(tag, 12);
    Serial.print("Got tag on rear scanner: ");
    for (int i = 0; i < 12; i++) {
      Serial.print(tag[i], HEX);
    }
    Serial.println();
    //Serial.print("Tag is: ");
    int result = tag_search();
    //Serial.println(result);
    beep();
    digitalWrite(RFID_2_ENABLE, HIGH);
    return result;
  }
  digitalWrite(RFID_2_ENABLE, HIGH);
  return -1;
}

void sendInventory(int value, int min_inv) {
  Serial1.print('a');
  Serial1.print(value);
  Serial1.print(",");
  Serial1.print(min_inv);
}

/*
  if(Serial1.available() > 0){
  Serial.write(Serial1.read());
  }
  if(Serial.available() > 0){
  byte b = Serial.read();
  Serial1.write(b);
  Serial.write(b);
  }
*/

byte tag_array[][14] = {
  {0x01, 0xA, 0x30, 0x38, 0x30, 0x30, 0x32, 0x46, 0x45, 0x35, 0x41, 0x32, 0xD}, //White tag
  {0x02, 0xA, 0x33, 0x35, 0x30, 0x32, 0x31, 0x44, 0x43, 0x36, 0x33, 0x32, 0xD}, //Blue keychain
  {0x03, 0xA, 0x31, 0x31, 0x30, 0x30, 0x36, 0x38, 0x43, 0x33, 0x43, 0x43, 0xD}, //White circle
  {0x04, 0xA, 0x30, 0x31, 0x30, 0x44, 0x35, 0x44, 0x45, 0x31, 0x43, 0x30, 0xD},
  {0x5, 0xA, 0x36, 0x30, 0x30, 0x30, 0x36, 0x31, 0x43, 0x39, 0x33, 0x35, 0xD}, //Sticker tags
  {0x6, 0xA, 0x36, 0x30, 0x30, 0x30, 0x36, 0x31, 0x45, 0x38, 0x32, 0x30, 0xD},
  {0x7, 0xA, 0x36, 0x30, 0x30, 0x30, 0x36, 0x31, 0x42, 0x30, 0x38, 0x41, 0xD},
  {0x8, 0xA, 0x36, 0x30, 0x30, 0x30, 0x36, 0x31, 0x45, 0x46, 0x44, 0x35, 0xD},
  {0x9, 0xA, 0x36, 0x30, 0x30, 0x30, 0x36, 0x31, 0x46, 0x41, 0x39, 0x35, 0xD},
  {0xA, 0xA, 0x36, 0x30, 0x30, 0x30, 0x36, 0x31, 0x43, 0x44, 0x45, 0x31, 0xD},
  {0xB, 0xA, 0x36, 0x30, 0x30, 0x30, 0x36, 0x31, 0x46, 0x41, 0x43, 0x31, 0xD},
  {0xC, 0xA, 0x36, 0x30, 0x30, 0x30, 0x36, 0x31, 0x35, 0x45, 0x30, 0x39, 0xD},
  {0xD, 0xA, 0x36, 0x30, 0x30, 0x30, 0x36, 0x31, 0x41, 0x43, 0x45, 0x35, 0xD},
  {0xE, 0xA, 0x36, 0x30, 0x30, 0x30, 0x36, 0x31, 0x45, 0x41, 0x31, 0x34, 0xD}, //End sticker tags
  {0xF, 0xA, 0x30, 0x38, 0x30, 0x30, 0x32, 0x45, 0x34, 0x43, 0x30, 0x45, 0xD} //White card #2
}; //Black circle

const int num_tags_in_array = 15;

/**
   Looks for match between current tag and stored tag ids.
   Returns tag ID number or -1 if no match found.
*/
int tag_search() {
  //Iterate over every recorded tag.
  for (int i = 0; i < num_tags_in_array; i++) {
    bool match = true;
    //Serial.print("Comparing to id ");
    //Serial.println(i);
    //Iterate over every byte
    for (int j = 0; j < 12; j++) {
      //If it doesn't match, then set match to false
      //j+1 is because the first byte is the tag ID.
      //Serial.print(tag[j]== tag_array[i][j+1]);
      //Serial.print(", ");
      //Serial.print(tag[j], HEX);
      //Serial.print(", ");
      //Serial.println(tag_array[i][j+1], HEX);
      if (tag[j] != tag_array[i][j + 1]) {
        match = false;
      }
    }
    //If it was a full match, return the tag id, the first byte
    if (match) {
      return tag_array[i][0];
    }
  }
  //No match found, return -1
  return -1;
}

void beep() {
  tone(BUZZER_PIN, 700, 10);
}


void drawScreen(int value) {
  u8g.firstPage();
  do {
    char cstr[16];
    u8g.drawStr(0, 10, "Current Inventory:");
    u8g.drawStr(0, 22, itoa(value, cstr, 10));
    u8g.drawStr(0, 34, "Inventory Status:");
    u8g.drawStr(0, 46, (value < min_inventory) ? "LOW" : "OK");
    u8g.drawStr(0, 58, "Mode:");
    u8g.drawStr(50, 58, (mode == MODE_HYBRID) ? "HYBRID" : ((mode == MODE_ADD) ? "ADD" : "REMOVE"));
  } while (u8g.nextPage());
}
