#include "SparkFun_PCA9536_Arduino_Library.h"

PCA9536 io;

void setup()
{
  Serial.begin(115200);
  Serial.println("PCA9536 example");
  Wire.begin();
  // Initialize the PCA9536 with a begin function
  if (io.begin() == false)
  {
    Serial.println("PCA9536 not detected. Please check wiring. Freezing...");
    while (1);
  }

  io.pinMode(0, OUTPUT);
  delay(1000);
  io.pinMode(1, OUTPUT);
  delay(1000);
  io.pinMode(2, INPUT);
  delay(1000);
  io.pinMode(3, INPUT);
  delay(1000);

  Serial.println("1.Apago LED AZUL");
  io.write(0, LOW);
  delay(2000);
  Serial.println("1.Apago LED ROJO");
  io.write(1, LOW);
  delay(2000);   

}

void loop()
{
  Serial.println("1.Apago LED AZUL");
  io.write(0, LOW);
  delay(500);
  Serial.println("1.PRENDo LED AZUL");
  io.write(0, HIGH);
  delay(500);
  Serial.println("1.Apago LED ROJO");
  io.write(1, LOW);
  delay(500);   
  Serial.println("1.Prendo LED ROJO");
  io.write(1, HIGH);
  delay(500);

  Serial.println("1.Apago LED AZUL");
  io.write(0, LOW);
  delay(500);
  Serial.println("1.Apago LED ROJO");
  io.write(1, LOW);
  delay(500);   

  Serial.println("2.PRENDo LED AZUL");
  io.write(0, HIGH);
  delay(500);
  Serial.println("2.Apago LED AZUL");
  io.write(0, LOW);
  delay(500);
  Serial.println("2.Prendo LED ROJO");
  io.write(1, HIGH);
  delay(500);
  Serial.println("2.Apago LED ROJO");
  io.write(1, LOW);
  delay(500);

  Serial.println("2.PRENDo LED AZUL");
  io.write(0, HIGH);
  delay(500);
  Serial.println("2.Prendo LED ROJO");
  io.write(1, HIGH);
  delay(500);

  Serial.println("1.Apago LED AZUL");
  io.write(0, LOW);
  delay(500);
  Serial.println("1.Apago LED ROJO");
  io.write(1, LOW);
  delay(500);   
}

