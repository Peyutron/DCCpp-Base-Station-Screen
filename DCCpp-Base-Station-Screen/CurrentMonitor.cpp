/**********************************************************************

CurrentMonitor.cpp
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#include "DCCpp_Uno.h"
#include "CurrentMonitor.h"
#include "Comm.h"

///////////////////////////////////////////////////////////////////////////////

CurrentMonitor::CurrentMonitor(int pin, char *msg){
    this->pin=pin;
    this->msg=msg;
    current=0;
  } // CurrentMonitor::CurrentMonitor
  
boolean CurrentMonitor::checkTime(){
  if(millis()-sampleTime<CURRENT_SAMPLE_TIME)            // no hay necesidad de comprobar todavía
    return(false);
  sampleTime=millis();                                   // note millis() uses TIMER-0.  For UNO, we change the scale on Timer-0.  For MEGA we do not.  This means CURENT_SAMPLE_TIME is different for UNO then MEGA
  return(true);  
} // CurrentMonitor::checkTime
  
void CurrentMonitor::check(){
  current=analogRead(pin)*CURRENT_SAMPLE_SMOOTHING+current*(1.0-CURRENT_SAMPLE_SMOOTHING);        // compute new exponentially-smoothed current
  if(current>CURRENT_SAMPLE_MAX && digitalRead(SIGNAL_ENABLE_PIN_PROG)==HIGH){                    // la sobrecarga actual y la señal de progreso están activadas (o podrían haber verificado la señal principal, ya que ambas siempre se encienden o apagans juntas)
    digitalWrite(SIGNAL_ENABLE_PIN_PROG,LOW);                                                     // deshabilitar ambos canales del Motor Shield
    digitalWrite(SIGNAL_ENABLE_PIN_MAIN,LOW);                                                     // independientemente del canal que causó la sobrecarga
    INTERFACE.print(msg);                                                                         // imprimir el mensaje de error correspondiente
  }    
} // CurrentMonitor::check  

long int CurrentMonitor::sampleTime=0;
