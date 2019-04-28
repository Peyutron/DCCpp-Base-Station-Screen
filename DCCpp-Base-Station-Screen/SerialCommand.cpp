/**********************************************************************

SerialCommand.cpp
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

// DCC++ BASE STATION se comunica a través del puerto serie con comandos de texto de un solo carácter 
// con parametros opcionales y delimitado por símbolos <y>. Los espacios entre parametros             
// son necesarios. Espacios en cualquier otro sitio se ignoran. Un espacio entre el comando           
// de un solo caracter y el primer parametro no se requiere                                           
    
// Ver SerialCommand::parse() debajo de los comandos de texto definido.    
#include "SerialCommand.h"
#include "DCCpp_Uno.h"
#include "Accessories.h"
#include "Sensor.h"
#include "Oled.h"
#include "Outputs.h"
#include "EEStore.h"
#include "Comm.h"

extern int __heap_start, *__brkval;
int Sram;
///////////////////////////////////////////////////////////////////////////////

char SerialCommand::commandString[MAX_COMMAND_LENGTH+1];
volatile RegisterList *SerialCommand::mRegs;
volatile RegisterList *SerialCommand::pRegs;
CurrentMonitor *SerialCommand::mMonitor;

///////////////////////////////////////////////////////////////////////////////

void SerialCommand::init(volatile RegisterList *_mRegs, volatile RegisterList *_pRegs, CurrentMonitor *_mMonitor){
  mRegs=_mRegs;
  pRegs=_pRegs;
  mMonitor=_mMonitor;
  sprintf(commandString,"");
} // SerialCommand:SerialCommand

///////////////////////////////////////////////////////////////////////////////

void SerialCommand::process(){
  char c;
    
  #if COMM_TYPE == 0

    while(INTERFACE.available()>0){    // Mientras hay datos en la linea serial
     c=INTERFACE.read();
     if(c=='<')                    // Comienza un nuevo comando
       sprintf(commandString,"");
     else if(c=='>')               // Fin de un nuevo comando
       parse(commandString);                    
     else if(strlen(commandString)<MAX_COMMAND_LENGTH)    // if comandString still has space, append character just read from serial line
       sprintf(commandString,"%s%c",commandString,c);     // otherwise, character is ignored (but continue to look for '<' or '>')
    } // while
  
  #elif COMM_TYPE == 1

    EthernetClient client=INTERFACE.available();

    if(client){
      while(client.connected() && client.available()){        // while there is data on the network
      c=client.read();
      if(c=='<')                    // start of new command
        sprintf(commandString,"");
      else if(c=='>')               // end of new command
        parse(commandString);                    
      else if(strlen(commandString)<MAX_COMMAND_LENGTH)    // if comandString still has space, append character just read from network
        sprintf(commandString,"%s%c",commandString,c);     // otherwise, character is ignored (but continue to look for '<' or '>')
      } // while
    }

  #endif

} // SerialCommand:process
   
///////////////////////////////////////////////////////////////////////////////

void SerialCommand::parse(char *com){
  
  switch(com[0]){

/***** SET ENGINE THROTTLES USING 128-STEP SPEED CONTROL ****/    

    case 't':       // <t REGISTER CAB SPEED DIRECTION>
/*
 *    establece el acelerador para una combinación de registro/cabina dada
 *    
 *    REGISTRO: Un numero de registro interno, desde 1 hasta MAX_MAIN_REGISTERS (incluido) para almacenar el paquete DCC utilizado para controlar esta posicion del acelerador
 *    CABINA: Direccion corta (1-127) o larga (128-10293) del decodificador de la locomotora
 *    VELOCIDAD: velocidad de aceleración, entre 0-126, o -1 para parada de emergencia (se restablece velocidad a 0) 
 *    DIRECCIÓN: 1 = adelante, 0 = atrás. Ajuste de la dirección cuando la velocidad = 0 = -1 sólo afecta a la direccionalidad de la iluminación de la cabina de un tren detenido
 *    
 *    retornos: <T REGISTRO VELOCIDAD DIRECCION>
 *    
 */
      mRegs->setThrottle(com+1);
      break;

/***** OPERATE ENGINE DECODER FUNCTIONS F0-F28 ****/    

    case 'f':       // <f CAB BYTE1 [BYTE2]>
