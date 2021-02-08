

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


//Variables para el guardado en EPPROM
int distDeteccion_ino;
int distAlto_ino;
int adressDistDeteccion_ino=0;
int adressDistAlto_ino=100 ;

struct dataTransfer {
  int txKyori;
};
struct dataRecived{
  int distDeteccion_esp;
  int distAlto_esp;
  bool peticion ;
};
dataTransfer envio;
dataRecived recibe;

bool newTxData = false;
bool newRxData = false;
const byte arduiono_nano = 8; // arduiono nano
const byte ESP8266 = 9;     //ESP8266
unsigned long prevUpdateTime = 0;
unsigned long updateInterval = 500;



int kyori;
int detector = 0;
int distancia ;
bool sync = false ;

LedControl lc = LedControl(DIN, CLK, CS, 1);
//funciones para la transmision
void madarData()
{

  if (newTxData == true)
  {
    //inicia la transmision a otro dispositivo
    Wire.beginTransmission(ESP8266);
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
    EEPROM.put(adressDistAlto_ino,recibe.distAlto_esp);
    EEPROM.put(adressDistDeteccion_ino,recibe.distDeteccion_esp);
    sync = recibe.peticion;
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
    envio.txKyori= kyori;
      newTxData = true;
    }
  }
}

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
void operacion(){
  distancia = distancia_cm();
  /* Serial.print("distancia:");
  Serial.println(distancia);*/
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
    if (kyori <= distAlto_ino)
    {

      Serial.println("PARKING");
      matrixDraw(8);
      delay(10000);
      lc.clearDisplay(0);
      detector = 0;
      sync = false;
    }
    else
    {
      Serial.print("MIDIENDO  d=");
      Serial.println(kyori);
      lighSignal(distancia);
      delay(350);
      lc.clearDisplay(0);
    }
  }
  else
  {
    Serial.print("Modo sleep");
  }
}

void setup()
{
  Serial.begin(9600);
  lc.shutdown(0, false);
  lc.clearDisplay(0);
  pinMode(trig, OUTPUT);
  pinMode(eco, INPUT);

  Wire.begin(arduiono_nano);
  // Cuando el Maestro le hace una petición,realiza el requestEvent
  Wire.onRequest(receiveEvent);
}
void loop()
{
if (newRxData == true)
{
  newRxData = false;
}
operacion();
updateDataToSend();
if(sync = true){
  madarData();
}
}
