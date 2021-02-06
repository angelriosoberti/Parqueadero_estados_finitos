int distDeteccion_esp;
int distAlto_esp;
#include <ESP8266WiFi.h>
WiFiServer server(80);
const char *ssid = "ESP_Park";
const char *password = "test";
#include <Wire.h>
void apCon()
{
  // esta funcion hace que el esp genere su propia red wifi y no tenga que estar necesitando una aparte.
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(ip, gateway, subnet);
  Serial.print("Iniciado AP:\t");
  Serial.println(ssid);
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP())
}
// SOLO USADO EN EL CASO DE CONECTARSE A UNA RED WIFI
// void wifiConection(){
//   Wifi.begin(ssid, password);
//   while (Wifi.status() != WL_CONNECTED)
//   {
//    delay(500);
//    Serial.print(".");
//   }

// }


IPAddress ip(192, 168, 1, 200);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
void setup()
{
  Wire.begin();       // Conexión al Bus I2C
  Serial.begin(9600); // Velocidad de conexión
}

void loop()
{
  Wire.requestFrom(2, 10); // Le pide 10 bytes al Esclavo 2

  while (Wire.available()) // slave may send less than requested
  {
    char c = Wire.read(); // Recibe byte a byte
    Serial.print(c);      // Presenta los caracteres en el Serial Monitor
  }
  
  Serial.println(); // Cámbia de línea en el Serial Monitor.
  delay(500);

  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }
  Serial.println("Nuevo Cliente...");
  while (!client.available())
  {
    delay(1);
  }
  String peticion = client.readStringUntil('\r');
  Serial.println(peticion);
  client.flush();

  if (peticion.indexOf('LED=ON') != -1)
  {
    estadoLed2 = 1;
    digitalWrite(led2, estadoLed2);
  }
  else if (peticion.indexOf('LED=OFF') != -1)
  {
    estadoLed2 = 0;
    digitalWrite(led2, estadoLed2);
  }

  if (peticion.indexOf('foco=prende') != -1)
  {
    estadoLed1 = 1;
    digitalWrite(led1, estadoLed1);
  }
  else if (peticion.indexOf('foco=apaga') != -1)
  {
    estadoLed1 = 0;
    digitalWrite(led1, estadoLed1);
  }

  client.println("HTTP/1.1 200 OK");
  client.println("");
  client.println("");
  client.println("");
  client.println("");

  client.println("<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
  client.println("<title>Control de LED</title></head>");
  client.println("<body style='font-family: Century gothic; width: 800;'><center>");
  client.println("<div style='box-shadow: 0px 0px 20px 8px rgba(0,0,0,0.22); padding: 20px; width: 300px; display: inline-block; margin: 30px;'> ");
  client.println("<h1>LED AZUL</h1>");

  if (estadoLed1 == 1)
    client.println("<h2>El led esta ENCENDIDO</h2>");
  else
    client.println("<h2>El led esta APAGADO</h2>");

  client.println("<button style='background-color:red;  color:white; border-radius: 10px; border-color: rgb(255, 0, 0);' ");
  client.println("type='button' onClick=location.href='/LED=OFF'><h2>Apagar</h2>");
  client.println("</button> <button style='background-color:blue; color:white; border-radius: 10px; border-color: rgb(25, 255, 4);' ");
  client.println("type='button' onClick=location.href='/LED=ON'><h2>Encender</h2>");
  client.println("</button></div></center>");

  client.println("<center>");
  client.println("<div style='box-shadow: 0px 0px 20px 8px rgba(0,0,0,0.22); padding: 20px; width: 300px; display: inline-block; margin: 30px;'> ");
  client.println("<h1>LED BLANCO</h1>");

  if (estadoLed2 == 1)
    client.println("<h2>El led esta ENCENDIDO</h2>");
  else
    client.println("<h2>El led esta APAGADO</h2>");

  client.println("<button style='background-color:red;  color:white; border-radius: 10px; border-color: rgb(255, 0, 0);' ");
  client.println("type='button' onClick=location.href='/foco=apaga'><h2>Apagar</h2>");
  client.println("</button> <button style='background-color:blue; color:white; border-radius: 10px; border-color: rgb(25, 255, 4);' ");
  client.println("type='button' onClick=location.href='/foco=prende'><h2>Encender</h2>");
  client.println("</button></div></center></body></html>");

  delay(10);
  Serial.println("Peticion finalizada");
  Serial.println("");
}