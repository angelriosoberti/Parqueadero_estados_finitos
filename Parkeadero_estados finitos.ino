#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <EEPROM.h>

// estado de la LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
// variables de estado
#define S_Home 0
#define S_Ingreso_S 1
#define S_Intermedio_S 2
#define S_Parking_S 3
#define S_num_muestras_S 4

int stage = S_Home; // estado inicial



// definicion de los botones
#define btn_menu  0
#define btn_salir  1
#define btn_up  2
#define btn_down  3

byte boton[] = // DEFINIR LOS PINES
      {
        A0,
        A1,
        A2,
        A3
         };

byte boton_state[4];

// variables de operacion

int aumentos[]= {0,10,10,10,1};
int led_verde = 4;
int led_amarillo = 3;
int led_rojo = 2;
int trig = 9;
int eco = 8;
int calibrador = 58.2;
float filter;

int ingreso;
int intermedio;
int parking;
int num_muestras;
int dir_ingreso = 0;
int dir_intermedio= 50;
int dir_parking = 100;
int dir_num_muestras= 150 ;

void prime_Menu(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[MENU medicion]");
}
void prime_Ingreso()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[MENU/Ingreso]");
}
void prime_Intermedio()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[MENU/Inter]");
}
void prime_Parking()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[MENU/Parking]");
}
void prime_numMuestra()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[MENU/muestras]");
}
void prime_variable(int var)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(var);
}


byte subida(int btn)
{
  uint8_t valor_nuevo = digitalRead(boton[btn]);
  uint8_t result = boton_state[btn] != valor_nuevo && valor_nuevo == 1;
  boton_state[btn] = valor_nuevo;
  return result;
}
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
       delay(400);
    digitalWrite(idLed, LOW);
   
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
  // Configurar como PULL-UP para ahorrar resistencias
  pinMode(boton[btn_menu], INPUT_PULLUP);
  pinMode(boton[btn_salir], INPUT_PULLUP);
  pinMode(boton[btn_up], INPUT_PULLUP);
  pinMode(boton[btn_down], INPUT_PULLUP);

  // Se asume que el estado inicial es HIGH
  boton_state[0] = HIGH;
  boton_state[1] = HIGH;
  boton_state[2] = HIGH;
  boton_state[3] = HIGH;
  // configuracion inicial para la pantalla con saludo
  initLedS();
  // configuracion del sensor
  pinMode(trig, OUTPUT);
  pinMode(eco, INPUT);
  // configuracion de las luces
  pinMode(led_verde, OUTPUT);
  pinMode(led_amarillo, OUTPUT);
  pinMode(led_rojo, OUTPUT);
 
  Serial.begin(9600);
}

