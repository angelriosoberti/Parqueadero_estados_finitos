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
#define btn_menu 0
#define btn_salir 1
#define btn_up 2
#define btn_down 3

byte boton[] = // DEFINIR LOS PINES
    {
        A0,
        A1,
        A2,
        A3};

byte boton_state[4];

// variables de operacion

int aumentos[] = {0, 10, 10, 10, 1};
int led_verde = 4;
int led_amarillo = 3;
int led_rojo = 2;
int trig = 9;
int eco = 8;
int calibrador = 58.2;
float filter;
int D;
int ingreso;
int intermedio;
int parking;
int num_muestras;
int dir_ingreso = 0;
int dir_intermedio = 100;
int dir_parking = 200;
int dir_num_muestras = 300;
float sum;
float prom;

void prime_Menu()
{

  lcd.setCursor(0, 0);
  lcd.print("[MENU medicion]");
}
void prime_Ingreso()
{

  lcd.setCursor(0, 0);
  lcd.print("[MENU/Ingreso]");
}
void prime_Intermedio()
{

  lcd.setCursor(0, 0);
  lcd.print("[MENU/Inter]");
}
void prime_Parking()
{

  lcd.setCursor(0, 0);
  lcd.print("[MENU/Parking]");
}
void prime_numMuestra()
{

  lcd.setCursor(0, 0);
  lcd.print("[MENU/muestras]");
}
void prime_variable(int var)
{
  lcd.setCursor(0, 1);
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
  delay(1500);
  lcd.clear();
  lcd.print("Asistente de  ");
  lcd.setCursor(0, 1);
  lcd.print(vershon);
  delay(1500);
  lcd.clear();
}
float distancia() // obtiene el tiempo entre ecos del sensor.
{
  float duracion;
  float large;
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duracion = pulseIn(eco, HIGH);
  large = duracion / 58.2;

  return large;
}

void ledStatus(int state, int idLed)
{
  // estado : apagar(0),prender(1), intermitente(2)
  // identificador de led : cual led de que pin
  //*******************************************************sin testeo aun *************************************
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
// descomentar solo si el microcontrolador es usado por primera vez 
  // EEPROM.put(dir_ingreso,1) ;
  //  EEPROM.put(dir_intermedio, 1);
  // EEPROM.put(dir_parking, 1);
  //  EEPROM.put(dir_num_muestras, 1);
}

