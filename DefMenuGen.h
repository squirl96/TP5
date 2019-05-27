// Ecole sup�rieur SL229_MINF TP
// Manipulation Tp3_MenuGen
// Cr�� le 9 Mai 2006 CHR
// Version 2016  du 03.02.2016
// Modif 2015 : utilisation de stdint.h
// Modif 2016 : ajout Duty pour PWM
// Modif 2018 SCA : suppression PWM et duty
// Definition pour le menuGen

#ifndef DEFMENUGEN_H
#define DEFMENUGEN_H

#include <stdint.h>

typedef enum  { SignalSinus, SignalTriangle, SignalDentDeScie, SignalCarre } E_FormesSignal;

// Structure des param�tres du g�n�rateur
typedef struct {
  E_FormesSignal Forme;
  int16_t Frequence;
  int16_t Amplitude;
  int16_t Offset;
  uint32_t Magic;
} S_ParamGen;

// En attendant test avec Harmony
// extern const unsigned int  eedata_addr[PAGE_SIZE] __attribute__((aligned(4096), space(prog)));
// #define NVM_PROGRAM_PAGE (&eedata_addr[0])

#endif
