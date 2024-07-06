#include <unishox1_progmem.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("Compression test code");
  String str = "mensaje de prueba";

  Serial.println("mensaje: " + str);

  unsigned int bufisze = str.length();
  char buf[bufisze+1];
  char bufcompressed[bufisze+1];

  // char* buf;
  // char* bufcompressed;

  str.toCharArray(buf,bufisze+1,0);

  // Serial.println(buf[0]);

  for(int i = 0;i < bufisze;i++)
  {
    Serial.print(buf[i]);
  }
  Serial.println();

   unishox2_compress_simple(buf,bufisze,bufcompressed);

}

void loop() {
  // put your main code here, to run repeatedly:

}
