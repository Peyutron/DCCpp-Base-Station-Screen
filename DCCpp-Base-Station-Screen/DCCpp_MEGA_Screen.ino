/**********************************************************************

DCC++ BASE STATION
COPYRIGHT (c) 2013-2016 Gregg E. Berman

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses

**********************************************************************/
/**********************************************************************
      
DCC ++ BASE STATION es un programa escrito en C ++ para el Arduino Uno y Arduino Mega
utilizando el IDE Arduino 1.6.6.

Permite que un Arduino Uno estándar o Mega con un Arduino Motor Shield (así como otros)
para ser utilizado como comando digital en pleno funcionamiento y control de la estación base (DCC)
Para el control de maquetas de modelos de tren conforme a los actuales estandares de la actual
National Model Railroad Association (NMRA) 

Esta versión de DCC ++ BASE STATIOM soporta:

  * Direcciones de Locomotoras de 2-byte y 4-byte
  *  Control simultáneo de varias locomotoras
  * 128-step pasos de aceleracion
  * Functions de cabin F0-F28
  * funciones Cab F0-F28 * Activar / desactivar funciones accesorias utilizando 512 direcciones, cada una con 4 subdirecciones 
      - incluye una funcionalidad opcional para controlar y guardar la dirección de los desvíos conectados  
  * Programación en la Pista Principal de Operaciones
      - Escribe variables bytes de configuracion
      - Establece/Limpia bits especificos de configuracion
  * Programación en la vía de programación
      - Escribe variables bytes de configuracion
      - sEstablece/Limpia bits especificos de configuracion
      - Lecturas de variables bytes de configuracion


DCC++ BASE STATION es controlado por comandos de texto simple recibidos 
en el puerto serial de Arduino. Los usuarios pueden escribir los comandos
directamente en el monitor serial del IDE de Arduino o puedes mandarlos desde otro 
dispositivo o programa de ordenador.

Cuando se compila para un Arduino Mega, se puede usar un Shield Ethernet para 
las comunicaciones en lugar de usar las comunicaciones seriales.

DCC++ CONTROLLER esta disponible por separado bajo una licencia similar a open-source,
es un programa escrito en java usando la libreria Processing y el IDE Processing esto
esto provee una completa y configurable interface grafica para el control de maquetas
de trenes con DCC++ BASE STATION.

Con la excepcion de una fuente de alimentacion de 15V que puede comprarse en cualquier 
tienda de electronica, no se necesita ningun hardware adicional.

Ni DCC ++ BASE STATION ni DCC ++ CONTROLLER usan ninguna propiedad conocida o hardware comercial, 
software, interfaces, especificaciones o métodos relacionados para el control de modelos de trenes 
que usan los estándares NMRA DCC. 
Ambos programas son totalmente originales, desarrollados por el autor, y no se deriva de ningún conocido 
comercial,paquetes de control de ferrocarril de modelo libre o de código abierto por cualquier otra parte. 

Sin embargo, DCC ++ BASE STATION y DCC ++ CONTROLLER dependen en gran medida del IDE y
las bibliotecas integradas asociadas con Arduino y Processing. Muchísimas gracias a aquellos
responsable de estas increíbles iniciativas de código abierto que permiten programas como
DCC ++ debe ser desarrollado y distribuido de la misma manera.

REFERENCIAS:

  NMRA DCC Estandares:         http://www.nmra.org/index-nmra-standards-and-recommended-practices
  Arduino:                     http://www.arduino.cc/
  Processing:                  http://processing.org/
  GNU General Public License:  http://opensource.org/licenses/GPL-3.0


BRIEF NOTES ON THE THEORY AND OPERATION OF DCC++ BASE STATION:

DCC ++ BASE STATION para Arduino Uno configura el pin de interrupción OC0B asociado con el temporizador 0,
y el pin de interupt OC1B asociado con el temporizador 1, para generar señales unipolares 0-5V por separado
que cada uno codifica correctamente cero y un bit conforme a los Estándares de sincronización DCC. 
Cuando se compila para Mega, DCC ++ BASE STATION usa OC3B en vez de OC0B.

La serie de flujos de bits DCC se agrupan en paquetes que forman la base de una instrucción DCC estándar. 
Los paquetes se almacenan en los registros de paquetes que contienen métodos para actualizar y poner en 
cola según los comandos de texto enviados por el usuario (u otro programa) sobre la interfaz serial. 
Hay un conjunto de registros que controla la pista de operaciones principales y otra que controla la 
pista de programación.

Para la pista de operaciones principales, los paquetes para almacenar la configuración del acelerador 
de la maquina se almacenan en registros numerados del 1 al MAX_MAIN_REGISTERS (como se define en DCCpp_Uno.h).
En general, se considera buena práctica enviar continuamente paquetes de control de aceleración
a cada maquina para que si un motor pierda momentáneamente la conectividad eléctrica con las pistas recibirá 
rápidamente otra señal de control del acelerador tan pronto como la conectividad sea restaurada (como cuando 
un tren pasa sobre una porción áspera de la pista o el frog de un cruce).

Por lo tanto, la estación base DCC ++ realiza un ciclo secuencial a través de cada registro de paquete de pistas 
de operaciones principales. que ha sido cargado con una configuración de control del acelerador para una maquina 
determinada. Para cada registro, transmite los bits del paquete DCC apropiados a la pista, luego pasa al siguiente 
registro sin ninguna pausa para asegurar que se proporcione potencia bipolar continua a las pistas.
Las actualizaciones de la configuración del acelerador almacenadas en cualquier registro de paquetes dado se 
realizan en un doble buffer La moda y el secuenciador apuntan a ese registro inmediatamente después de haber sido 
modificados para que los bits DCC actualizados puede transmitirse a la cabina apropiada sin demora o cualquier 
interrupción en la señal de potencia bipolar. Las maquinas identificadas en cada ajuste de acelerador almacenado 
deben ser únicas en todos los registros. Si dos registros contienen el ajuste del acelerador para la misma maquina, 
el acelerador en el motor oscilará entre los dos, lo que probablemente no sea un resultado deseable.

Tanto para la pista de operaciones principales como para la pista de programación, también hay un registro de paquetes 
especial con id = 0 que se usa para almacenar todos los demás paquetes DCC que no requieren transmisión continua a las pistas.
Esto incluye paquetes DCC para controlar las funciones del decodificador, configurar decodificadores accesorios y leer y escribir 
variables de configuración. Es una práctica común que la transmisión de estos paquetes únicos se repita varias veces para garantizar
la recepcion correcta por el decodificador receptor. Los decodificadores DCC están diseñados para escuchar las repeticiones del mismo paquete
y siempre que no haya otros paquetes recibidos entre las repeticiones, el decodificador DCC no repetirá la acción en sí.
Algunos decodificadores DCC realmente requieren el recibo de múltiples paquetes idénticos de una sola vez secuenciales como una forma de
verificar la transmisión correcta antes de actuar según las instrucciones contenidas en esos paquetes

Un Arduino Motor Shield (o similar), alimentado por una fuente de alimentación estándar de 15V CC y conectado encima del Arduino Uno o Mega,
se utiliza para transformar las señales lógicas DCC 0-5V producido por el Timer de Uno interrumpe en señales DCC bipolares 0-15V apropiadas.

Esto se logra en el Uno usando un pequeño cable puente para conectar la salida OC1B del Uno (pin 10) a la entrada de DIRECCIÓN A del 
Motor Shield (pin 12), y otro cable puente pequeño para conectar la salida OC0B del Uno (pin 5) a la entrada DIRECTION B del Motor Shield (pin 13).

Para el Mega, la salida OC1B se produce directamente en el pin 12, por lo que no se necesita un puente para conectarse a la entrada de
DIRECCION A de Motor Shield. Sin embargo, se necesita un pequeño cable puente para conectar la salida OC3B del Mega (pin 2)
a la entrada DIRECCIÓN B del Motor Shield (pin 13).

Otros protectores de motor pueden requerir diferentes conjuntos de puentes o configuraciones (ver Config.h y DCCpp_Uno.h para más detalles).

Cuando se configuran como tales, las salidas CANAL A y CANAL B del Motor Shield pueden ser conectado directamente a las pistas. Este software 
supone que el CANAL A está conectado a la Pista de Operaciones Principal, y el CANAL B está conectado a la Pista de Programación.

DCC ++ BASE STATION se divide en varios módulos, cada uno con su propio archivo de encabezado:

  DCCpp_Uno:        declara los objetos globales requeridos y contiene la configuración inicial de Arduino ()
                    y las funciones de Arduino loop (), así como también el código de interrput para OC0B y OC1B.
                    También incluye declaraciones de matriz opcional de interrupciones y matriz opcional de sensores

  SerialCommand:    contiene métodos para leer e interpretar comandos de texto de la línea serie,
                    procesar esas instrucciones y, si es necesario, llamar a los métodos apropiados de Packet RegisterList
                    para actualizar ya sea la pista principal o la programación de los registros de paquetes de pistas

  PacketRegister:   contiene métodos para cargar, almacenar y actualizar los registros de paquetes con instrucciones DCC

  CurrentMonitor:   contiene métodos para monitorear e informar por separado la corriente extraída del CANAL A y
                    CANAL B de Arduino Motor Shield's, y apague la energía si se produce una sobrecarga de cortocircuito
                    es detectado

  Accesorios:       contiene métodos para operar y almacenar el estado de los desvíos definidos opcionalmente controlados
                    mediante un decodificador de accesorios fijos DCC.

  Sensor:           contiene métodos para controlar e informar sobre el estado del infrarrojo opcionalmente definido
                    sensores integrados en la pista principal y conectados a varios pines en el Arudino Uno

  Outputs:          contiene métodos para configurar uno o más pines Arduino como salida para su propio uso personalizado

  EEStore:           contiene métodos para almacenar, actualizar y cargar diversas configuraciones y estado de DCC
                    (por ejemplo, los estados de todos los desvíos definidos) en la EEPROM para recuperarlos después del encendido

DCC ++ BASE STATION se configura a través del archivo Config.h que contiene todos los parámetros definibles por el usuario

************************************************** ********************/

