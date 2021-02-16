
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
ESP8266WebServer server(80);
IPAddress ip(192, 168, 1, 200);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
String header;

const char *ssid = "ESP_Park";
const char *password = "test";

//Variables para el guardado en EPPROM
int distDeteccion_esp;
int distAlto_esp;
int kyori;
bool medir = false;

struct dataRecived
{
  int txKyori;
};
struct dataTransfer
{
  
  int distDeteccion_esp;
  int distAlto_esp;
  bool peticion;
};

dataTransfer envio;
dataRecived recibe;

bool newTxData = false;
bool newRxData = false;
const byte arduiono_nano = 8; // arduiono nano
const byte ESP8266 = 9;       //ESP8266
unsigned long prevUpdateTime = 0;
unsigned long updateInterval = 500;

void madarData()
{

  if (newTxData == true)
  {
    //inicia la transmision a otro dispositivo
    Wire.beginTransmission(arduiono_nano);
    Wire.write((byte *)&envio, sizeof(envio));
    Wire.endTransmission();

    /*
    Serial.print("Sent ");
    Serial.print(txData.textA);
    Serial.print(' ');
    Serial.print(txData.valA);
    Serial.print(' ');
    Serial.println(txData.valB);*/

    newTxData = false;
  }
}

void receiveEvent(int numBytesReceived)
{

  if (newRxData == false)
  {
    // copy the data to rxData
    Wire.readBytes((byte *)&recibe, numBytesReceived);
    kyori = recibe.txKyori;
    newRxData = true;
  }
}

// en esta funcion asigno los valores a
void updateDataToSend()
{

  if (millis() - prevUpdateTime >= updateInterval)
  {
    prevUpdateTime = millis();
    if (newTxData == false)
    { // ensure previous message has been sent
     distAlto_esp =  envio.distAlto_esp;
     distDeteccion_esp = envio.distDeteccion_esp;
     medir = envio.peticion;
      medirnewTxData = true;
    }
  }
}

void apCon()
{
  // esta funcion hace que el esp genere su propia red wifi y no tenga que estar necesitando una aparte.
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(ip, gateway, subnet);
  Serial.print("Iniciado AP:\t");
  Serial.println(ssid);
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP())
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void setup()
{
  Wire.begin(1);       // Conexión al Bus I2C 
  Serial.begin(9600); // Velocidad de conexión
  apCon();
  Wire.onRequest(receiveEvent);
}

void loop()
{

if (newRxData == true)
{
  newRxData = false;
}
updateDataToSend();
madarData();
server.handleClient();
}
void handle_OnConnect()
{
  server.send(200, "text/html", SendHTML(kyori,distDeteccion_esp,distAlto_esp));
}
void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
}
String SendHTML(int medida, int deteccion, int alto )
{
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>ESP8266 Weather Report</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "p {font-size:24px;color:#444444;margin-bottom:10px;border-style:ridge;\n";
  ptr += "border-width:1px;border-color:black;}\n";
  ptr += "#actual{font-size:40px;color:#070707;   margin-bottom:10px;margin-top:1px;}\n";
  ptr += "#deteccion{font-size:40px;color:#30ff07;margin-bottom:10px;margin-top:1px;}\n";
  ptr += "#alto{font-size:40px;color:#fa0303; margin-bottom:10px;margin-top:1px;\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>ESP8266 NodeMCU Distance Display</h1>\n";

  ptr += "<p>Medida Actual:</p> <p id=\"actual\">";
  ptr += (int)medida;
  ptr += "cm</p><br>";
  ptr += "<p>Distancia de deteccion:</p><p id=\"deteccion\"> ";
  ptr += (int)deteccion;
  ptr += "cm</p><br>";
  ptr += "<p>Distancia de alto:</p> <p id=\"alto\"> ";
  ptr += (int)alto;
  ptr += "cm</p><br>";

  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}






///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void appWeb(){


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
}*/