// GesPec12.c  Canevas pour réalisation
// C. HUBER    09/02/2015

// Fonctions pour la gestion du Pec12
//
//
// Principe : Il est nécessaire d'appeler cycliquement la fonction ScanPec12
//            avec un cycle de 1 ms
//
//  Pour la gestion du Pec12, il y a 9 fonctions à disposition :
//       Pec12IsPlus       true indique un nouveau incrément
//       Pec12IsMinus      true indique un nouveau décrément
//       Pec12IsOK         true indique action OK
//       Pec12IsESC        true indique action ESC
//       Pec12NoActivity   true indique abscence d'activité sur PEC12
//  Fonctions pour quittance des indications
//       Pec12ClearPlus    annule indication d'incrément
//       Pec12ClearMinus   annule indication de décrément
//       Pec12ClearOK      annule indication action OK
//       Pec12ClearESC     annule indication action ESC
//
//
//---------------------------------------------------------------------------


// définitions des types qui seront utilisés dans cette application

#include "GesPec12.h"
#include "Mc32Debounce.h"

//********************************************************
#define VALMAXCOUNT 500
// Descripteur des sinaux
S_SwitchDescriptor DescrA;
S_SwitchDescriptor DescrB;
S_SwitchDescriptor DescrPB;

// Structure pour les traitement du Pec12
S_Pec12_Descriptor Pec12;


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Principe utilisation des fonctions
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//
// ScanPec12 (bool ValA, bool ValB, bool ValPB)
//              Routine effectuant la gestion du Pec12
//              recoit la valeur des signaux et du boutons
//
// s'appuie sur le descripteur global.
// Apres l'appel le descripteur est mis a jour

// Comportement du PEC12
// =====================

// Attention 1 cran genere une pulse complete (les 4 combinaisons)
// D'ou traitement uniquement au flanc descendand de B

// Dans le sens horaire CW:
//     __________                      ________________
// B:            |____________________|
//     ___________________                       _________
// A:                     |_____________________|

// Dans le sens anti-horaire CCW:
//     ____________________                      _________
// B:                      |____________________|
//     __________                       __________________
// A:            |_____________________|

void ScanPec12(bool ValA, bool ValB, bool ValPB)
{
  // Traitement antirebond sur A, B et PB
  DoDebounce(&DescrA, ValA);
  DoDebounce(&DescrB, ValB);
  DoDebounce(&DescrPB, ValPB);

  // Detection increment / decrement
  if (DescrB.bits.KeyPressed == 1) {
    DescrB.bits.KeyPressed = 0;
    if (DescrA.bits.KeyValue == 0) { //CW
      Pec12.Inc = 1;
    }
    else { //CCW
      Pec12.Dec = 1;
    }
  }

  if (DebounceGetInput(&DescrPB) == 0) {
    Pec12.PressDuration++;
  }
  if (DebounceIsReleased(&DescrPB)) {
    DebounceClearReleased(&DescrPB);  // quittance
    if (Pec12.PressDuration >= VALMAXCOUNT) {
      Pec12.ESC = 1;
    }
    else {
      Pec12.OK = 1;
    }
    Pec12.PressDuration = 0;
  }

  // Gestion inactivite
  if ((Pec12.Inc == 1) || (Pec12.Dec == 1) || (Pec12.OK == 1) || (Pec12.ESC == 1)) {
    Pec12.InactivityDuration = 0;
    Pec12.NoActivity = 0;
  }
  else {
    Pec12.InactivityDuration++;
  }
  if (Pec12.InactivityDuration >= 5000) {
    Pec12.NoActivity = 1;
  }
} // ScanPec12

void Pec12Init(void)
{
  // Initialisation des descripteurs de touches Pec12
  DebounceInit(&DescrA);
  DebounceInit(&DescrB);
  DebounceInit(&DescrPB);

  // Init de la structure PEc12
  Pec12.Inc = 0;             // evenement increment
  Pec12.Dec = 0;             // evenement decrement
  Pec12.OK = 0;              // evenement action OK
  Pec12.ESC = 0;             // evenement action ESC
  Pec12.NoActivity = 0;      // Indication d'activite
  Pec12.PressDuration = 0;   // Pour duree pression du P.B.
  Pec12.InactivityDuration = 0; // Duree inactivite

} // Pec12Init

//       Pec12IsPlus       true indique un nouveau increment
bool Pec12IsPlus(void)
{
  return (Pec12.Inc);
}

//       Pec12IsMinus      true indique un nouveau decrement
bool Pec12IsMinus(void)
{
  return (Pec12.Dec);
}

//       Pec12IsOK         true indique action OK
bool Pec12IsOK(void)
{
  return (Pec12.OK);
}

//       Pec12IsESC        true indique action ESC
bool Pec12IsESC(void)
{
  return (Pec12.ESC);
}

//       Pec12NoActivity   true indique abscence d'activite sur PEC12
bool Pec12NoActivity(void)
{
  return (Pec12.NoActivity);
}

//  Fonctions pour quittance des indications
//       Pec12ClearPlus    annule indication d'increment
void Pec12ClearPlus(void)
{
  Pec12.Inc = 0;
}

//       Pec12ClearMinus   annule indication de decrement
void Pec12ClearMinus(void)
{
  Pec12.Dec = 0;
}

//       Pec12ClearOK      annule indication action OK
void Pec12ClearOK(void)
{
  Pec12.OK = 0;
}

//       Pec12ClearESC     annule indication action ESC
void Pec12ClearESC(void)
{
  Pec12.ESC = 0;
}

void Pec12ClearInactivity(void)
{
  Pec12.NoActivity = 0;
  Pec12.InactivityDuration = 0;
}