/*
 *    turns on and off engine decoder functions F0-F28 (F0 is sometimes called FL)  
 *    NOTE: setting requests transmitted directly to mobile engine decoder --- current state of engine functions is not stored by this program
 *    
 *    CAB:  the short (1-127) or long (128-10293) address of the engine decoder
 *    
 *    To set functions F0-F4 on (=1) or off (=0):
 *      
 *    BYTE1:  128 + F1*1 + F2*2 + F3*4 + F4*8 + F0*16
 *    BYTE2:  omitted
 *   
 *    To set functions F5-F8 on (=1) or off (=0):
 *   
 *    BYTE1:  176 + F5*1 + F6*2 + F7*4 + F8*8
 *    BYTE2:  omitted
 *   
 *    To set functions F9-F12 on (=1) or off (=0):
 *   
 *    BYTE1:  160 + F9*1 +F10*2 + F11*4 + F12*8
 *    BYTE2:  omitted
 *   
 *    To set functions F13-F20 on (=1) or off (=0):
 *   
 *    BYTE1: 222 
 *    BYTE2: F13*1 + F14*2 + F15*4 + F16*8 + F17*16 + F18*32 + F19*64 + F20*128
 *   
 *    To set functions F21-F28 on (=1) of off (=0):
 *   
 *    BYTE1: 223
 *    BYTE2: F21*1 + F22*2 + F23*4 + F24*8 + F25*16 + F26*32 + F27*64 + F28*128
 *   
 *    returns: NONE
 * 
 */
      mRegs->setFunction(com+1);
      break;
      
/***** OPERATE STATIONARY ACCESSORY DECODERS  ****/    

    case 'a':       // <a ADDRESS SUBADDRESS ACTIVATE>
/*
  Cambia un decodificador accesorio (estacionario) encendido o apagado  
 *    
 *    ADDRESS: direccion primaria del decoder (0-511)          
 *    SUBADDRESS: La subdireccion del decodificador (0-3)
 *    ACTIVATE: 1=encendido (fijado), 0=apagado (limpio) 
 *    
 *    Note that many decoders and controllers combine the ADDRESS and SUBADDRESS into a single number, N,
 *    from  1 through a max of 2044, where
 *    
 *    N = (ADDRESS - 1) * 4 + SUBADDRESS + 1, for all ADDRESS>0
 *    
 *    OR
 *    
 *    ADDRESS = INT((N - 1) / 4) + 1
 *    SUBADDRESS = (N - 1) % 4
 *    
 *    returns: NONE
 */
      mRegs->setAccessory(com+1);
      break;

/***** CREA/EDITA/BORRA/MUESTA Y OPERA UN DESVIO  ****/    

    case 'T':       // <T ID THROW>
/*
 *   <T ID THROW>:                sets turnout ID to either the "thrown" or "unthrown" position
 *   
 *   ID: the numeric ID (0-32767) of the turnout to control
 *   THROW: 0 (unthrown) or 1 (thrown)
 *   
 *   returns: <H ID THROW> or <X> if turnout ID does not exist
 *   
 *   *** SEE ACCESSORIES.CPP FOR COMPLETE INFO ON THE DIFFERENT VARIATIONS OF THE "T" COMMAND
 *   USED TO CREATE/EDIT/REMOVE/SHOW TURNOUT DEFINITIONS
 */
      Turnout::parse(com+1);
      break;

/***** CREATE/EDIT/REMOVE/SHOW & OPERATE AN OUTPUT PIN  ****/    

    case 'Z':       // <Z ID ACTIVATE>
/*
 *   <Z ID ACTIVATE>:          sets output ID to either the "active" or "inactive" state
 *   
 *   ID: the numeric ID (0-32767) of the output to control
 *   ACTIVATE: 0 (active) or 1 (inactive)
 *   
 *   returns: <Y ID ACTIVATE> or <X> if output ID does not exist
 *   
 *   *** SEE OUTPUTS.CPP FOR COMPLETE INFO ON THE DIFFERENT VARIATIONS OF THE "O" COMMAND
 *   USED TO CREATE/EDIT/REMOVE/SHOW TURNOUT DEFINITIONS
 */
      Output::parse(com+1);
      break;
      
