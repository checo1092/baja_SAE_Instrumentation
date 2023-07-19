/*
 SineWavePoints
 
 Write sine wave points to the serial port, followed by the Carriage Return and LineFeed terminator.
 */

int i = 0;

// The setup routine runs once when you press reset:
void setup() {
  // Initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
}

// The loop routine runs over and over again forever:
void loop() {
  // Write the sinewave points, followed by the terminator "Carriage Return" and "Linefeed".
  Serial.print(i++);
  Serial.print(",");
  Serial.print(2*i);
  Serial.print(",");
  Serial.println(3*i);
  //Serial.print(sizeof(i));
  delay(100);
}
