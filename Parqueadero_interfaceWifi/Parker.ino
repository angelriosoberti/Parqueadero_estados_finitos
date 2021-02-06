

#include <Wire.h>
#include <LedControl.h>
#include <EEPROM.h>

//Los pines deben estar sipre conectados a puertos PWM
// DEBEN SER CONECTADOS EN PINES CON PWM EN EL NODEMCU ES 2,5,6 Y 7
int DIN = 12; //
int CLK = 11; //
int CS = 10;  //
int estados[] = {8, 7, 6, 5, 4, 3, 2, 1};
int eco = 7;
int trig = 8;
int intensidad; // La intensidad es de luz de la matriz son  14 niveles
int distDeteccion_ino = 100;
int distAlto_ino = 5;
int memDistDeteccion_ino;
int memDistAlto_ino;
int kyori;
int detector = 0;
int distancia = 5;

LedControl lc = LedControl(DIN, CLK, CS, 1);

int lumen()
{
  // tomo lectura de un puerto analogico
  int lecturaLuz = analogRead(A2);
  int luzLevel; // este numero debe ser entre 1 y 14
  // dividor de voltaje vcc a cupula y a tierra  330R
  luzLevel = map(lecturaLuz, 0, 900, 3, 14);
  return luzLevel;
}
void matrixDraw(int columna)
{
  if (columna >= 9)
  {
    Serial.print("ALERTA PROBLEMA DE CARGADO DE ILAS EXCESIVAS");
  }
  else
  {
    for (int fila = 0; fila < 8; fila++)
    {
      for (int row = 0; row < columna; row++)
      {
        lc.setLed(0, fila, row, true);
        delay(1);
      }
    }
  }
}
int distancia_cm()
{
  float duracion;
  float large;
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duracion = pulseIn(eco, HIGH);
  large = duracion / 58.2;
  delay(100);
  return large;
}
void lighSignal(int d)
{
  int indice;
  int dot;
  int dist;
  dist = d - distAlto_ino;
  dot = distDeteccion_ino / 8;
  indice = dist / dot;
  Serial.print(indice);
  if (indice > 7)
  {
    // no se muestra nada
    lc.clearDisplay(0);
  }
  else
  {

    matrixDraw(estados[indice]);
  }
}
//void requestEvent(){
//}
void setup()
{
  Serial.begin(9600);
  lc.shutdown(0, false);
  lc.clearDisplay(0);
  pinMode(trig, OUTPUT);
  pinMode(eco, INPUT);
  // Este Esclavo es el número 2
  //Wire.begin(2);
  // Cuando el Maestro le hace una petición,realiza el requestEvent
  //Wire.onRequest(requestEvent);
}
void loop()
{
  //variable detector para saber cuando dormir y cuando trabajar
  Serial.print("detector:");
  Serial.println(detector);
  distancia = distancia_cm();
  Serial.print("distancia:");
  Serial.println(distancia);
  //regula intensidad de luz de la matrix
  intensidad = lumen(); // intensidad tomara un valor del 3 al  14
  lc.setIntensity(0, intensidad);
  // obtenemos la distancia y la guardamos en variable kyori
  kyori = distancia - distAlto_ino;
  if (distancia <= distDeteccion_ino && distancia >= distAlto_ino + 20)
  {
    detector = 1;

    Serial.println("DETECTADO");
  }

  if (detector == 1)
  {
    if (kyori <= 2)
    {

      Serial.println("PARKING");
      matrixDraw(8);
      delay(10000);
      lc.clearDisplay(0);
      detector = 0;
    }
    else
    {
      Serial.print("MIDIENDO  d=");
      Serial.println(kyori);
      lighSignal(distancia);
      delay(500);
      lc.clearDisplay(0);
    }
  }
  else
  {
    Serial.print("Modo sleep");
  }
}
