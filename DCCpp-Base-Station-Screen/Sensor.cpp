/**********************************************************************

Sensor.cpp
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/
/**********************************************************************

DCC++ BASE STATION  admite entradas de sensores que se pueden conectar a cualquier Pin Aruidno
que no esté en uso por este programa.  Los sensores pueden ser de cualquier tipo (infrarrojos, mágicos, mecánicos ...).
El único requisito es que cuando se "activa" el sensor debe forzar el Arduino especificado
Pin LOW (es decir, a tierra), y cuando no está activado, este Pin debe permanecer HIGH (por ejemplo, 5V),
o se le permite flotar ALTO si se especifica el uso del resistor interno de Arduino Pin.

Para asegurar niveles de voltaje apropiados, alguna parte del circuito del sensor
DEBE estar amarrado a la misma tierra (-) que usa Arduino.

El código del sensor a continuación utiliza el suavizado exponencial para "deshacer" los picos generados por
Interruptores mecánicos y transistores. Esto evita la necesidad de crear circuitos de suavizado.
para cada sensor. Es posible que deba cambiar estos parámetros mediante prueba y error para sus sensores específicos.


Para que este sketch monitoree uno o más pines Arduino para disparadores de sensores, primero defina / edite / elimine
Definiciones de sensores usando la siguiente variación del comando "S":

  <S ID PIN PULLUP>:           Crea unidentificador de sensor ID, con un PIN esecifico y PULLUP
                               if sensor ID already exists, it is updated with specificed PIN and PULLUP
                               returns: <O> if successful and <X> if unsuccessful (e.g. out of memory)

  <S ID>:                      borra la definición de ID del sensor
                               devuelve: <O> si tiene éxito y <X> si no tiene éxito (por ejemplo, la ID no existe)
  <S>:                         lista todos los sensores definidos
                               devuelve: <Q ID PIN PULLUP> para cada sensor definido o <X> si no hay sensores definidos
  
where

  ID: ID numerica (0-32767) del sensor
  PIN:  numero de pin donde el sensor esta conectado a arduino
  PULLUP: 1=uso de la resistencia interna para el PIN, 0=no usar la resistencia pullup inerna del PIN

Una vez que todos los sensores se hayan definido correctamente, use el comando <E> para almacenar las definiciones en la EEPROM.
Si luego realiza ediciones / adiciones / eliminaciones a las definiciones del sensor, debe invocar el comando <E> si quieres esas
nuevas definiciones sean actualizadas en la EEPROM. También puede borrar todo lo almacenado en la EEPROM invocando el comando <e>.

Todos los sensores definidos de acuerdo con lo anterior se verifican de forma repetida y secuencial dentro del bucle principal del sketch.
Si se encuentra que un Pin del sensor ha pasado de un estado a otro, se genera uno de los siguientes mensajes en serie:

  <Q ID>     - Para la transición de ID de sensor de estado HIGH a estado LOW (i.e. el sensor se dispara)
  <q ID>     - Para la transición de ID de sensor de estado LOW a estado HIGH (i.e. el sensor ya no se dispara)

Dependiendo de si el sensor físico está actuando como un "activador de eventos" o un "sensor de detección", puede
decide ignorar el retorno <q ID> y reaccionar solo a los disparadores de <Q ID>.


**********************************************************************/

#include "DCCpp_Uno.h"
#include "Sensor.h"
#include "EEStore.h"
#include <EEPROM.h>
#include "Comm.h"
#include "Oled.h"

///////////////////////////////////////////////////////////////////////////////
  
void Sensor::check(){    
  Sensor *tt;

  for(tt=firstSensor;tt!=NULL;tt=tt->nextSensor){
    tt->signal=tt->signal*(1.0-SENSOR_DECAY)+digitalRead(tt->data.pin)*SENSOR_DECAY;
    
    if(!tt->active && tt->signal<0.5){
      tt->active=true;
      INTERFACE.print("<Q");
      INTERFACE.print(tt->data.snum);
      INTERFACE.print(">");
    } else if(tt->active && tt->signal>0.9){
      tt->active=false;
      INTERFACE.print("<q");
      INTERFACE.print(tt->data.snum);
      INTERFACE.print(">");
    }
  } // bucle sobre todos los sensores
    
} // Sensor::check

///////////////////////////////////////////////////////////////////////////////