void loop()
{

  switch (stage)

  {
  case S_Home:

    if (subida(btn_menu))
    { // Transición BTN_MENU
      stage = S_Ingreso_S;
      lcd.clear();
      prime_Ingreso();
      break;
    }
    else
    {
      ingreso = EEPROM.get(dir_ingreso, ingreso);
      intermedio = EEPROM.get(dir_intermedio, intermedio);
      parking = EEPROM.get(dir_parking, parking);
      num_muestras = EEPROM.get(dir_num_muestras, num_muestras);
      sum = 0;

      for (int i = 0; i < num_muestras; i++)
      {
        sum += distancia();
        delay(50);
      }
      prom = sum / num_muestras;

      D = prom;
      Serial.print("Distancia: ");
      Serial.println(D);
      lcd.clear();
      prime_Menu();
      prime_variable(D);
      lcd.setCursor(4, 1);
      lcd.print("cm");
      delay(500);
      if (D > ingreso)
      {
        ledStatus(0,led_rojo);
        ledStatus(0,led_amarillo);
        ledStatus(2,led_verde);
      }
      if (D <= ingreso && D > intermedio)
      {
        ledStatus(0, led_rojo);
        ledStatus(0, led_amarillo);
        ledStatus(1, led_verde);
      }
      if (D <= intermedio && D > parking)
      {
        ledStatus(0, led_rojo);
        ledStatus(2, led_amarillo);
        ledStatus(1, led_verde);
      }
      if (D <= parking)
      {
        ledStatus(1, led_rojo);
        ledStatus(1, led_amarillo);
        ledStatus(1, led_verde);
      }
   
    }

    break;

  case S_Ingreso_S:
    prime_variable(ingreso);
    if (subida(btn_menu))
    { // Transición btn_menu
      stage = S_Intermedio_S;
      lcd.clear();
      prime_Intermedio();
      prime_variable(intermedio);
      break;
    }
    if (subida(btn_salir))
    { // Transición BTN_EXIT
      stage = S_Home;
      lcd.clear();
      prime_Menu();
      break;
    }
    if (subida(btn_up))
    { // Transición BTN_UP
      if (ingreso < 500)
      {
        ingreso = ingreso + 10;
      }
      else
      {
        ingreso = 0;
      }
      EEPROM.update(dir_ingreso, ingreso);
      lcd.clear();
      prime_Ingreso();
      prime_variable(ingreso);
      break;
    }
    if (subida(btn_down))
    { // Transición BTN_DWN
      if (ingreso > 0)
      {
        ingreso = ingreso - 10;
      }
      else
      {
        ingreso = 500;
      }
      EEPROM.update(dir_ingreso, ingreso);
      lcd.clear();
      prime_Ingreso();
      prime_variable(ingreso);

      break;
    }
    break; /*** FIN ESTADO Ingreso ***/

  case S_Intermedio_S:
    prime_variable(intermedio);
    if (subida(btn_menu))
    { // Transición btn_menu
      stage = S_Parking_S;
      lcd.clear();
      prime_Parking();
      prime_variable(parking);
      break;
    }
    if (subida(btn_salir))
    { // Transición BTN_EXIT
      stage = S_Home;
      lcd.clear();
      prime_Menu();
      break;
    }
    if (subida(btn_up))
    { // Transición BTN_UP
      if (intermedio < 400)
      {
        intermedio = intermedio + 10;
      }
      else
      {
        intermedio = 0;
      }
      EEPROM.update(dir_intermedio, intermedio);
      lcd.clear();
      prime_Intermedio();
      prime_variable(intermedio);
      break;
    }
    if (subida(btn_down))
    { // Transición BTN_DWN
      if (intermedio > 0)
      {
        intermedio = intermedio - 10;
      }
      else
      {
        intermedio = 400;
      }
      EEPROM.update(dir_intermedio, intermedio);
      lcd.clear();
      prime_Intermedio();
      prime_variable(intermedio);

      break;
    }
    break; /*** FIN ESTADO Intermedio ***/

  case S_Parking_S:
    prime_variable(parking);
    if (subida(btn_menu))
    { // Transición btn_menu
      stage = S_num_muestras_S;
      lcd.clear();
      prime_numMuestra();
      prime_variable(num_muestras);

      break;
    }
    if (subida(btn_salir))
    { // Transición BTN_EXIT
      stage = S_Home;
      lcd.clear();
      prime_Menu();
      break;
    }
    if (subida(btn_up))
    { // Transición BTN_UP
      if (parking < 400)
      {
        parking = parking + 10;
      }
      else
      {
        parking = 0;
      }
      EEPROM.update(dir_parking, parking);
      lcd.clear();
      prime_Parking();
      prime_variable(parking);
      break;
    }
    if (subida(btn_down))
    { // Transición BTN_DWN
      if (parking > 0)
      {
        parking = parking - 10;
      }
      else
      {
        parking = 400;
      }
      EEPROM.update(dir_parking, parking);
      lcd.clear();
      prime_Parking();
      prime_variable(parking);

      break;
    }
    break; /*** FIN ESTADO parking ***/

  case S_num_muestras_S:
    prime_variable(num_muestras);
    if (subida(btn_menu))
    { // Transición btn_menu
      stage = S_Ingreso_S;
      prime_Ingreso();
      prime_variable(ingreso);
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
        num_muestras++;
      }
      else
      {
        num_muestras = 0;
      }
      EEPROM.update(dir_num_muestras, num_muestras);
      lcd.clear();
      prime_numMuestra();
      prime_variable(num_muestras);
      break;
    }
    if (subida(btn_down))
    { // Transición BTN_DWN
      if (num_muestras > 0)
      {
        num_muestras--;
      }
      else
      {
        num_muestras = 15;
      }
      EEPROM.update(dir_num_muestras, num_muestras);
      lcd.clear();
      prime_numMuestra();
      prime_variable(num_muestras);

      break;
    }
    break; /*** FIN ESTADO S_SET_R ***/
  }
}