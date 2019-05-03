#include "Oled.h"
#include "SerialCommand.h"
#include "DCCpp_Uno.h"
#include "Accessories.h"
#include "Sensor.h"
#include "Outputs.h"
#include "EEStore.h"
#include "Comm.h"

#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
#define PIE_          "lamaquetade"
#define ACTUALIZA_INFO 250  //la pantalla se actualiza cada 250ms
#define PAUSA_          24  //Pausa de 6 segundos para mostrar la información. 4x6 = 24


Adafruit_SSD1306 display(OLED_RESET);

bool on_off = false;
int cab = 0;
int velocidad = 0;
int direccion = 0;
int nId = 0;
int address = 0;
int subAddress = 0;
int tStatus = 0;
int idsensor = 0;
int pinsensor = 0;
int estadosensor = 0;
int idsalida = 0;
int pinsalida = 0;
int estadosalida = 0;
int pantalla = 0;
int timerpantalla = 0;
String pantalla1cadena1 = "";
String pantalla1cadena2 = "";
String pantalla1cadena3 = "";
long previousMillis = 0;
const long interval = ACTUALIZA_INFO;



void Oled::init() {

  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(55, 15);
  display.clearDisplay();
  display.println("DCC++");
  display.setCursor(55, 45);
  display.println (VERSION);
  display.display();
  delay(1);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);  //
  display.stopscroll();
  delay(3000);
  display.clearDisplay();

}

void Oled::initScreen() {
  startScreen();
  if (on_off) {
    display.setCursor(5, 1);
    display.print("DCC++");
    display.setCursor(50, 1);
    display.setTextColor(BLACK, WHITE); // Texto invertido
    display.println(" ON ");
    display.setCursor(5, 13);
    display.setTextColor(WHITE);
    // CAB
    display.print("Throttle: ");
    display.print(cab);
    display.print(" ");
    display.print(velocidad);
    display.print(" ");
    if (direccion) display.println(">>");
    else display.println("<<");
    // accessorios:
    display.setCursor(5, 23);
    display.print("Accessor: ");
    display.print(address);
    display.print(" ");
    display.print(subAddress);
    display.print(" ");
    if (tStatus == 0) display.println("Closed");
    else display.println("Open");
    // sensores:
    display.setCursor(5, 33);
    display.print("Sensores: ");
    display.print(idsensor);
    display.print(" ");
    display.print(pinsensor);
    display.print(" ");
    display.print(estadosensor);
    // Pines de Salida
    display.setCursor(5, 43);
    display.print("Salidas:  ");
    display.print(idsalida);
    display.print(" ");
    display.print(pinsalida);
    display.print(" ");
    display.print(estadosalida);

  } else {        //PANTALLA INICIAL
    
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(5, 1);  display.print("DCC++"); display.setCursor(80, 1); display.println("OFF");
    display.setCursor(5, 21);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("Ver: "); display.print(VERSION); display.print(" "); display.print(OLED_NAME);//display.print(ARDUINO_TYPE);
    display.setCursor(5, 33);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print("Com: ");
#if COMM_TYPE == 0                 // Serial Selected
    display.print(COMM_NAME);
#elif COMM_TYPE == 1          // Ethernet Shield Card Selected
    display.print(COMM_SHIELD_OLED_NAME);
#endif
    display.setCursor(5, 45);
    display.print("D:  ");  display.print(EEStore::eeStore->data.nTurnouts);
    display.print(" S:  "); display.print(EEStore::eeStore->data.nSensors);
    display.print(" O:  "); display.print(EEStore::eeStore->data.nOutputs);
  }
  display.setCursor(30, 56);
  display.print(PIE_);
  display.display();
}

void Oled::updateOled() {

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (timerpantalla >= 0) { //si el temporizador es mayor que 0 selecciona una de las pantallas

      switch (pantalla) { // Selecciona la pantalla que segun la acción.Definir/Eliminar/editar...
        case 1:                 //pantalla elemento
          Oled::pantalla1();
          break;
        case 2:                 //Pantalla error
          Oled::pantalla2();
          break;
        case 3:
          Oled::pantalla3();
          break;
        case 4:
          Oled::pantalla4();
          break;
        default: //por defecto la pantalla siempre en modo inicio
          break;
      }
      timerpantalla--;
      
    } else { //Si el temporizador es = o menor que 0 muestra la pantalla inicial
      
      Oled::initScreen();
    }
  }
}
void Oled::showOledOn(bool onOff) {
  on_off = onOff;
  timerpantalla = 0;
  startScreen();
}

void Oled::printThrottle(int tCab, int vel, int direc) {
  cab = tCab;
  velocidad = vel;
  direccion = direc;
  startScreen();
}

void Oled::printAccesories(int addr, int subAddr, int Status) {
  address = addr;
  subAddress = subAddr;
  tStatus = Status;
  startScreen();

}

void Oled::printSensor(int num, int Pin, int pullUp) {
  idsensor = num;
  pinsensor = Pin;
  estadosensor = pullUp;
  startScreen();
}

void Oled::printOutput(int num, int Pin, int state) {
  idsalida = num;
  pinsalida = Pin;
  estadosalida = state;
  startScreen();
}