// BEGIN BY INCLUDING THE HEADER FILES FOR EACH MODULE
#include "Oled.h"
#include "DCCpp_Uno.h"
#include "PacketRegister.h"
#include "CurrentMonitor.h"
#include "Sensor.h"
#include "SerialCommand.h"
#include "Accessories.h"
#include "EEStore.h"
#include "Config.h"
#include "Comm.h"
//#include <SPI.h>
//#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>

//#define OLED_RESET 4
//Adafruit_SSD1306 display(OLED_RESET);
void showConfiguration();

// SET UP COMMUNICATIONS INTERFACE - FOR STANDARD SERIAL, NOTHING NEEDS TO BE DONE

#if COMM_TYPE == 1
  byte mac[] =  MAC_ADDRESS;                                // Crea una direccion MAC (Para ser usada por DHCP cuando se inicie el servidor)
  EthernetServer INTERFACE(ETHERNET_PORT);                  // Crea una instancia para el Servidor de red
#endif

// NEXT DECLARE GLOBAL OBJECTS TO PROCESS AND STORE DCC PACKETS AND MONITOR TRACK CURRENTS.
// NOTE REGISTER LISTS MUST BE DECLARED WITH "VOLATILE" QUALIFIER TO ENSURE THEY ARE PROPERLY UPDATED BY INTERRUPT ROUTINES