Sensor *Sensor::create(int snum, int pin, int pullUp, int v){
  Sensor *tt;
  
  if(firstSensor==NULL){
    firstSensor=(Sensor *)calloc(1,sizeof(Sensor));
    tt=firstSensor;
  } else if((tt=get(snum))==NULL){
    tt=firstSensor;
    while(tt->nextSensor!=NULL)
      tt=tt->nextSensor;
    tt->nextSensor=(Sensor *)calloc(1,sizeof(Sensor));
    tt=tt->nextSensor;
  }

  if(tt==NULL){       // problema al asignar memoria
    if(v==1)
      INTERFACE.print("<X>");
    return(tt);
  }
  
  tt->data.snum=snum;
  tt->data.pin=pin;
  tt->data.pullUp=(pullUp==0?LOW:HIGH);
  tt->active=false;
  tt->signal=1;
  pinMode(pin,INPUT);         // establecer el modo de entrada
  digitalWrite(pin,pullUp);   // No utilice las resistencias de pull-up internas de Arduino para sensores infrarrojos externos--- 
                              // Cada sensor debe tener su propia resistencia externa de pull-up 1K.
  if(v==1)
    INTERFACE.print("<O>");    
 //   Oled::printSaved(1,true);       //OLED = muestra en pantalla guadado con exito
  return(tt);
  
}

///////////////////////////////////////////////////////////////////////////////

Sensor* Sensor::get(int n){
  Sensor *tt;
  for(tt=firstSensor;tt!=NULL && tt->data.snum!=n;tt=tt->nextSensor);
  return(tt); 
}
///////////////////////////////////////////////////////////////////////////////

void Sensor::remove(int n){
  Sensor *tt,*pp;
  
  for(tt=firstSensor;tt!=NULL && tt->data.snum!=n;pp=tt,tt=tt->nextSensor);

  if(tt==NULL){
    INTERFACE.print("<X>");
    Oled::printDelete(2,false); 
    return;
  }
  
  if(tt==firstSensor)
    firstSensor=tt->nextSensor;
  else
    pp->nextSensor=tt->nextSensor;

  free(tt);

  INTERFACE.print("<O>");
  Oled::printDelete(2, true);
}

///////////////////////////////////////////////////////////////////////////////

void Sensor::show(){
  Sensor *tt;

  if(firstSensor==NULL){
    INTERFACE.print("<X>");
    Oled::printErrorOptionEmpty(2);
    return;
  }
    
  for(tt=firstSensor;tt!=NULL;tt=tt->nextSensor){
    INTERFACE.print("<Q");
    INTERFACE.print(tt->data.snum);
    INTERFACE.print(" ");
    INTERFACE.print(tt->data.pin);
    INTERFACE.print(" ");
    INTERFACE.print(tt->data.pullUp);
    INTERFACE.print(">");
  }
  Oled::printSensor(tt->data.snum, tt->data.pin, tt->data.pullUp);
}

///////////////////////////////////////////////////////////////////////////////

void Sensor::status(){
  Sensor *tt;

  if(firstSensor==NULL){
    INTERFACE.print("<X>");
  Oled::printErrorOptionUndefined(2);

    return;
  }
    
  for(tt=firstSensor;tt!=NULL;tt=tt->nextSensor){
    INTERFACE.print(tt->active?"<Q":"<q");
    INTERFACE.print(tt->data.snum);
    INTERFACE.print(">");
  }
}

///////////////////////////////////////////////////////////////////////////////

void Sensor::parse(char *c){
  int n,s,m;
  Sensor *t;
  
  switch(sscanf(c,"%d %d %d",&n,&s,&m)){
    
    case 3:                     // El argumento es un string con el identificador id del sensor seguido del numero de pin y el indicador pullup (0=LOW/1=HIGH)
      create(n,s,m,1);
      Oled::printDefined(2, n, s, m); // OLED n=ID s=pin m=pullup

    break;

    case 1:                     // El argumento es un string con el numero ID
      remove(n);
    break;
    
    case -1:                    // Sin argumentos
      show();
    break;

    case 2:                     // invalid number of arguments
      INTERFACE.print("<X>");
      Oled::printErrorOptionUndefined(2);

      break;
  }
}

///////////////////////////////////////////////////////////////////////////////

void Sensor::load(){
  struct SensorData data;
  Sensor *tt;

  for(int i=0;i<EEStore::eeStore->data.nSensors;i++){
    EEPROM.get(EEStore::pointer(),data);  
    tt=create(data.snum,data.pin,data.pullUp);
    EEStore::advance(sizeof(tt->data));
  }  
}

///////////////////////////////////////////////////////////////////////////////

void Sensor::store(){
  Sensor *tt;
  
  tt=firstSensor;
  EEStore::eeStore->data.nSensors=0;
  
  while(tt!=NULL){
    EEPROM.put(EEStore::pointer(),tt->data);
    EEStore::advance(sizeof(tt->data));
    tt=tt->nextSensor;
    EEStore::eeStore->data.nSensors++;
  }  
}

///////////////////////////////////////////////////////////////////////////////

Sensor *Sensor::firstSensor=NULL;