void loop()
{
  ingreso = EEPROM.get(dir_ingreso,ingreso) ;
  intermedio = EEPROM.get(dir_intermedio, intermedio);
  parking = EEPROM.get(dir_parking, parking);
  num_muestras = EEPROM.get(dir_num_muestras, num_muestras);
  switch (stage)
  {
  case S_Home:
    filter = filtradoPromedio(num_muestras, tempoSen);
    int D;
    if (subida(btn_menu))
    { // Transición BTN_MENU
      stage = S_Ingreso_S;
      prime_Ingreso();
      break;
    } if (filter >= 150 && filter <= 25000)
    {
      
      D = medicionDistancia(filter);
      if (D <= parking)
      {
        ledStatus(1, led_rojo);
        ledStatus(2, led_verde);
        ledStatus(2, led_amarillo);
        prime_Menu();
        prime_variable(D);
      }
      else if (D >= ingreso && D < intermedio)
      {
        ledStatus(0, led_rojo);
        ledStatus(2, led_verde);
        ledStatus(1, led_amarillo);
        prime_Menu();
        prime_variable(D);
      }
      else if (D >= ingreso && D < intermedio)
      {
        ledStatus(0, led_rojo);
        ledStatus(2, led_verde);
        ledStatus(0, led_amarillo);
        prime_Menu();
        prime_variable(D);
      }
    }
    break;
  


  case S_Ingreso_S:
   
    if (subida(btn_menu))
    { // Transición btn_menu
      stage = S_Ingreso_S;
      prime_Intermedio();
      break;
    }
    if (subida(btn_salir))
    { // Transición BTN_EXIT
      stage = S_Home;
      prime_Menu();
      break;
    }
    if (subida(btn_up))
    { // Transición BTN_UP
      if (ingreso < 400)
      {
        ingreso = ingreso + aumentos[S_Ingreso_S];
      }
      else
      {
        ingreso = 0;
      }
     
      prime_Ingreso();
      prime_variable(ingreso);
      break;
    }
    if (subida(btn_down))
    { // Transición BTN_DWN
      if (ingreso > 0)
      {
        ingreso = ingreso - aumentos[S_Ingreso_S];
      }
      else
      {
        ingreso = 400;
      }
      prime_Ingreso();
      prime_variable(ingreso);
      EEPROM.update(dir_ingreso,ingreso);
      break;
    }
    break; /*** FIN ESTADO S_SET_R ***/
    



  case S_Intermedio_S:
    if (subida(btn_menu))
    { // Transición btn_menu
      stage = S_Parking_S;
      prime_Parking();
      break;
    }
    if (subida(btn_salir))
    { // Transición BTN_EXIT
      stage = S_Home;
      prime_Menu();
      break;
    }
    if (subida(btn_up))
    { // Transición BTN_UP
      if (intermedio < 400)
      {
        intermedio = intermedio + aumentos[S_Intermedio_S];
      }
      else
      {
        intermedio = 0;
      }

      prime_Intermedio();
      prime_variable(intermedio);
      break;
    }
    if (subida(btn_down))
    { // Transición BTN_DWN
      if (intermedio > 0)
      {
        intermedio = intermedio - aumentos[S_Intermedio_S];
      }
      else
      {
        intermedio = 400;
      }
      prime_Ingreso();
      prime_variable(intermedio);
      EEPROM.update(dir_intermedio, intermedio);
      break;
    }
    break; /*** FIN ESTADO S_SET_R ***/

  case S_Parking_S:
    if (subida(btn_menu))
    { // Transición btn_menu
      stage = S_num_muestras_S;
      prime_numMuestra();
      break;
    }
    if (subida(btn_salir))
    { // Transición BTN_EXIT
      stage = S_Home;
      prime_Menu();
      break;
    }
    if (subida(btn_up))
    { // Transición BTN_UP
      if (parking < 400)
      {
        parking = parking + aumentos[S_Parking_S];
      }
      else
      {
        parking = 0;
      }

      prime_Parking();
      prime_variable(parking);
      break;
    }
    if (subida(btn_down))
    { // Transición BTN_DWN
      if (parking > 0)
      {
        parking = parking - aumentos[S_Parking_S];
      }
      else
      {
        parking = 400;
      }
      prime_Ingreso();
      prime_variable(parking);
      EEPROM.update(dir_parking, parking);
      break;
    }
    break; /*** FIN ESTADO S_SET_R ***/

  case S_num_muestras_S:
    if (subida(btn_menu))
    { // Transición btn_menu
      stage = S_Ingreso_S;
      prime_Ingreso();
      break;
    }
    if (subida(btn_salir))
    { // Transición BTN_EXIT
      stage = S_Home;
      prime_Menu();
      break;
    }
    if (subida(btn_up))
    { // Transición BTN_UP
      if (num_muestras < 15)
      {
        num_muestras = num_muestras + aumentos[S_num_muestras_S];
      }
      else
      {
        num_muestras = 0;
      }

      prime_numMuestra();
      prime_variable(num_muestras);
      break;
    }
    if (subida(btn_down))
    { // Transición BTN_DWN
      if (num_muestras > 0)
      {
        num_muestras = num_muestras - aumentos[S_num_muestras_S];
      }
      else
      {
        num_muestras = 15;
      }
      prime_Ingreso();
      prime_variable(num_muestras);
      EEPROM.update(dir_num_muestras, num_muestras);
      break;
    }
    break; /*** FIN ESTADO S_SET_R ***/

  }
}