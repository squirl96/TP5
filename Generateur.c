// Canevas manipulation GenSig avec menu
//
// Fichier Generateur.C
// Gestion  du générateur

// Prévu pour signal de 40 echantillons

#include "Generateur.h"
#include "DefMenuGen.h"
#include "Mc32gestSpiDac.h"
#include "Mc32NVMUtil.h"

// T.P. 2019 100 echantillons
#define MAX_ECH 100
#define ZERO 0
#define MAGIC 0x12345678u
// double permet de promovoir le type en double afin de maitriser le type

//3.2767 est egale au pas le plus petit
double const mV_DAC = ((32767.0 / 10000.0));
// le tableau Tech est le tableau dans lequel on a pu memoriser toutes les
// valeurs dans notre cas les valeurs sont precalculee
uint16_t Tech[MAX_ECH];


// Initialisation du  generateur
void  GENSIG_Initialize(S_ParamGen * pParam)
{
    NVM_ReadBlock((uint32_t *) pParam, sizeof(S_ParamGen));
    if(pParam->Magic != MAGIC)
    {
        //valeurs par defaut
        pParam->Forme = SignalCarre;
        pParam->Frequence = 100;
        pParam->Amplitude = 1000;
        pParam->Offset = 0;
        pParam->Magic = MAGIC;
    } 
}

// Mise a jour du signal (forme, amplitude, offset)
void  GENSIG_UpdateSignal(S_ParamGen * pParam)
{
  GENSIG_UpdatePeriode(pParam);
  switch (pParam->Forme) {
    case SignalSinus:
      Sinus(pParam);
      break;

    case SignalTriangle:
      Triangle(pParam);
      break;

    case SignalDentDeScie:
      DentDeScie(pParam);
      break;

    case SignalCarre:
      Carre(pParam);
      break;

    default:
      break;
  }

}

void  Sinus(S_ParamGen * pParam)
{
  uint32_t i = 0 ;
  int32_t vInstant = 0;

  // Boucle qui se repete le nombre de fois d'echantillons
  for (i = 0 ; i < MAX_ECH; i++) {
    // On part de 0 ( donc 32767 qui correspond donc millieu de la rang),
    // on ajoute (inverse) l'offset et on ajoute
    // les pas d'amplitude en fonction de la forme d'un sinus
    vInstant = (32767 - (mV_DAC * pParam->Offset) - 
            (mV_DAC * pParam->Amplitude * (sin((2 * M_PI * i) / MAX_ECH)))) ;

    // Test si la valeur est dans l'echelle voulue (ecretage)
    if (vInstant > 65535) {
      Tech[i] = 65535;
    }
    else if (vInstant < ZERO) {
      Tech[i] = ZERO;
    }
    // Sinon place la valeur dans le tableau
    else {
      Tech[i] = vInstant;
    }
  }
}

void  Triangle(S_ParamGen * pParam)
{
  // Pour le triangle, nous le traitons par 4 etapes
  // 1. partir de 0 et monter
  // 2. descendre jusqu'à 0
  // 3. descendre jusqu'en en bas
  // 4. Remonter jusqu'à 0

  // Commencement a 0 donc au milieux de la rempe
  int32_t  vInstant = 32767;
  int8_t i = 0;
  double step;
  step = (mV_DAC * pParam->Amplitude)  / (MAX_ECH / 4);
  // Boucle qui se repete le nombre de fois d'echantillons
  for (i = 0 ; i < MAX_ECH; i++) {
    // Pour les deux montees du triangle (B1)
    if (i < (MAX_ECH / 4) || i >= ((MAX_ECH / 4) * 3)) {
      // Ajoute l'amplitude par pas proportionnels des 4 etapes  (1 et 4)
      vInstant = vInstant - step ;
    }
    // Pour la descente du triangle (B2)
    else {
      // Enleve l'amplitude par pas proportionnels des 4 etapes
      vInstant = vInstant + step;
    }

    // Test si on est dans les limites
    if (vInstant - (mV_DAC * pParam->Offset) > 65535) {
      Tech[i] = 65535;
    }
    else if (vInstant - (mV_DAC * pParam->Offset) < ZERO) {
      Tech[i] = ZERO;
    }
    // Sinon place la valeur dans le tableau
    else {
      Tech[i] = (vInstant - (mV_DAC * pParam->Offset)) ;
    }
  }
}

void  Carre(S_ParamGen * pParam)
{
  int i ;
  int32_t ValeurH;
  int32_t ValeurL;
  int32_t vInstant;

  // Calcul des amplitudes maximum et minimum du carre
  ValeurH = 65535 - (32767 - (mV_DAC * pParam->Amplitude))
            - (mV_DAC * pParam->Offset);
  ValeurL = 0 + (32767 - (mV_DAC * pParam->Amplitude))
            - (mV_DAC * pParam->Offset);

  // Boucle qui se repete le nombre de fois d'echantillons
  for (i = 0 ; i < MAX_ECH; i++) {
    // S'il est dans la premiere etape
    if (i < (MAX_ECH / 2)) {
      vInstant = ValeurH;

      // Test de l'ecretage
      if (vInstant > 65535) {
        vInstant = 65535;
      }
    }
    // Si il est dans la deuxieme etape
    else {
      vInstant = ValeurL;

      // Test de l'ecretage
      if (vInstant < ZERO) {
        vInstant = ZERO;
      }
    }
    // place la valeur dans le tableau
    Tech[i] = vInstant;
  }
}

void  DentDeScie(S_ParamGen * pParam)
{
  // Commencement a 0
  int32_t vInstant = 32767;
  int32_t i;

  // Boucle qui se repete le nombre de fois d'echantillons
  for (i = 0 ; i < MAX_ECH; i++) {

    vInstant = (32767 - (mV_DAC * pParam->Amplitude)) + 
            (i * ((double)(2 * mV_DAC * pParam->Amplitude) / (MAX_ECH - 1)));

    // Test si la valeur avec l'offset est dans l'echelle voulue (ecretage)
    if (vInstant - (mV_DAC * pParam->Offset) > 65535) {
      Tech[i] = 65535;
    }
    else if (vInstant + (mV_DAC * pParam->Offset) < 0) {
      Tech[i] = 0;
    }
    // Sinon place la valeur dans le tableau
    else {
      Tech[i] = vInstant - (mV_DAC * pParam->Offset);
    }
  }
}
// Execution du generateur
// Fonction appelee dans Int timer3 (cycle variable variable)

// Version provisoire pour test du DAC a modifier
void  GENSIG_Execute(void)
{
  static uint16_t EchNb = 0;

  SPI_WriteToDac(0, Tech[EchNb]);    // le canal 0 prende la valeur  Tech[EchNb]
  // cree une boulce qui va de 0 a 100 mais grace a l'operation modulo le code 
  // et plus court
  EchNb++;
  EchNb = EchNb % MAX_ECH;
}

void  GENSIG_UpdatePeriode(S_ParamGen * pParam)
{
  // Varible de recharge
  uint32_t Timer3Reload;

  // Frequence du CPU / frequence de notre signal fois 100 echantillons
  Timer3Reload = (80000000 / (pParam->Frequence * MAX_ECH)) - 1u;

  // Recharge de la nouvelle periode du timer
  DRV_TMR1_PeriodValueSet(Timer3Reload);
}