volatile RegisterList mainRegs(MAX_MAIN_REGISTERS);    // create list of registers for MAX_MAIN_REGISTER Main Track Packets
volatile RegisterList progRegs(2);                     // create a shorter list of only two registers for Program Track Packets

CurrentMonitor mainMonitor(CURRENT_MONITOR_PIN_MAIN,"<p2>");  // create monitor for current on Main Track
CurrentMonitor progMonitor(CURRENT_MONITOR_PIN_PROG,"<p3>");  // create monitor for current on Program Track

///////////////////////////////////////////////////////////////////////////////
// MAIN ARDUINO LOOP
///////////////////////////////////////////////////////////////////////////////

void loop(){
  
  SerialCommand::process();              // Comprueba y procesa un nuevo comando serial
  if(CurrentMonitor::checkTime()){      // Si ha pasado suficiente tiempo desde la ultima actualizacion, comprueba el consumo en la via principal y de programacion 
    mainMonitor.check();
    progMonitor.check();
    Oled::updateOled();
  }

  Sensor::check();    // Comprueba los sensores activados/desactivados
  
  
} // end loop

///////////////////////////////////////////////////////////////////////////////
// INITIAL SETUP
///////////////////////////////////////////////////////////////////////////////

void setup(){  

  Serial.begin(115200);            // Configura la interfaz serial
  Serial.flush();
  Oled::init(); //Inicia la pantalla con direccion definida en Config.h

  #ifdef SDCARD_CS
    pinMode(SDCARD_CS,OUTPUT);
    digitalWrite(SDCARD_CS,HIGH);     // Deselecciona la tarjeta SD
  #endif

  EEStore::init();                                          // inicia y carga las definiciones de desvios y sensores almacenados en la memoria EEPROM
  //Oled::initScreen();  


  pinMode(A5,INPUT);                                       // si el pin A5 está puesto a tierra al momento de la puesta en marcha, imprime la configuración del sistema y detiene
  digitalWrite(A5,HIGH);
  if(!digitalRead(A5))
    showConfiguration();

   //   Serial.print("<iDCC++ BASE STATION V-");
   //   Serial.print(VERSION);
   //   Serial.print(" PARA ARDUINO:");
      Serial.print(ARDUINO_TYPE);
      Serial.print(" / ");
      Serial.print(MOTOR_SHIELD_NAME);

 // Serial.print(" / ");
 /* Serial.print(__DATE__);
  Serial.print(" ");
  Serial.print(__TIME__);*/
  Serial.print(">");

  

  #if COMM_TYPE == 1
    #ifdef IP_ADDRESS
      Ethernet.begin(mac,IP_ADDRESS);           // Inicia la red una direccion IP FIJA
    #else
      Ethernet.begin(mac);                      // Inicia la red usando el DHCP para adquirir una direccion IP
    #endif
    INTERFACE.begin();
  #endif
             
  SerialCommand::init(&mainRegs, &progRegs, &mainMonitor);   // Crea una structura para leer y pasar comandos en la linea seial

  Serial.print("<N");
  Serial.print(COMM_TYPE);
  Serial.print(": ");

  #if COMM_TYPE == 0
    Serial.print("SERIAL>");
  #elif COMM_TYPE == 1
    Serial.print(Ethernet.localIP());
    Serial.print(">");
  #endif
  
  // CONFIGURA LA SALIDA TIMER _1 PARA DAR SALIDA A UNA SEÑAL DCC CON UN 50% DE CICLO EN EL PIN DE INTERRUPCION OC1B
  
  // El pin de direccion para el Motor Shield Canal A - VIA DE OPERACIONES PRINCIPAL
  // controlado por el pin 16-bit de Arduino TIMER 1 / Pin de interrupcion OC1B 
  // Los valores de los registros 16-bit OCR1A y OCR1B calibrados para el prescaler 1:1 con una frecuencia de reloj de 16MHz
  // Resultando ondas largas con 200 microsegundos con el bit CERO un pulso corto 116 microsegundos para el bit UNO, 50% de ciclo

  #define DCC_ZERO_BIT_TOTAL_DURATION_TIMER1 3199
  #define DCC_ZERO_BIT_PULSE_DURATION_TIMER1 1599

  #define DCC_ONE_BIT_TOTAL_DURATION_TIMER1 1855
  #define DCC_ONE_BIT_PULSE_DURATION_TIMER1 927

  pinMode(DIRECTION_MOTOR_CHANNEL_PIN_A,INPUT);      // ¡Asegúrate de que este pin no esté activo! La dirección será controlada por DCC SIGNAL en su lugar (abajo)
  digitalWrite(DIRECTION_MOTOR_CHANNEL_PIN_A,LOW);

  pinMode(DCC_SIGNAL_PIN_MAIN, OUTPUT);      // /ESTE PIN DE SALIDA ARDUINO DEBE CONECTARSE FÍSICAMENTE AL PIN PARA LA DIRECCIÓN-A EN EL CANAL DEL MOTOR-A

  bitSet(TCCR1A,WGM10);     // configura el TIMER 1 en FAST PWM, con TOP = OCR1A
  bitSet(TCCR1A,WGM11);
  bitSet(TCCR1B,WGM12);
  bitSet(TCCR1B,WGM13);

  bitSet(TCCR1A,COM1B1);    // establece el TIMER 1, OC1B (pin 10 / UNO, pin 12 / MEGA) para alternar (la dirección actual es arbitraria)
  bitSet(TCCR1A,COM1B0);

  bitClear(TCCR1B,CS12);    // establece el TIMER 1 prescale=1
  bitClear(TCCR1B,CS11);
  bitSet(TCCR1B,CS10);
    
  OCR1A=DCC_ONE_BIT_TOTAL_DURATION_TIMER1;
  OCR1B=DCC_ONE_BIT_PULSE_DURATION_TIMER1;
  
  pinMode(SIGNAL_ENABLE_PIN_MAIN,OUTPUT);   // habilitación maestra para el canal de motor A

  mainRegs.loadPacket(1,RegisterList::idlePacket,2,0);    //carga el paquete idle en el registro 1
      
  bitSet(TIMSK1,OCIE1B);    // habilita el vector de interrupción para la salida del TIMER 1 Compare B Match (OCR1B)    

  // CONFIGURA LA SALIDA TIMER _0 PARA DAR SALIDA A UNA SEÑAL DCC CON UN 50% DE CICLO EN EL PIN DE INTERRUPCION OC0B (UNO) o 0C3B (MEGA)
  
#ifdef ARDUINO_AVR_UNO      // configuracion para UNO

  // El pin de direccion para el Motor Shield Canal B - VIA DE PROGRAMACION
  // controlado por el pin 8-bit de Arduino TIMER 0 / Pin de interrupcion OC0B 
  // Los valores de los registros 8-bit OCR0A y OCR0B calibrados para el prescaler 1:64 con una frecuencia de reloj de 16MHz
  // Resultando ondas largas con 200 microsegundos con el bit CERO un pulso corto 116 microsegundos para el bit UNO,tan cerca como sea posiblem al 50% del ciclo de trabajo

  #define DCC_ZERO_BIT_TOTAL_DURATION_TIMER0 49
  #define DCC_ZERO_BIT_PULSE_DURATION_TIMER0 24

  #define DCC_ONE_BIT_TOTAL_DURATION_TIMER0 28
  #define DCC_ONE_BIT_PULSE_DURATION_TIMER0 14
  
  pinMode(DIRECTION_MOTOR_CHANNEL_PIN_B,INPUT);      //¡Asegúrate de que este pin no esté activo! La dirección será controlada por DCC SIGNAL en su lugar (abajo)
  digitalWrite(DIRECTION_MOTOR_CHANNEL_PIN_B,LOW);

  pinMode(DCC_SIGNAL_PIN_PROG,OUTPUT);      //ESTE PIN DE SALIDA ARDUINO DEBE CONECTARSE FÍSICAMENTE AL PIN PARA LA DIRECCIÓN-B EN EL CANAL DEL MOTOR-B

  bitSet(TCCR0A,WGM00);     // configura el TIMER 0 en FAST PWM, con TOP = OCR0A
  bitSet(TCCR0A,WGM01);
  bitSet(TCCR0B,WGM02);
     
  bitSet(TCCR0A,COM0B1);    // establece el TIMER 0, OC0B (pin 5) para alternar (la dirección actual es arbitraria)
  bitSet(TCCR0A,COM0B0);

  bitClear(TCCR0B,CS02);    // establece TIMER 0 prescale=64
  bitSet(TCCR0B,CS01);
  bitSet(TCCR0B,CS00);
    
  OCR0A=DCC_ONE_BIT_TOTAL_DURATION_TIMER0;
  OCR0B=DCC_ONE_BIT_PULSE_DURATION_TIMER0;
  
  pinMode(SIGNAL_ENABLE_PIN_PROG,OUTPUT);   // habilitación maestra para el canal de motor B

  progRegs.loadPacket(1,RegisterList::idlePacket,2,0);    // carga el paquete idle en el registro 1  
      
  bitSet(TIMSK0,OCIE0B);    // habilita el vector de interrupción para la salida del TIMER 0 Compare B Match (OCR0B)

#else     // Configuracion para MEGA

  // El pin de direccion para el Motor Shield Canal B - VIA DE PROGRAMACION
  // controlado por el pin 16-bit de Arduino TIMER 3 / Pin de interrupcion OC3B 
  // Los valores de los registros 16-bit OCR3A y OCR3B calibrados para el prescaler 1:1 con una frecuencia de reloj de 16MHz
  // Resultando ondas largas con 200 microsegundos con el bit CERO un pulso corto 116 microsegundos para el bit UNO ,con exactamente 50% del ciclo de trabajo

  #define DCC_ZERO_BIT_TOTAL_DURATION_TIMER3 3199
  #define DCC_ZERO_BIT_PULSE_DURATION_TIMER3 1599

  #define DCC_ONE_BIT_TOTAL_DURATION_TIMER3 1855
  #define DCC_ONE_BIT_PULSE_DURATION_TIMER3 927

  pinMode(DIRECTION_MOTOR_CHANNEL_PIN_B,INPUT);      // ¡Asegúrate de que este pin no esté activo! La dirección será controlada por DCC SIGNAL en su lugar (abajo)
  digitalWrite(DIRECTION_MOTOR_CHANNEL_PIN_B,LOW);

  pinMode(DCC_SIGNAL_PIN_PROG,OUTPUT);      // ESTE PIN DE SALIDA ARDUINO DEBE CONECTARSE FÍSICAMENTE AL PIN PARA LA DIRECCIÓN-B EN EL CANAL DEL MOTOR-B

  bitSet(TCCR3A,WGM30);     // configura el TIMER 3 en FAST PWM, con TOP = OCR3A
  bitSet(TCCR3A,WGM31);
  bitSet(TCCR3B,WGM32);
  bitSet(TCCR3B,WGM33);

  bitSet(TCCR3A,COM3B1);    // establece el TIMER 3, OC3B (pin 2) para alternar (la dirección actual es arbitraria)
  bitSet(TCCR3A,COM3B0);

  bitClear(TCCR3B,CS32);    // Establece el Timer 3 prescale=1
  bitClear(TCCR3B,CS31);
  bitSet(TCCR3B,CS30);
    
  OCR3A=DCC_ONE_BIT_TOTAL_DURATION_TIMER3;
  OCR3B=DCC_ONE_BIT_PULSE_DURATION_TIMER3;
  
  pinMode(SIGNAL_ENABLE_PIN_PROG,OUTPUT);   // habilitación maestra para el canal de motor B

  progRegs.loadPacket(1,RegisterList::idlePacket,2,0);    // carga el paquete idle en el registro 1
      
  bitSet(TIMSK3,OCIE3B);    // habilita el vector de interrupción para la salida del TIMER 3 Compare B Match (OCR3B)    )    
  
#endif

} // end setup
///////////////////////////////////////////////////////////////////////////////
// DEFINE LA LÓGICA DE INTERRUPCIÓN QUE GENERA LA SEÑAL DCC
///////////////////////////////////////////////////////////////////////////////