void Oled::pantalla1() { //Corresponde a printDefined
  display.setCursor(20, 15);
  display.println(pantalla1cadena1);
  display.setTextSize(1);
  display.setCursor(35, 30);
  display.println(pantalla1cadena2);
  display.setCursor(2, 45);
  display.println(pantalla1cadena3);
  display.display();

}
void Oled::pantalla2() { //Corresponde a Error
  display.setCursor(5, 5);
  display.println(pantalla1cadena1);
  display.setTextSize(1);
  display.setCursor(5, 25);
  display.println(pantalla1cadena2);
  display.display();
}
void Oled::pantalla3() { //Corresponde a Error
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(25, 15);
  display.println(pantalla1cadena1);
  display.setTextSize(2);
  display.setCursor(25, 35);
  display.println(pantalla1cadena2);
  display.display();
}
void Oled::pantalla4() { //Corresponde a Sram
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(25, 5);
  display.println(pantalla1cadena1);     //2KB para UNO - 8KB para MEGA
  display.setTextSize(2);
  display.setCursor(10, 45);
  display.print(pantalla1cadena2);
  display.println(pantalla1cadena3);
}

void Oled::printDefined(int option, int n, int s, int m) {
  startScreen();

  switch (option) {
    case 1:
      pantalla1cadena1 = "Desvio definido";
      pantalla1cadena3 = (String) "id: " + n + " D: " + s + " S: " + m;
      break;
    case 2:
      pantalla1cadena1 = "Sensor definido";
      pantalla1cadena3 = (String) "id: " + n + " Pin: " + s + " P: " + m;
      break;
    case 3:
      pantalla1cadena1 = "Salida definida";
      pantalla1cadena3 = (String) "id: " + n + " Pin: " + s + " F: " + m;
      break;
  }

  pantalla1cadena2 = "con exito";
  timerpantalla = PAUSA_;
  pantalla = 1; //Pantalla para mostrar elemento definido
}




void Oled::printDelete(int option, boolean estado) {
  pantalla1cadena3 = "";
  startScreen();
  if (estado) pantalla1cadena1 = "Error al elimnar";
  else pantalla1cadena2 = "Con exito";
  display.setCursor(20, 15);
  switch (option) {
    case 1:
      if (estado) pantalla1cadena1 = "Desvio elinado";
      else pantalla1cadena2 = "Desvio";
      break;
    case 2:
      if (estado) pantalla1cadena1 = "Sensor eliminado";
      else pantalla1cadena2 = "Sensor";
      break;
    case 3:
      if (estado) pantalla1cadena1 = "Salida eliminada";
      else pantalla1cadena2 = "Salida";
      break;
  }
  timerpantalla = PAUSA_;
  pantalla = 1; //Pantalla para mostrar elemento definido

}

void Oled::printSaved( int option, bool estado) {
  pantalla1cadena3 = "";
  startScreen();
  if (estado) {
    pantalla1cadena1 = "Memoria Guardada";
    pantalla1cadena2 = "Con exito";
    pantalla = 1; //Pantalla para mostrar elemento guardado
  } else {
    pantalla1cadena1 =  "Error";
    pantalla1cadena2 =  "Almacenando";
    pantalla1cadena3 =  "";
    pantalla = 2; //Pantalla para mostrar el exito al guardar la memoria EEPROM
  }
  timerpantalla = PAUSA_;
}

void Oled::printErrorOptionEmpty(int option) {
  startScreen();
  switch (option) {
    case 1:
      pantalla1cadena1 = "No hay Desvios";
      break;
    case 2:
      pantalla1cadena1 = "No hay Sensores";
      break;
    case 3:
      pantalla1cadena1 = "No hay Salidas";
      break;
  }
  pantalla1cadena2 = "definidos";
  pantalla1cadena3 = "";
  timerpantalla = PAUSA_;
  pantalla = 1; //Pantalla para mostrar elemento definido

}
void Oled::printErrorOptionUndefined(int option) {
  startScreen();
  switch (option) {
    case 1:
      pantalla1cadena1 = "No hay Desvio";
      break;
    case 2:
      pantalla1cadena1 = "No hay Sensor";
      break;
    case 3:
      pantalla1cadena1 = "No hay Salida";
      break;
  }
  pantalla1cadena2 = "definido ";
  pantalla1cadena3 = "";
  timerpantalla = PAUSA_;
  pantalla = 1; //Pantalla para mostrar elemento definido

}
void Oled::printDeleteMemory() {  //Comando <e> /***** BORRA LA CONFIGURACION DE LA EEPROM  ****/

  pantalla1cadena1 = "Memoria";

  pantalla1cadena2 =  "Eliminada";
  pantalla1cadena3 =  "";
  timerpantalla = PAUSA_;
  pantalla = 3; //Pantalla para mostrar 'memoria borrada'

}
void Oled::startScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
}
void Oled::printSram(int ram) {  //Comando <F> INTENTOS PARA DETERMINAR CUANTA SRAM LIBRE ESTA DISPONIBLE EN ARDUINO  ****/
  pantalla1cadena2  = (String) ram;
  pantalla1cadena1  = "Memoria disponible";     //2KB para UNO - 8KB para MEGA
  pantalla1cadena3  = "bytes";
  timerpantalla = PAUSA_;
  pantalla = 4; //Pantalla para mostrar el estado de la memoria Sram


}
