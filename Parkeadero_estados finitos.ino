#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <EEPROM.h>

// estado de la LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
// para la pantalla corrediza 
#define COLS 16       // Columnas del LCD
#define ROWS 2        // Filas del LCD
#define VELOCIDAD 300 // Velocidad a la que se mueve el texto
// variables de estado
#define S_Home 0
#define S_Ingreso_S 1
#define S_Intermedio_S 2
#define S_Parking_S 3
#define S_num_muestras_S 4
#define brillo_pantalla 5

int stage = S_Home; // estado inicial

// definicion de los botones
#define btn_menu 0
#define btn_salir 1
#define btn_up 2      // A
#define btn_down 3    // B

byte boton[] = // DEFINIR LOS PINES
    {
        A0,
        A1,
        A2,
        A3};

byte boton_state[4];

// variables de operacion

int aumentos[] = {0, 10, 10, 10, 1};
int led_verde = 12;
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
int brillo;
int dir_ingreso = 0;
int dir_intermedio = 100;
int dir_parking = 200;
int dir_num_muestras = 300;
int dir_brillo = 400; 
float sum;
float prom;

String textoMenu = "Midiendo";
String textoIngreso="Indicar distancia de ingreso";
String textoIntermedio = "Indicar distancia intermedia de precaucion";
String textoParking = "Indicar distancia de parada del Auto (minimo de 2 cm)";
String textoMuestras ="Indicar el numero de muestras a tomar del sensor";
String textoBrillo = "Brillo de pantalla";

