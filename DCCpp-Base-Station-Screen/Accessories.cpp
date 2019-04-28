/**********************************************************************

Accessories.cpp
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/
/**********************************************************************
 * 
DCC++ BASE STATION puede realizar un seguimiento de la dirección de cualquier desvio que este controlado 
por un decodificador de accesorio DCC. Todos los desvíos, así como cualquier otro accesorio DCC 
conectados de esta manera, siempre se pueden utilizar mediante el comando:

  <a DIRECCION SUBDIRECCION ACTIVACION>

Sin embargo,este comando general simplemente envía el paquete de instrucciones DCC adecuado 
a las principales pistas para operar los accesorios conectados. 
No almacena o retiene ninguna información sobre el estado actual de ese accesorio.

Para tener esta tienda boceto y retener la dirección de desvíos conectados-DCC, 
así como invocar automáticamente el comando <a> necesita, según sea necesario, 
primero definir / editar / borrar dichos desvíos usando las siguientes variaciones del comando "T":

<T ID DIRECCION SUBDIRECCION>: Crea una nueva ID de desvio con una DIRECCION y SUBDIRECCION especifica
                               si el ID ya existe, este se actualizara con la DIRECCION y SUBDIRECCION especifica
                               devuelve: <O> si tiene exito y <X> si falla (e.j. fuera de memoria)

  <T ID>:                      Elimina la ID definida del desvio 
                               devuelve : <O> si tiene exito <X> si falla (e.j. ID no existe)

  <T>:                         Lista todos los desvios definidos
                               devuelve : <H ID DIRECCION SUBDIRECCION THROW> para cada desvio definido o <X> si no hay desvios definidos
  
donde:

  ID: ID numerica (0-32767) para los desvios a controlar
  DIRECCION:  la direccion primaria del decoder que controla este desvio (0-511)
  SUBADDRESS: La subdireccion del decoder que controla este desvio (0-3)

Una vez todos los desvíos se han definido adecuadamente, utilice el comando <E> para almacenar sus definiciones en la EEPROM.
Si, posteriormente, realizar ediciones/adiciones/supresiones a las definiciones de participación, debe invocar el comando <E> si desea que las
nuevas definiciones sean actualizadas en la EEPROM. También puede borrar todo lo almacenado en la EEPROM mediante la invocación del comando <e>.

Cambia los desvios que han sido definidos:

  <T ID THROW>:                establece la  posicion recto or desviado del desvio ID
                               devuelve: <H ID THROW>, o <X> si la ID del desvio no existe

donde:

  ID: ID numerica (0-32767) del desvio a controlar 
  THROW: 0 (desviado) o 1 (recto)

Cuando controlada como tales, las actualizaciones de Arduino y almacena la dirección de cada tasa de participación en la EEPROM
de modo que se mantiene incluso sin energía eléctrica. Una lista de las direcciones de las corrientes de cada La participación 
en la forma <H ID THROW> es generada por este esquema cada vez que se invoca el comando <s> Estado. 
Esto proporciona una manera eficiente de inicializar las direcciones de cualquier Desvíos siendo vigilados 
o controlados por un programa de interfaz GUI o por separado.
////////////////////////////////////////////////////////////////////////////////////////////////////
*/

#include "Accessories.h"
#include "SerialCommand.h"
#include "DCCpp_Uno.h"
#include "EEStore.h"
#include <EEPROM.h>
#include "Oled.h"
#include "Comm.h"

///////////////////////////////////////////////////////////////////////////////

void Turnout::activate(int s){
  char c[20];
  data.tStatus=(s>0);                                    // si s>0 cambia el desvio=ENCENDIDO, pero si es cero o negativo cambia el desvio=APAGADO
  sprintf(c,"a %d %d %d",data.address,data.subAddress,data.tStatus);
  SerialCommand::parse(c);
  if(num>0)
    EEPROM.put(num,data.tStatus);
  INTERFACE.print("<H");
  INTERFACE.print(data.id);
  if(data.tStatus==0)
    INTERFACE.print(" 0>");
  else
    INTERFACE.print(" 1>"); 
}

///////////////////////////////////////////////////////////////////////////////

Turnout* Turnout::get(int n){
  Turnout *tt;
  for(tt=firstTurnout;tt!=NULL && tt->data.id!=n;tt=tt->nextTurnout);
  return(tt); 
}
///////////////////////////////////////////////////////////////////////////////