/***** CREA/EDITA/BORRA/MUESTRA UN SENSOR  ****/    

    case 'S': 
/*   
 *   *** SEE SENSOR.CPP FOR COMPLETE INFO ON THE DIFFERENT VARIATIONS OF THE "S" COMMAND
 *   USED TO CREATE/EDIT/REMOVE/SHOW SENSOR DEFINITIONS
 */
      Sensor::parse(com+1);
      break;

/***** MUESTRA EL ESTADO DE TODOS LOS SENSORES ****/

    case 'Q':         // <Q>
/*
 *    returns: the status of each sensor ID in the form <Q ID> (active) or <q ID> (not active)
 */
      Sensor::status();
      break;

/***** WRITE CONFIGURATION VARIABLE BYTE TO ENGINE DECODER ON MAIN OPERATIONS TRACK  ****/    

    case 'w':      // <w CAB CV VALUE>
/*
 *    writes, without any verification, a Configuration Variable to the decoder of an engine on the main operations track
 *    
 *    CAB:  the short (1-127) or long (128-10293) address of the engine decoder 
 *    CV: the number of the Configuration Variable memory location in the decoder to write to (1-1024)
 *    VALUE: the value to be written to the Configuration Variable memory location (0-255)
 *    
 *    returns: NONE
*/    
      mRegs->writeCVByteMain(com+1);
      break;      

/***** WRITE CONFIGURATION VARIABLE BIT TO ENGINE DECODER ON MAIN OPERATIONS TRACK  ****/    

    case 'b':      // <b CAB CV BIT VALUE>
/*
 *    writes, without any verification, a single bit within a Configuration Variable to the decoder of an engine on the main operations track
 *    
 *    CAB:  the short (1-127) or long (128-10293) address of the engine decoder 
 *    CV: the number of the Configuration Variable memory location in the decoder to write to (1-1024)
 *    BIT: the bit number of the Configurarion Variable regsiter to write (0-7)
 *    VALUE: the value of the bit to be written (0-1)
 *    
 *    returns: NONE
*/        
      mRegs->writeCVBitMain(com+1);
      break;      

/***** WRITE CONFIGURATION VARIABLE BYTE TO ENGINE DECODER ON PROGRAMMING TRACK  ****/    

    case 'W':      // <W CV VALUE CALLBACKNUM CALLBACKSUB>
/*
 *    writes, and then verifies, a Configuration Variable to the decoder of an engine on the programming track
 *    
 *    CV: the number of the Configuration Variable memory location in the decoder to write to (1-1024)
 *    VALUE: the value to be written to the Configuration Variable memory location (0-255) 
 *    CALLBACKNUM: an arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs that call this function
 *    CALLBACKSUB: a second arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs (e.g. DCC++ Interface) that call this function
 *    
 *    returns: <r CALLBACKNUM|CALLBACKSUB|CV Value)
 *    where VALUE is a number from 0-255 as read from the requested CV, or -1 if verificaiton read fails
*/    
      pRegs->writeCVByte(com+1);
      break;      

/***** WRITE CONFIGURATION VARIABLE BIT TO ENGINE DECODER ON PROGRAMMING TRACK  ****/    

    case 'B':      // <B CV BIT VALUE CALLBACKNUM CALLBACKSUB>
/*
 *    writes, and then verifies, a single bit within a Configuration Variable to the decoder of an engine on the programming track
 *    
 *    CV: the number of the Configuration Variable memory location in the decoder to write to (1-1024)
 *    BIT: the bit number of the Configurarion Variable memory location to write (0-7)
 *    VALUE: the value of the bit to be written (0-1)
 *    CALLBACKNUM: an arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs that call this function
 *    CALLBACKSUB: a second arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs (e.g. DCC++ Interface) that call this function
 *    
 *    returns: <r CALLBACKNUM|CALLBACKSUB|CV BIT VALUE)
 *    where VALUE is a number from 0-1 as read from the requested CV bit, or -1 if verificaiton read fails
*/    
      pRegs->writeCVBit(com+1);
      break;      

/***** READ CONFIGURATION VARIABLE BYTE FROM ENGINE DECODER ON PROGRAMMING TRACK  ****/    

    case 'R':     // <R CV CALLBACKNUM CALLBACKSUB>