// Se llamará al código siguiente cada vez que se active una interrupción en OCNB, donde N puede ser 0 o 1.
// Está diseñado para leer el bit actual del paquete de registro actual y
// actualiza los contadores OCNA y OCNB de Timer-N a valores que producirán
// un pulso largo (200 microsegundos) o un pulso corto (116 microsegundos), que representan respectivamente 
// los bits DCC ZERO y DCC ONE.

// Estas son interrupciones impulsadas por hardware que se invocarán automáticamente cuando se activen independientemente de qué
// DCC ++ BASE STATION estaba procesando de otra forma. Pero una vez dentro de la interrupción, todas las demás rutinas de interrupción son temporalmente desabilitadas.
// Dado que un pulso corto solo dura 116 microsegundos, y hay DOS interrupciones separadas
// (una para los registros de via principal y otra para los registros de via de programacion), el código de interrupción debe completarse
// en mucho menos de 58 microsendios, de lo contrario no habría tiempo para que se ejecutara el resto del programa. Peor aún, si la lógica
// del código de interrupción alguna vez hizo que se ejecutara más de 58 microsendios, se ignoraría un disparador de interrupción, OCNA y OCNB
// los registros no se actualizarían, y el efecto neto sería una señal DCC que sigue enviando el mismo bit DCC repetidamente hasta que el
// el código de interrupción se completa y se puede volver a llamar.