void prime_Menu(String texto)
{
int text_long = texto.length();
if(text_long > 17){
  for (int i = 1; i <= text_long; i++)
  {
    String impresionenPantalla = texto.substring(i - 1);

    // Limpiamos pantalla
    lcd.clear();

    //Situamos el cursor
    lcd.setCursor(0, 1);

    // Escribimos el texto
    lcd.print(impresionenPantalla);

    // Esperamos
    delay(VELOCIDAD);
  }
}else {
  lcd.setCursor(0, 0);
  lcd.print(texto);
}

}
void prime_variable(int var)
{
  lcd.setCursor(0, 1);
  lcd.print(var);
}
int encoder(int cambio, int incremento)
{
  static unsigned long ultimaInterrupcion = 0; // variable static con ultimo valor de
                                               // tiempo de interrupcion
  unsigned long tiempoInterrupcion = millis(); // variable almacena valor de func. millis

  if (tiempoInterrupcion - ultimaInterrupcion > 5)
  {                             // rutina antirebote desestima
                                // pulsos menores a 5 mseg.
    if (digitalRead(boton[btn_up]) == HIGH) // si B es HIGH, sentido horario
    {
      cambio= cambio + incremento; // incrementa POSICION en 1
    }
    else
    {           // si B es LOW, senti anti horario
      cambio = cambio - incremento; // decrementa POSICION en 1
    }
    //cambio = min(100, max(0, calibrador));   // establece limite inferior de 0 ysuperior de 100 para POSICION
    ultimaInterrupcion = tiempoInterrupcion; // guarda valor actualizado del tiempo
    return cambio;
  } // de la interrupcion en variable static
}
byte subida(int btn)
{
  uint8_t valor_nuevo = digitalget(boton[btn]);
  uint8_t result = boton_state[btn] != valor_nuevo && valor_nuevo == 1;
  boton_state[btn] = valor_nuevo;

  return result;
}
void initLedS()
{
  const char *deve = "Sr. Rios ";
  const char *vershon = "parking v1.1";

  lcd.init();
  lcd.backlight();
  lcd.setBacklight(HIGH);
  lcd.begin(COLS, ROWS);
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
  switch (state)
  {
  case 2:
    digitalWrite(idLed, HIGH);
    delay(400);
    digitalWrite(idLed, LOW);
    break;
  case 1 :
    digitalWrite(idLed, HIGH);

  break;
  case 0:
    digitalWrite(idLed, LOW);

    break;
  }
  
}
void setup()
{
  // Configurar como PULL-UP para ahorrar resistencias
  pinMode(boton[btn_menu], INPUT_PULLUP);
 // pinMode(boton[btn_salir], INPUTUP);
 pinMode(boton[btn_up], INPUT);
 pinMode(boton[btn_down], INPUT);

  // Se asume que el estado inicial es HIGH
  boton_state[0] = HIGH;
 // boton_state[1] = HIGH;
 // boton_state[2] = HIGH;
 // boton_state[3] = HIGH;
  // configuracion inicial para la pantalla con saludo
  initLedS();
  // Configurar las entradas del encoder

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
  EEPROM.get(dir_ingreso, ingreso);
  EEPROM.get(dir_intermedio, intermedio);
  EEPROM.get(dir_parking, parking);
  EEPROM.get(dir_num_muestras, num_muestras);
  EEPROM.get(dir_brillo,brillo);

  switch (stage)

  {
  case S_Home:

    if (subida(btn_menu))
    { // Transición BTN_MENU
      stage = S_Ingreso_S;
      lcd.clear();
      prime_Menu(textoIngreso);
      break;
    }
    else
    {
      sum = 0;
      for (int i = 0; i < num_muestras; i++)
      {
        sum += distancia();
        delay(50);
      }
      prom = sum / num_muestras;
      D = prom;
      lcd.clear();
      prime_Menu(textoMenu);
      prime_variable(D);
      lcd.setCursor(5, 1);
      lcd.print("cm");
      delay(400);
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
        unsigned long tiempo_parkeado = millis();
        ledStatus(1, led_rojo);
        ledStatus(0, led_amarillo);
        ledStatus(0, led_verde);
        if (tiempo_parkeado >= 25000)
        {
        ledStatus(0,led_rojo);
         lcd.setBacklight(LOW);
        break;
        }
        
      }
   
    }

    break;
/**** Fin del estado S_HOME***/
  case S_Ingreso_S:
    prime_variable(ingreso);
    if (subida(btn_menu))
    { // Transición btn_menu
      stage = S_Intermedio_S;
      lcd.clear();
      prime_Menu(textoIntermedio);
      prime_variable(intermedio);
      break;
    }else {
      encoder(ingreso, 10);
      EEPROM.put(dir_ingreso, ingreso);
      lcd.clear();
      prime_Menu(textoIngreso);
      prime_variable(ingreso);
      
    }
  break;
    /*** FIN ESTADO Ingreso ***/

  case S_Intermedio_S:
    prime_variable(intermedio);
    if (subida(btn_menu))
    { // Transición btn_menu
      stage = S_Parking_S;
      lcd.clear();
      prime_Menu(textoParking);
     break;
    }else{
      encoder(intermedio, 10);
      EEPROM.put(dir_intermedio, intermedio);
      lcd.clear();
      prime_Menu(textoIntermedio);
      prime_variable(intermedio);
      
    }
  break;
    
     /*** FIN ESTADO Intermedio ***/

  case S_Parking_S:
    prime_variable(parking);
    if (subida(btn_menu))
    { // Transición btn_menu
      stage = S_num_muestras_S;
      lcd.clear();
      prime_Menu(textoMuestras);
      prime_variable(num_muestras);

      break;
    }else
    {
      encoder(parking, 10);
      EEPROM.put(dir_parking, parking);
      lcd.clear();
      prime_Menu(textoParking);
      prime_variable(parking);
      
    }
  break;
    /*** FIN ESTADO parking ***/

  case S_num_muestras_S:
    prime_variable(num_muestras);
    if (subida(btn_menu))
    { // Transición btn_menu
      stage = brillo_pantalla;
      prime_Menu(textoBrillo);
      prime_variable(brillo);
      break;
    }
    else
    {
      encoder(num_muestras, 1);
      EEPROM.put(dir_num_muestras, num_muestras);
      lcd.clear();
      prime_Menu(textoMuestras);
      prime_variable(num_muestras);
     
    }
    break;

    /*** FIN ESTADO S_SET_R ***/

    case brillo_pantalla:
    prime_variable(brillo);
    if (subida(btn_menu))
    {
      stage = S_Home;
      lcd.clear();
      prime_Menu(textoMenu);
      break;
    }else
    {
      encoder(brillo, 1);
      EEPROM.put(dir_brillo, brillo);
      lcd.clear();
      prime_Menu(textoBrillo);
      prime_variable(brillo);
    }
    break;
  }
}