/*    
 *    reads a Configuration Variable from the decoder of an engine on the programming track
 *    
 *    CV: the number of the Configuration Variable memory location in the decoder to read from (1-1024)
 *    CALLBACKNUM: an arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs that call this function
 *    CALLBACKSUB: a second arbitrary integer (0-32767) that is ignored by the Base Station and is simply echoed back in the output - useful for external programs (e.g. DCC++ Interface) that call this function
 *    
 *    returns: <r CALLBACKNUM|CALLBACKSUB|CV VALUE)
 *    where VALUE is a number from 0-255 as read from the requested CV, or -1 if read could not be verified
*/    
      pRegs->readCV(com+1);
      break;

/***** TURN ON POWER FROM MOTOR SHIELD TO TRACKS  ****/    

    case '1':      // <1>
/*   
*    activa la alimentacion del motor shield de la via principal y la via de programacion
 *    
 *    devuelve: <p1>
 */    
     digitalWrite(SIGNAL_ENABLE_PIN_PROG,HIGH);
     digitalWrite(SIGNAL_ENABLE_PIN_MAIN,HIGH);
     INTERFACE.print("<p1>");
     Oled::showOledOn(true);    //OLED
     break;
          
/***** APAGA LA ALIMENTACION DEL MOTOR HIELD A LAS PISTAS  ****/    

    case '0':     // <0>
/*   
  *    desactiva la alimentacion del motor shield de la via principal y la via de programacion
 *    
 *    devuelve: <p0>
 */
     digitalWrite(SIGNAL_ENABLE_PIN_PROG,LOW);
     digitalWrite(SIGNAL_ENABLE_PIN_MAIN,LOW);
     INTERFACE.print("<p0>");
     Oled::showOledOn(false);   //OLED 
     break;

/***** LECTURA DE LA CORRIENTE EN LA VIA PRINCIPAL  ****/    

    case 'c':     // <c>
/*
 *    reads current being drawn on main operations track
 *    
 *    devuelve: <a CURRENT> 
 *    where CURRENT = 0-1024, based on exponentially-smoothed weighting scheme
 */
      INTERFACE.print("<a");
      INTERFACE.print(int(mMonitor->current));
      INTERFACE.print(">");
      break;

/***** LECTURA DEL ESTADO DE DCC++ BASE STATION  ****/ 

    case 's':      // <s>
/*
 *    returns status messages containing track power status, throttle status, turn-out status, and a version number
 *    NOTE: this is very useful as a first command for an interface to send to this sketch in order to verify connectivity and update any GUI to reflect actual throttle and turn-out settings
 *    
 *    returns: series of status messages that can be read by an interface to determine status of DCC++ Base Station and important settings
 */
      if(digitalRead(SIGNAL_ENABLE_PIN_PROG)==LOW)      // could check either PROG or MAIN
        INTERFACE.print("<p0>");
      else
        INTERFACE.print("<p1>");

      for(int i=1;i<=MAX_MAIN_REGISTERS;i++){
        if(mRegs->speedTable[i]==0)
          continue;
        INTERFACE.print("<T");
        INTERFACE.print(i); INTERFACE.print(" ");
        if(mRegs->speedTable[i]>0){
          INTERFACE.print(mRegs->speedTable[i]);
          INTERFACE.print(" 1>");
        } else{
          INTERFACE.print(-mRegs->speedTable[i]);
          INTERFACE.print(" 0>");
        }          
      }
      INTERFACE.print("<iDCC++ BASE STATION V-");
      INTERFACE.print(VERSION);
      INTERFACE.print(" PARA ARDUINO:");
      INTERFACE.print(ARDUINO_TYPE);
      INTERFACE.print(" / ");
      INTERFACE.print(MOTOR_SHIELD_NAME);

//      INTERFACE.print(" / ");
      INTERFACE.print(__DATE__);
      INTERFACE.print(" ");
      INTERFACE.print(__TIME__);
      INTERFACE.print(">");

      INTERFACE.print("<N");
      INTERFACE.print(COMM_TYPE);
      INTERFACE.print(": ");

      #if COMM_TYPE == 0
        INTERFACE.print("SERIAL>");
      #elif COMM_TYPE == 1
        INTERFACE.print(Ethernet.localIP());
        INTERFACE.print(">");
      #endif
      
      Turnout::show();
      Output::show();
                        
      break;