// Una porción significativa de este programa está diseñada para hacer tanto del procesamiento pesado de crear un
// Corriente de bits DCC por adelantado, para que el código de interrupción a continuación sea lo más simple y eficiente posible.

// Tenga en cuenta que necesitamos crear dos copias muy similares del código: una para la interrupción de la via Principal OC1B y otra para la
// interrupcion de la via de programacion OCOB. Pero en lugar de crear una función genérica que genere gastos adicionales, creamos una macro
// que se puede invocar con los parámetros adecuados para cada interrupción. Esto aumenta ligeramente el tamaño de la base de código al duplicar
// parte de la lógica para cada interrupción, pero ahorra tiempo adicional.

// Como está estructurado, el código de interrupción a continuación completa a una media de poco menos de 6 microsegundos con un caso peor de algo menos de 11 microsegundos
// cuando se carga un nuevo registro y la lógica necesita cambiar los punteros de paquete de registro activo.

// CODIGO DE INTERUPCION  MACRO:  R=LISTA DE REGISTROS (mainRegs or progRegs), y N=TIMER (0 o 1)

#define DCC_SIGNAL(R,N) \
  if(R.currentBit==R.currentReg->activePacket->nBits){    /* IF no hay mas bits en este paquete DCC */ \
    R.currentBit=0;                                       /*   restablecer el puntero del bit actual y determinar qué Registro y Paquete procesar a continuación --- */\   
    if(R.nRepeat>0 && R.currentReg==R.reg){               /*   IF registro actual es el primer registro Y debe repetirse */ \
      R.nRepeat--;                                        /*      cuenta de repetición de decremento; resultado es este mismo paquete se repetirá */ \
    } else if(R.nextReg!=NULL){                           /*   ELSE IF otro Register ha sido actualizado */ \
      R.currentReg=R.nextReg;                             /*      actualiza currentReg a nextReg */ \
      R.nextReg=NULL;                                     /*     resetea nextReg a NULL */ \
      R.tempPacket=R.currentReg->activePacket;            /*     voltear activo y actualizar paquetes */ \        
      R.currentReg->activePacket=R.currentReg->updatePacket; \
      R.currentReg->updatePacket=R.tempPacket; \
    } else{                                               /*   ELSE simplemente pasa al siguiente registro */ \
      if(R.currentReg==R.maxLoadedReg)                    /*     BUT IF este es el último registro cargado */ \
        R.currentReg=R.reg;                               /*       primero reestablece currentReg a registro base, THEN */ \
      R.currentReg++;                                     /*     Incrementar el registro actual (tenga en cuenta que esta lógica hace que el Registro [0] se salte cuando simplemente se pasa por todos los Registros) */ \
    }                                                     /*   END-ELSE */ \
  }                                                       /* END-IF: currentReg, activePacket, y currentBit ahora debería estar configurado correctamente para apuntar al próximo bit DCC */ \
                                                          \
  if(R.currentReg->activePacket->buf[R.currentBit/8] & R.bitMask[R.currentBit%8]){     /* IF bit es un UNO  */ \
    OCR ## N ## A=DCC_ONE_BIT_TOTAL_DURATION_TIMER ## N;                               /*   establece OCRA para timer N para un ciclo completo de trabajo del bit DCC UNO */ \
    OCR ## N ## B=DCC_ONE_BIT_PULSE_DURATION_TIMER ## N;                               /*   establece OCRB para timer N para medio ciclo de duracion para el bit DCC UNO pero */ \
  } else{                                                                              /* ELSE si es un CERO */ \
    OCR ## N ## A=DCC_ZERO_BIT_TOTAL_DURATION_TIMER ## N;                              /*    establece OCRA para timer N para un ciclo completo de trabajo del bit DCC ZERO */ \
    OCR ## N ## B=DCC_ZERO_BIT_PULSE_DURATION_TIMER ## N;                              /*   establece OCRB para timer N para medio ciclo de duracion para el bit DCC ZERO  */ \
  }                                                                                    /* END-ELSE */ \ 
                                                                                       \ 
  R.currentBit++;                                         /* apunta al siguiente bit en el Paquete actual */  
  
