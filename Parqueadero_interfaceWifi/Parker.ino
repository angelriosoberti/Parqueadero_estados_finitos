
#include <ESP8266WiFi.h>
const char *ssid = "ESP_Park";
const char *password = "test";

IPAddress ip(192, 168, 1, 200);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

#include <Wire.h>
#include <LedControl.h>

String ssid = "Family_rios";
String password = "yeraybenjamin";
WiFiServer server(80);

//Los pines deben estar sipre conectados a puertos PWM
// DEBEN SER CONECTADOS EN PINES CON PWM EN EL NODEMCU ES 2,5,6 Y 7
int DIN = D6;//
int CLK = D5;//
int CS = D8;//
int intensidad // La intensidad es del  
LedControl lc = LedControl(D6,D5,D8,1);
void apCon (){
  // esta funcion hace que el esp genere su propia red wifi y no tenga que estar necesitando una aparte.
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(ip, gateway, subnet);
  Serial.print("Iniciado AP:\t");
  Serial.println(ssid);
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP())
}
int lumen(){
  // tomo lectura de un puerto analogico
  int lecturaLuz = analogRead(A4);
  int luzLevel; // este numero debe ser entre 1 y 14
  // determinar resistenacia y saber q caida de tension habra!************************************************
  return luzLevel;
}
void matrixDraw(int columna)
{
  if (columna >= 8)
  {
    Serial.print("ALERTA PROBLEMA DE CARGADO DE")
  }
  else
  {
    for (int fila = 0; fila < 8; fila++)
    {
      for (int row = 0; i < columna; i++)
      {
        lc.setLed(0, fila, row, true);
        delay(10);
      }
    }
  }

  
}
void wifiConection(){
  Wifi.begin(ssid, password);
  while (Wifi.status() != WL_CONNECTED)
  {
   delay(500);
   Serial.print(".");
  }
  
}
void setup()
{
  wifiConection();
  Serial.begin(115200);
  lc.shutdown(0, false);
  lc.clearDisplay(0);
}
void loop()
{
  intensidad = lumen();
  lc.setIntensity(0, intensidad);


}