/***** ALMACENA CONFIGURACION EN EEPROM  ****/    
    case 'E':     // <E>
/*
 *    stores settings for turnouts and sensors EEPROM
 *    
 *    returns: <e nTurnouts nSensors>
*/
     
    EEStore::store();
    INTERFACE.print("<e ");
    INTERFACE.print(EEStore::eeStore->data.nTurnouts);
    INTERFACE.print(" ");
    INTERFACE.print(EEStore::eeStore->data.nSensors);
    INTERFACE.print(" ");
    INTERFACE.print(EEStore::eeStore->data.nOutputs);
    INTERFACE.print(">");
    break;
    
/***** BORRA LA CONFIGURACION DE LA EEPROM  ****/    

    case 'e':     // <e>
/*
 *    clears settings for Turnouts in EEPROM
 *    
 *    devuelve: <O>
*/
     
    EEStore::clear();
    INTERFACE.print("<O>");
    break;

/***** PRINT CARRIAGE RETURN IN SERIAL MONITOR WINDOW  ****/    
                
    case ' ':     // < >                
/*
 *    simply prints a carriage return - useful when interacting with Ardiuno through serial monitor window
 *    
 *    returns: a carriage return
*/
      INTERFACE.print("");
      break;  

///          
///LOS SIGUIENTES COMANDOS NO SON NECESARIOS PARA UN FUNCIONAMIENTO NORMAL, USAR SOLO PARA TEST Y DEPURACION
///POR FAVOR VEA LAS ADVERTENCIAS DE CADA COMANDO
///

/***** ENTRAR EN MODO DE DIAGNOSTICO ****/    

    case 'D':       // <D>  
/*
 *    changes the clock speed of the chip and the pre-scaler for the timers so that you can visually see the DCC signals flickering with an LED
 *    SERIAL COMMUNICAITON WILL BE INTERUPTED ONCE THIS COMMAND IS ISSUED - MUST RESET BOARD OR RE-OPEN SERIAL WINDOW TO RE-ESTABLISH COMMS
 */

    Serial.print("\nEntrando en modo Diagnostico...");
    delay(1000);
    
    bitClear(TCCR1B,CS12);    // set Timer 1 prescale=8 - SLOWS NORMAL SPEED BY FACTOR OF 8
    bitSet(TCCR1B,CS11);
    bitClear(TCCR1B,CS10);

    #ifdef ARDUINO_AVR_UNO      // Configuracion para UNO

      bitSet(TCCR0B,CS02);    // set Timer 0 prescale=256 - SLOWS NORMAL SPEED BY A FACTOR OF 4
      bitClear(TCCR0B,CS01);
      bitClear(TCCR0B,CS00);
      
    #else                     // Configuracion para MEGA

      bitClear(TCCR3B,CS32);    // set Timer 3 prescale=8 - SLOWS NORMAL SPEED BY A FACTOR OF 8
      bitSet(TCCR3B,CS31);
      bitClear(TCCR3B,CS30);

    #endif

    CLKPR=0x80;           // THIS SLOWS DOWN SYSYEM CLOCK BY FACTOR OF 256
    CLKPR=0x08;           // BOARD MUST BE RESET TO RESUME NORMAL OPERATIONS

    break;

/***** WRITE A DCC PACKET TO ONE OF THE REGSITERS DRIVING THE MAIN OPERATIONS TRACK  ****/    
      
    case 'M':       // <M REGISTER BYTE1 BYTE2 [BYTE3] [BYTE4] [BYTE5]>
/*
 *   writes a DCC packet of two, three, four, or five hexidecimal bytes to a register driving the main operations track
 *   FOR DEBUGGING AND TESTING PURPOSES ONLY.  DO NOT USE UNLESS YOU KNOW HOW TO CONSTRUCT NMRA DCC PACKETS - YOU CAN INADVERTENTLY RE-PROGRAM YOUR ENGINE DECODER
 *   
 *    REGISTER: an internal register number, from 0 through MAX_MAIN_REGISTERS (inclusive), to write (if REGISTER=0) or write and store (if REGISTER>0) the packet 
 *    BYTE1:  first hexidecimal byte in the packet
 *    BYTE2:  second hexidecimal byte in the packet
 *    BYTE3:  optional third hexidecimal byte in the packet
 *    BYTE4:  optional fourth hexidecimal byte in the packet
 *    BYTE5:  optional fifth hexidecimal byte in the packet
 *   
 *    returns: NONE   
 */
      mRegs->writeTextPacket(com+1);
      break;

