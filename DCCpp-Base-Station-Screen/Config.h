/**********************************************************************

Config.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

/////////////////////////////////////////////////////////////////////////////////////
//
// DEFINE MOTOR_SHIELD_TYPE DE ACUERDO CON LA SIGUIENTE TABLA:
//
//  0 = ARDUINO MOTOR SHIELD          (MAX 18V/2A PER CHANNEL)
//  1 = POLOLU MC33926 MOTOR SHIELD   (MAX 28V/3A PER CHANNEL)

#define MOTOR_SHIELD_TYPE   0

/////////////////////////////////////////////////////////////////////////////////////
//
// DEFINE EL NUMERO DE REGISTRO DE VIAS PRINCIPALES 

#define MAX_MAIN_REGISTERS 12

/////////////////////////////////////////////////////////////////////////////////////
//
// DEFINE EL TIPO DE INTERFAZ DE COMUNICACIONES
//
//  0 = Puerto serie incorporado
//  1 = Arduino.cc Ethernet/SD-Card Shield
//  2 = Arduino.org Ethernet/SD-Card Shield
//  3 = Seeed Studio Ethernet/SD-Card Shield W5200

#define COMM_INTERFACE   0
#define COMM_NAME "SERIAL/BT"

/////////////////////////////////////////////////////////////////////////////////////
//
//  DEFINE LA DIRECCION IP STATICA O COMENTA PARA USAR DHCP
//

//#define IP_ADDRESS { 192, 168, 1, 200 }

/////////////////////////////////////////////////////////////////////////////////////
//
// DEFINE EL PUERTO QUE USA LA INTERFAZ DE COMUNCACION ETHERNET
//

#define ETHERNET_PORT 2560

/////////////////////////////////////////////////////////////////////////////////////
//
// DEFINE LA DIRECCION MAC PARA LA INTERFAZ DE COMUNICACIONES ETHERNET
//

#define MAC_ADDRESS {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF }

/////////////////////////////////////////////////////////////////////////////////////
//
// DEFINE LA DIRECCION DEL LA PANTALLA OLED SSD1306

#define OLED_ADDRESS 0x3C
#define OLED_NAME "SDD1306"
