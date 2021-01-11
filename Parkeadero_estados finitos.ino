#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <EEPROM.h>

// estado de la LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
// variables de estado
int stage =0;
// variables de operacion
int ingreso, intermedio, parking, num_muestras;
int aumentos[]= {10,10,10,1};
int led_verde = 4;
int led_amarillo = 3;
int led_rojo = 2;
int trig = 9;
int eco = 8;
int calibrador = 58.2;

void initLedS()
{
  const char *deve = "Sr. Rios ";
  const char *vershon = "parking v0.3";

  lcd.init();
  lcd.backlight();
  lcd.setBacklight(HIGH);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bienvenido ");
  lcd.setCursor(0, 1);
  lcd.print(deve);
  delay(3000);
  lcd.clear();
  lcd.print("Asistente de  ");
  lcd.setCursor(0, 1);
  lcd.print(vershon);
  delay(3000);
  lcd.clear();
}
float tempoSen() // obtiene el tiempo entre ecos del sensor.
{
  float duracion = 0;

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duracion = pulseIn(eco, HIGH);
  delayMicroseconds(50);

  return duracion;
}
float filtradoPromedio(int samplesNumber, float (*funct)()) // *funct vendria a ser --> tempoSen() y promedia la cantidad de samplesNumber muestras
{
  float sum;
  for (int i = 0; i < samplesNumber; i++)
  {
    sum += funct();
  }

  return sum / samplesNumber;
}
float medicionDistancia(float filtrado)
{
  int distancia = filtrado / calibrador;
  Serial.print(distancia);
  lcd.print("D: ");
  lcd.print(String(distancia));
  lcd.setCursor(0, 1);

  return distancia;
}
void ledStatus(int state, int idLed)
{
  // estado : apagar(0),prender(1), intermitente(2)
  // identificador de led : cual led de que pin
  if (state == 2)
  {
    digitalWrite(idLed, HIGH);
    digitalWrite(soundDiv, HIGH);
    delay(400);
    digitalWrite(idLed, LOW);
    digitalWrite(soundDiv, LOW);
  }
  else if (state == 1)
  {
    digitalWrite(idLed, HIGH);
  }
  else
  {
    digitalWrite(idLed, LOW);
  }
}
void setup()
{
  // configuracion inicial para la pantalla con saludo
  initLedS();
  // configuracion del sensor
  pinMode(trig, OUTPUT);
  pinMode(eco, INPUT);
  // configuracion de las luces
  pinMode(led_verde, OUTPUT);
  pinMode(led_amarillo, OUTPUT);
  pinMode(led_rojo, OUTPUT);
  pinMode(soundDiv, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  
}