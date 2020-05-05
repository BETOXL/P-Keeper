/* Control velocidad y sentido giro de motor PaP con pololu A4988 
 *  2020 - P-keeper
 */
//https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads/
#include <Wire.h>			// libreria de comunicacion por I2C
#include <LCD.h>			// libreria para funciones de LCD
#include <LiquidCrystal_I2C.h>		// libreria para LCD por I2C

int steps = 9;       // pin step 9
int direccion = 3;   // pin direccion 3
int potenciometro;   // lectura del potenciometro
int exalacionVel;
int potenciometro2; // lectura del potenciometro2
const byte interruptPin = 2; //PIN 2 BOTON START
volatile byte running = LOW;
int porRetVeloc;
int porRetInaExa;
int analogico0;
int analogico1;
LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7); // DIR, E, RW, RS, D4, D5, D6, D7

void setup() {
  // inicializamos pin como salidas.
  pinMode(steps, OUTPUT); 
  pinMode(direccion, OUTPUT); 
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), iniciarDetener, CHANGE);
  // Indicar a la libreria que tenemos conectada una pantalla de 16x2
  lcd.setBacklightPin(3,POSITIVE);	// puerto P3 de PCF8574 como positivo
  lcd.setBacklight(HIGH);		// habilita iluminacion posterior de LCD
  lcd.begin(16, 2);			// 16 columnas por 2 lineas para LCD 1602A
  lcd.clear();			// limpia pantalla
}

void loop() { 
    if (running == HIGH) {
      analogico0 = analogRead(A0);     // leemos el potenciometro en el puerto A0
      porRetVeloc = map(analogico0, 0, 1024, 100, 0);
      potenciometro = map(analogico0,0,1024,350,4000);  // adaptamos el valor leido a un retardo 250microSeg y 4000microSeg
      analogico1 = analogRead(A1);     // leemos el potenciometro en el puero A1
      porRetInaExa = map(analogico1, 0, 1024, 0, 100);
      potenciometro2 = map(analogico1,0,1024,300,2500);  // adaptamos el valor leido a un retardo de 0.3 a 2.5seg
      digitalWrite(direccion, LOW);    // cambiamos de dirección segun pulsador
      //PANTALLA
      lcd.setCursor(0, 0);		// ubica cursor en columna 0 y linea 0
      lcd.print(porRetVeloc);  // Valor pauda en microsegundo
      lcd.print("% Breathing.V");
      lcd.setCursor(0, 1);
      lcd.print(porRetInaExa);  // Valor pauda en microsegundo
      lcd.print("% Inhal.Pause");
      exalacionVel = (potenciometro + 800);
      //INAHALACION
      for (int x = 0; x < 1370 ; x++) {      // 200 pasos para 180° y como tiene un reductor se necesita 1370 pasos para 180°
        digitalWrite(steps, HIGH);         // Aqui generamos un flanco de bajada HIGH - LOW
        delayMicroseconds(5);              // Pequeño retardo para formar el pulso en STEP
        digitalWrite(steps, LOW);         // y el A4988 de avanzara un paso el motor
        delayMicroseconds(potenciometro); // generamos un retardo con el valor leido del potenciometro
      }
      delay(potenciometro2); // retardo de inahalacion y exalacion
      //EXALACION
      for (int x = 0; x < 1370 ; x++) {      // 200 pasos para 180° y como tiene un reductor se necesita 1370 pasos para 180°
        digitalWrite(steps, HIGH);         // Aqui generamos un flanco de bajada HIGH - LOW
        delayMicroseconds(5);              // Pequeño retardo para formar el pulso en STEP
        digitalWrite(steps, LOW);         // y el A4988 de avanzara un paso el motor
        delayMicroseconds(exalacionVel); // generamos un retardo con el valor leido del potenciometro Y LE sumamos 800MICROSEGUNDOS
      }
      delay(potenciometro2); // retardo de inahalacion y exalacion
    }else {
      lcd.print("The P-Keeper"); // Enviar el mensaje
      lcd.setCursor(0, 1);
      lcd.print(".wordpress.com");
      delay(100);
    }
}
// ISR pin 2, iniciar detener
void iniciarDetener()
{
  running = !running;             // toggle running variable
}
