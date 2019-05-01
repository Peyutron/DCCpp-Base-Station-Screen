#ifndef _OLED_H_
#define _OLED_H_

#include "Config.h"


class Oled {
  public:
    static void init();
    static void clear();
    static void initScreen();
    static void showOledOn(bool);
    static void printThrottle(int, int, int );
    static void printAccesories(int, int, int);
    static void printSensor(int, int, int);
    static void printOutput(int, int, int);     
    static void printSram (int);                  //Muestra en pantalla la cantidad de Sram disponible

    static void printDefined(int, int, int, int); //Imprime en la pantalla que se ha definido un desvío
    static void printSaved(int, bool);            //Imprime en la pantalla que se ha Guardado un desvío /Identificador/id/direccion/subdireccion
    static void printDelete(int, bool);           //Imprime en pantalla que se ha eliminado un desvío /Identificador
    static void printDeleteMemory();              //Aviso borrado de memoria
    static void printErrorOptionEmpty(int);       //ERROR id no disponible
    static void printErrorOptionUndefined(int);   //ERROR no definido
    static void Presentacion();
    static void updateOled();                     //Actualiza la patalla desde loop();
    static void pantalla1();
    static void pantalla2();
    static void pantalla3();
    static void pantalla4();

  private:
    static bool _enabled;
    static void startScreen();
};
#endif
