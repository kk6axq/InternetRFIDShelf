/**
 * Tag_Add.ino
 * Author: Lukas Severinghaus
 * November 22, 2021
 * 
 * Scans a RFID reader on Serial 3 and prints out the tag ID code for a given ID value.
 * This enables easy tag adding to the main script, by printing out the array code pre-formatted.
 * To use, send a number 0<=x<=255 to assign the tag ID number, then scan the tag on the sensor.
 */

void setup() {
  // put your setup code here, to run once:
  Serial3.begin(2400);
  Serial.begin(9600);
}
byte new_tag_id = 0;
void loop() {
  if(Serial.available() > 0){
    new_tag_id = Serial.parseInt();
    Serial.print("New tag ID is: ");
    Serial.println(new_tag_id);
  }
  checkRFID1();
}
byte tag[13];
void checkRFID1() {
  if (Serial3.available() >= 12) {
    Serial3.readBytes(tag, 12);
    Serial.println("Got tag on Serial3.");
    Serial.println("Begin code:");
    Serial.print("{0x");
    Serial.print(new_tag_id, HEX);
    for (int i = 0; i < 12; i++) {
      Serial.print(", 0x");
      Serial.print(tag[i], HEX);
    }
    Serial.println("},");
    Serial.println();
    //Serial.print("Tag is: ");    //Serial.println(result);
  }

}