void Turnout::remove(int n){
  Turnout *tt,*pp;
  
  for(tt=firstTurnout;tt!=NULL && tt->data.id!=n;pp=tt,tt=tt->nextTurnout);

  if(tt==NULL){
    INTERFACE.print("<X>");
    Oled::printDelete(1,false);
    return;
  }
  
  if(tt==firstTurnout)
    firstTurnout=tt->nextTurnout;
  else
    pp->nextTurnout=tt->nextTurnout;

  free(tt);

  INTERFACE.print("<O>");
  Oled::printDelete(1,true);
}

///////////////////////////////////////////////////////////////////////////////

void Turnout::show(int n){
  Turnout *tt;

  if(firstTurnout==NULL){
    INTERFACE.print("<X>");
    Oled::printErrorOptionEmpty(1);     //OLED
    return;
  }
    
  for(tt=firstTurnout;tt!=NULL;tt=tt->nextTurnout){
    INTERFACE.print("<H");
    INTERFACE.print(tt->data.id);
    if(n==1){
      INTERFACE.print(" ");
      INTERFACE.print(tt->data.address);
      INTERFACE.print(" ");
      INTERFACE.print(tt->data.subAddress);
    }
    if(tt->data.tStatus==0)
       INTERFACE.print(" 0>");
     else
       INTERFACE.print(" 1>"); 
  }
  Oled::printAccesories(tt->data.address, tt->data.subAddress, tt->data.tStatus);
}

///////////////////////////////////////////////////////////////////////////////

void Turnout::parse(char *c){
  int n,s,m;
  Turnout *t;
  
  switch(sscanf(c,"%d %d %d",&n,&s,&m)){
    
    case 2:                     // argumento es un String con el numero ID seguido de cero (recto) o uno (desviado)
      t=get(n);
      if(t!=NULL)
        t->activate(s);
      else{
        INTERFACE.print("<X>");
        Oled::printErrorOptionUndefined(1);
      }
      break;

    case 3:                     // / argumento es String con el numero ID del desvio seguido de la direccion y subdireccion
      create(n,s,m,1);
          Oled::printDefined(1, n, s, m); // OLED n=ID s=dir m=subdir

    break;

    case 1:                     // argumento es una cadena con el numero ID solo
      remove(n);
    break;
    
    case -1:                    // Sin argumentos
      show(1);                  // verbose show
    break;
  }
 // Oled::printSaved(1, n, s);
}

///////////////////////////////////////////////////////////////////////////////

void Turnout::load(){
  struct TurnoutData data;
  Turnout *tt;

  for(int i=0;i<EEStore::eeStore->data.nTurnouts;i++){
    EEPROM.get(EEStore::pointer(),data);  
    tt=create(data.id,data.address,data.subAddress);
    tt->data.tStatus=data.tStatus;
    tt->num=EEStore::pointer();
    EEStore::advance(sizeof(tt->data));
  }  
}

///////////////////////////////////////////////////////////////////////////////

void Turnout::store(){
  Turnout *tt;
  
  tt=firstTurnout;
  EEStore::eeStore->data.nTurnouts=0;
  
  while(tt!=NULL){
    tt->num=EEStore::pointer();
    EEPROM.put(EEStore::pointer(),tt->data);
    EEStore::advance(sizeof(tt->data));
    tt=tt->nextTurnout;
    EEStore::eeStore->data.nTurnouts++;
  }  
}
///////////////////////////////////////////////////////////////////////////////

Turnout *Turnout::create(int id, int add, int subAdd, int v){
  Turnout *tt;
  
  if(firstTurnout==NULL){
    firstTurnout=(Turnout *)calloc(1,sizeof(Turnout));
    tt=firstTurnout;
  } else if((tt=get(id))==NULL){
    tt=firstTurnout;
    while(tt->nextTurnout!=NULL)
      tt=tt->nextTurnout;
    tt->nextTurnout=(Turnout *)calloc(1,sizeof(Turnout));
    tt=tt->nextTurnout;
  }

  if(tt==NULL){       // problema al asignar memoria
    if(v==1)
      INTERFACE.print("<X>");
       Oled::printSaved(1,false);  //OLED = muentra en pantalla Error al guardar el desvío (ejem: memoria llena)

    return(tt);
  }
  
  tt->data.id=id;
  tt->data.address=add;
  tt->data.subAddress=subAdd;
  tt->data.tStatus=0;
  if(v==1)
    INTERFACE.print("<O>");
  return(tt);
  
}

///////////////////////////////////////////////////////////////////////////////

Turnout *Turnout::firstTurnout=NULL;
