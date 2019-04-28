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
#define DESVIOS_      "Desvios"
#define SENSORES_     "Sensores"
#define SALIDAS_      "Salidas"

Adafruit_SSD1306 display(OLED_RESET);
bool presentacion = true;
int scrollPos = 0;
boolean scrolling = true;
bool on_off = false;
int cab = 0;
int velocidad = 0;
int direccion = 0;
int nId = 0;
int address = 0;
int subAddress = 0;
int tStatus = 0;
int num = 0;
int Pin = 0;
int pullUp = 0;


void Oled::init() {



  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(60, 0);
  display.clearDisplay();
  display.println("DCC++");
  display.setCursor(55, 25);
  display.println (VERSION);
  display.display();
  delay(1);
  display.startscrollleft(0x00, 0x0F);
  delay(3500);
  display.stopscroll();
  delay(1000);
  display.clearDisplay();

}




void Oled::initScreen() {
  startScreen();
  if (on_off) {
    display.setCursor(5, 1);
    display.print("DCC++");
    display.setCursor(50, 1);
    display.setTextColor(BLACK, WHITE); // 'inverted' text
    //display.setCursor(65,3);
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
    // accessories:
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
    display.print(num);
    display.print(" ");
    display.print(Pin);
    display.print(" ");
    display.print(pullUp);
    // Pines de Salida
    display.setCursor(5, 43);
    display.print("Salidas:  ");
    display.print(num);
    display.print(" ");
    display.print(Pin);
    display.print(" ");
    display.print(pullUp);

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
  //delay(2000);
}


void Oled::showOledOn(bool onOff) {
  on_off = onOff;
  Oled::initScreen();
}
void Oled::printThrottle(int tCab, int vel, int direc) {
  cab = tCab;
  velocidad = vel;
  direccion = direc;
  Oled::initScreen();
}
void Oled::printAccesories(int addr, int subAddr, int Status) {
  //nId = id;
  address = addr;
  subAddress = subAddr;
  tStatus = Status;
  Oled::initScreen();

}
void Oled::printSensor(int num, int Pin, int pullUp) {

}

void Oled::printDelete(int option, boolean estado) {
  startScreen();
  String mensaje1 = "";
  String mensaje2 = "";
  if (estado) mensaje1 = "Error al elimnar";
  else mensaje2 = "Con exito";
  display.setCursor(20, 15);
  switch (option) {
    case 1:
      if (estado) mensaje1 = "Desvio elinado";
      else mensaje2 = "Desvio";
      break;
    case 2:
      if (estado) mensaje1 = "Sensor eliminado";
      else mensaje2 = "Sensor";
      break;
    case 3:
      if (estado) mensaje1 = "Salida eliminada";
      else mensaje2 = "Salida";
      break;
  }
  display.println(mensaje1);
  display.setTextSize(1);
  display.setCursor(35, 30);
  display.println(mensaje2);
  display.display();
  delay(6000);
  Oled::initScreen();
}

void Oled::printSaved( int option, bool estado) {
  startScreen();
  if (estado){
  display.setCursor(20, 15);
  display.println("Memoria Guardada");
  display.setTextSize(1);
  display.setCursor(35, 30);
  display.println("Con exito");
  display.display();
  delay(4000);
  } else{
  display.setCursor(5, 5);
  display.println("Error");
  display.setTextSize(1);
  display.setCursor(5, 25);
  display.println("Almacenando");
  display.display();
  delay(5000);
  }
  Oled::initScreen();

}
/**
void Oled::printErrorMemory() {
  startScreen();
  display.setCursor(5, 5);
  display.println("Error");
  display.setTextSize(1);
  display.setCursor(5, 25);
  display.println("Almacenando");
  display.display();
  delay(5000);
  }
  Oled::initScreen();
}*/




void Oled::printDefined(int option, int n, int s, int m) {
  startScreen();
  String mensaje = "";
  display.setCursor(20, 15);
  switch (option) {
    case 1:
      display.println("Desvio definido");
      mensaje = (String) "id: " + n + " D: " + s + " S: " + m;
      break;
    case 2:
      display.println("Sensor definido");
      mensaje = (String) "id: " + n + " Pin: " + s + " P: " + m;
      break;
    case 3:
      display.println("Salida definida");
      mensaje = (String) "id: " + n + " Pin: " + s + " F: " + m;

      break;
  }
  display.setTextSize(1);
  display.setCursor(35, 30);
  display.println("con exito");
  display.setCursor(2, 45);
  display.println(mensaje);
  display.display();
  delay(6000);
  Oled::initScreen();
}



void Oled::printErrorOptionEmpty(int option) {
  startScreen();
  display.setCursor(20, 15);
  switch (option) {
    case 1:
      display.println("No hay Desvios");
      break;
    case 2:
      display.println("No hay Sensores");
      break;
    case 3:
      display.println("No hay Salidas");
      break;
  }
  display.setTextSize(1);
  display.setCursor(35, 30);
  display.println("definidos");
  display.display();
  delay(6000);
  Oled::initScreen();
}
void Oled::printErrorOptionUndefined(int option) {
  startScreen();
  display.setCursor(20, 15);
  switch (option) {
    case 1:
      display.println("No hay Desvio");
      break;
    case 2:
      display.println("No hay Sensor");
      break;
    case 3:
      display.println("No hay Salida");
      break;
  }
  display.setTextSize(1);
  display.setCursor(35, 30);
  display.println("definido ");
  display.setCursor(20, 45);
  display.println("en la ID ");
  display.display();
  delay(3000);
  Oled::initScreen();
}
void Oled::printDeleteMemory() {  //Comando <e> /***** BORRA LA CONFIGURACION DE LA EEPROM  ****/    
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(25, 15);
  display.println("Memoria");
  display.setTextSize(2);
  display.setCursor(25, 35);
  display.println("Eliminada");
  display.display();
  delay(5000);
  Oled::initScreen();
}
void Oled::startScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
}
void Oled::printSram(int ram){   //Comando <F> INTENTOS PARA DETERMINAR CUANTA SRAM LIBRE ESTA DISPONIBLE EN ARDUINO  ****/                        
  display.clearDisplay();
  String memoria = (String) ram;
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(25, 5);
  display.println("Memoria disponible");     //2KB para UNO - 8KB para MEGA
  display.setTextSize(2);
  display.setCursor(10, 45);
  display.print(memoria);
  display.println("bytes");
  display.display();
  delay(5000);
  Oled::initScreen();
  
}
