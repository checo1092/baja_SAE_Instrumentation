#include <Wire.h>
// Include the required Wire library for I2C<br>#include <Wire.h>
int LED = 2;
int x = 0;
void setup() {
  // Define the LED pin as Output
  pinMode (LED, OUTPUT);
  // Start the I2C Bus as Slave on address 9
  Wire.begin(9); 
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);

  Serial.begin(115200);
}
void receiveEvent(int bytes) {
  x = Wire.read();    // read one character from the I2C
  Serial.print("Received: ");
  Serial.println(x);
}
void loop() {
  Serial.print("Receiver: ");
  Serial.println(x);
  //If value received is 0 blink LED for 200 ms
  if (x == 0) {
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
  }
  //If value received is 3 blink LED for 400 ms
  if (x == 3) {
    digitalWrite(LED, HIGH);
    delay(400);
    digitalWrite(LED, LOW);
    delay(400);
  }
}