///////////////////////////////////////////////////////////////////////////////

// AHORA UTILICE EL MACRO DE ARRIBA PARA CREAR EL CÓDIGO PARA CADA INTERRUPCIÓN

ISR(TIMER1_COMPB_vect){              // configura el servicio de interrupción para OCR1B de TIMER-1 que voltea el bit de dirección del Motor Shield Channel A controlando la via principal
  DCC_SIGNAL(mainRegs,1)
}

#ifdef ARDUINO_AVR_UNO      // Configuracion para UNO

ISR(TIMER0_COMPB_vect){              // configura el servicio de interrupción para OCR1B de TIMER-0 que cambia el bit de dirección de Motor Shield Channel B que controla la via de programacion
  DCC_SIGNAL(progRegs,0)
  DCC_SIGNAL(progRegs,0)
}

#else      // Configuration for MEGA

ISR(TIMER3_COMPB_vect){              // configura el servicio de interrupción para OCR3B de TIMER-3 que voltea el bit de dirección del Motor Shield Channel B que controla la via de programacion
  DCC_SIGNAL(progRegs,3)
}

#endif


  

///////////////////////////////////////////////////////////////////////////////
//IMPRIME INFORMACION DE CONFIGURACION EN EL PUERTO SERIAL
// - ACTIVADO EN EL INICIO SI SHOW_CONFIG_PIN ES ALTOHIGH 

