#ifndef MenuGen_h
#define MenuGen_h

// Tp3  manipulation MenuGen avec PEC12
// C. HUBER  03.02.2016
// Fichier MenuGen.h
// Gestion du menu  du générateur
// Traitement cyclique à 1 ms du Pec12


/*#include <stdbool.h>
#include <stdint.h> */
#include "DefMenuGen.h"


typedef enum  { Main, Forme, Freq, Amp, Offset } E_Menu;

void MENU_Initialize(S_ParamGen * pParam);


void MENU_Execute(S_ParamGen * pParam);
void MENU_Print(S_ParamGen * pParam);


void ClearPrefix(void);
void ClearLcd(void);
void Menu_Callback(S_ParamGen * pParam);

int8_t PosCurseur = 1;

#endif