/***** WRITE A DCC PACKET TO ONE OF THE REGSITERS DRIVING THE MAIN OPERATIONS TRACK  ****/    

    case 'P':       // <P REGISTER BYTE1 BYTE2 [BYTE3] [BYTE4] [BYTE5]>
/*
 *   writes a DCC packet of two, three, four, or five hexidecimal bytes to a register driving the programming track
 *   FOR DEBUGGING AND TESTING PURPOSES ONLY.  DO NOT USE UNLESS YOU KNOW HOW TO CONSTRUCT NMRA DCC PACKETS - YOU CAN INADVERTENTLY RE-PROGRAM YOUR ENGINE DECODER
 *   
 *    REGISTER: an internal register number, from 0 through MAX_MAIN_REGISTERS (inclusive), to write (if REGISTER=0) or write and store (if REGISTER>0) the packet 
 *    BYTE1:  first hexidecimal byte in the packet
 *    BYTE2:  second hexidecimal byte in the packet
 *    BYTE3:  optional third hexidecimal byte in the packet
 *    BYTE4:  optional fourth hexidecimal byte in the packet
 *    BYTE5:  optional fifth hexidecimal byte in the packet
 *   
 *    returns: NONE   
 */
      pRegs->writeTextPacket(com+1);
      break;
            
/***** INTENTOS PARA DETERMINAR CUANTA SRAM LIBRE ESTA DISPONIBLE EN ARDUINO  ****/                        
      
    case 'F':     // <F>
/*
 *     Mide la cantidad de memoria SRAM libre que queda en el Arduino basada en el truco encontrado en Internet.
 *     Útil al configurar tamaños de matriz dinámica, considerando que Uno solo tiene 2048 bytes de SRAM dinámica.
 *     Desafortunadamente no es muy confiable --- sería genial encontrar un método mejor
 *     
 *     returns: <f MEM>
 *     where MEM is the number of free bytes remaining in the Arduino's SRAM
 */
      int v; 
      INTERFACE.print("<f");
      Sram = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
      INTERFACE.print((int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));
      INTERFACE.print(">");
      Oled::printSram(Sram);
      break;

/***** LISTS BIT CONTENTS OF ALL INTERNAL DCC PACKET REGISTERS  ****/        

    case 'L':     // <L>
/*
 *    lists the packet contents of the main operations track registers and the programming track registers
 *    FOR DIAGNOSTIC AND TESTING USE ONLY
 */
      INTERFACE.print("");
      for(Register *p=mRegs->reg;p<=mRegs->maxLoadedReg;p++){
        INTERFACE.print("M"); INTERFACE.print((int)(p-mRegs->reg)); INTERFACE.print(":\t");
        INTERFACE.print((int)p); INTERFACE.print("\t");
        INTERFACE.print((int)p->activePacket); INTERFACE.print("\t");
        INTERFACE.print(p->activePacket->nBits); INTERFACE.print("\t");
        for(int i=0;i<10;i++){
          INTERFACE.print(p->activePacket->buf[i],HEX); INTERFACE.print("\t");
        }
        INTERFACE.print("");
      }
      for(Register *p=pRegs->reg;p<=pRegs->maxLoadedReg;p++){
        INTERFACE.print("P"); INTERFACE.print((int)(p-pRegs->reg)); INTERFACE.print(":\t");
        INTERFACE.print((int)p); INTERFACE.print("\t");
        INTERFACE.print((int)p->activePacket); INTERFACE.print("\t");
        INTERFACE.print(p->activePacket->nBits); INTERFACE.print("\t");
        for(int i=0;i<10;i++){
          INTERFACE.print(p->activePacket->buf[i],HEX); INTERFACE.print("\t");
        }
        INTERFACE.print("");
      }
      INTERFACE.print("");
      break;

  } // switch
}; // SerialCommand::parse

///////////////////////////////////////////////////////////////////////////////