void showConfiguration(){

  int mac_address[]=MAC_ADDRESS;

  Serial.print("\n*** DCC++ CONFIGURACION ***\n");

  Serial.print("\nVERSION:      ");
  Serial.print(VERSION);
  Serial.print("\nCOMPILED:     ");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.print(__TIME__);

  Serial.print("\nARDUINO:      ");
  Serial.print(ARDUINO_TYPE);

  Serial.print("\n\nMOTOR SHIELD: ");
  Serial.print(MOTOR_SHIELD_NAME);
  
  Serial.print("\n\nDCC SIG MAIN: ");
  Serial.print(DCC_SIGNAL_PIN_MAIN);
  Serial.print("\n   DIRECTION: ");
  Serial.print(DIRECTION_MOTOR_CHANNEL_PIN_A);
  Serial.print("\n      ENABLE: ");
  Serial.print(SIGNAL_ENABLE_PIN_MAIN);
  Serial.print("\n     CURRENT: ");
  Serial.print(CURRENT_MONITOR_PIN_MAIN);

  Serial.print("\n\nDCC SIG PROG: ");
  Serial.print(DCC_SIGNAL_PIN_PROG);
  Serial.print("\n   DIRECTION: ");
  Serial.print(DIRECTION_MOTOR_CHANNEL_PIN_B);
  Serial.print("\n      ENABLE: ");
  Serial.print(SIGNAL_ENABLE_PIN_PROG);
  Serial.print("\n     CURRENT: ");
  Serial.print(CURRENT_MONITOR_PIN_PROG);

  Serial.print("\n\nNUM TURNOUTS: ");
  Serial.print(EEStore::eeStore->data.nTurnouts);
  Serial.print("\n     SENSORS: ");
  Serial.print(EEStore::eeStore->data.nSensors);
  Serial.print("\n     OUTPUTS: ");
  Serial.print(EEStore::eeStore->data.nOutputs);
  
  Serial.print("\n\nINTERFACE:    ");
  #if COMM_TYPE == 0
    Serial.print("SERIAL");
  #elif COMM_TYPE == 1
    Serial.print(COMM_SHIELD_NAME);
    Serial.print("\nMAC ADDRESS:  ");
    for(int i=0;i<5;i++){
      Serial.print(mac_address[i],HEX);
      Serial.print(":");
    }
    Serial.print(mac_address[5],HEX);
    Serial.print("\nPORT:         ");
    Serial.print(ETHERNET_PORT);
    Serial.print("\nIP ADDRESS:   ");

    #ifdef IP_ADDRESS
      Ethernet.begin(mac,IP_ADDRESS);           // Inicia la red con una direccion IP ESTATICA
    #else
      Ethernet.begin(mac);                      // Inicia la red usando DHCP para adquirir una direccion IP
    #endif     
    
    Serial.print(Ethernet.localIP());

    #ifdef IP_ADDRESS
      Serial.print(" (STATIC)");
    #else
      Serial.print(" (DHCP)");
    #endif
  
  #endif
  Serial.print("\n\nPROGRAMA DETENIDO - POR FAVOR REINICIA ARDUINO");

  while(true);
}
