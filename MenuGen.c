// Tp3  manipulation MenuGen avec PEC12
// C. HUBER  10/02/2015 pour SLO2 2014-2015
// Fichier MenuGen.c
// Gestion du menu  du générateur
// Traitement cyclique à 10 ms



#include <stdint.h>
#include <stdbool.h>
#include "MenuGen.h"
#include "DefMenuGen.h"
#include "Mc32Debounce.h"
#include "GesPec12.h"
#include "C:\microchip\harmony\v2_05_01\bsp\pic32mx_skes\Mc32DriverLcd.h"
#include "app.h"
#include "C:\microchip\harmony\v2_05_01\apps\MINF\TP\TP3_Gen\firmware\src\Mc32NVMUtil.h"

#define VALMAXCOUNT 200u

/******************************************************************************/
/*                           Function Menu_Initialize                         */
/******************************************************************************/
/* This function prints the menu for the first time.                          */
/* Input parameters: S_ParamGen -> menu's values                              */
/* Output parameters: none                                                    */
/******************************************************************************/
void MENU_Initialize(S_ParamGen * pParam)
{  

  // Initialize
  NVM_ReadBlock((uint32_t *)pParam, sizeof(*pParam));
  if (pParam->Magic != MAGIC) {
    pParam->Amplitude = 2000;
    pParam->Forme = SignalSinus;
    pParam->Frequence = 20;
    pParam->Magic = MAGIC;
    pParam->Offset = 1000;
}
  lcd_gotoxy(1, 1);                     // Go to the first line
  switch (pParam->Forme) {              // Prints a text depending of the
    case SignalSinus :                  // form's value.
      printf_lcd(" Forme = Sinus      ");
      break;
    case SignalTriangle :
      printf_lcd(" Forme = Triangle   ");
      break;
    case SignalDentDeScie :
      printf_lcd(" Forme = Saw        ");
      break;
    case SignalCarre :
      printf_lcd(" Forme = Square     ");
      break;
    default:
      break;
  }

  lcd_gotoxy(1, 2);                     // Prints the frequency on the 2nd line
  printf_lcd(" Freq [Hz] =    %4ld", pParam->Frequence);
  lcd_gotoxy(1, 3);                     // Prints the amplitude on the 3rd line
  printf_lcd(" Ampl [mV] =   %5ld", pParam->Amplitude);
  lcd_gotoxy(1, 4);                     // Prints the offset on the 4th line
  printf_lcd(" Offset [mV] = %5ld", pParam->Offset);
}

// Execution du menu, appel cyclique depuis l'application
//void Menu_Execute(S_ParamGen *pParam)
void MENU_Execute(S_ParamGen * pParam)
{
  static E_Menu Menu;
  static S_ParamGen MenuTmp;
  static bool ok = false;
  static bool S9Save = false;
  static uint16_t saveCount = 0;
  static uint8_t savePrint = 0u;
  static uint16_t countPrint = 0;

  if (S9Save == false && DebounceIsPressed(&S9Descr) != 0 && countPrint == 0) {
    // Check if the S9 key is pressed and are not in save mode already neither
    // printing the saving statut
    DebounceClearPressed(&S9Descr);
    S9Save = true;                          // Flags the entry into save menu
    ClearLcd();                             // Clears the screen
    lcd_bl_on();                            // Wakes up the blacklight
    lcd_gotoxy(1, 2);                       // Prints the save menu
    printf_lcd("   Sauvegarde ?      ");
    lcd_gotoxy(1, 3);                       // Prints the save menu
    printf_lcd("   (appui long)    ");
  }
  
  // Check if S9 key is pressed and if we are already in save menu
  if (S9Save == true && DebounceGetInput(&S9Descr) == 0) {
    saveCount++;                    // Key must be maintained the be accepted
    if (saveCount >= 500u) {        // Checks how long the key has been pressed
      savePrint = 1u;               // Indicate which message has to be printed
      saveCount = 0;                // Clears the key's pressed time
      
      NVM_WriteBlock((uint32_t *)pParam, sizeof(*pParam)); // Perform saving
    }
  }
  
  else {
    saveCount = 0;                  // Clears the key's pressed time
  }
  
  // Check if any key is pressed to exit the saving menu
  if (S9Save == true && (Pec12IsESC() || Pec12IsPlus()
                         || Pec12IsMinus() || Pec12IsOK())) {
    Pec12ClearESC();
    Pec12ClearPlus();
    Pec12ClearMinus();
    Pec12ClearOK();
    lcd_bl_on();
    savePrint = 2u;               // Indicate which message has to be printed
  }

  // Checks if any save statut has to be printed
  if (S9Save == true && savePrint > 0) {
    switch (savePrint) {
      case 1:
        lcd_gotoxy(1, 2);
        printf_lcd("  Sauvegarde OK ! ");       
        break;
      case 2:
        lcd_gotoxy(1, 2);
        printf_lcd(" Sauvegarde annule !"); 
        break;
      default:
        break;
        
    }
    lcd_gotoxy(1, 3);                       // Prints the save menu
    printf_lcd("                ");
    // Freeze the screen for 2 secondes
    if (countPrint >= VALMAXCOUNT) {
      S9Save = false;                   // Exits saving menu
      Menu_Callback(pParam);            // Prints the original menu
      countPrint = 0;
      savePrint = 0;
      DebounceClearPressed(&S9Descr);
      lcd_gotoxy(1, 1);                     // Go to the first line
      switch (pParam->Forme) {              // Prints a text depending of the
      case SignalSinus :                    // form's value.
        printf_lcd(" Forme = Sinus      ");
        break;
      case SignalTriangle :
        printf_lcd(" Forme = Triangle   ");
        break;
      case SignalDentDeScie :
        printf_lcd(" Forme = Saw        ");
        break;
      case SignalCarre :
        printf_lcd(" Forme = Square     ");
        break;
      default:
        break;
        }
    }
    else {
      countPrint++;
    }
  }

  if (S9Save == false) {        // Test if we are not into saving procedure
                                // Execute the menu only if we are not saving
    if (Pec12IsPlus()) {        // Test is the Pec goes up
      Pec12ClearPlus();
      lcd_bl_on();
      ClearPrefix();
      switch (Menu) {           // Check in which menu or submenu we are
        case Main :             // Select the submenu
          PosCurseur++;
          if (PosCurseur == 5) {
            PosCurseur = 1;
          }
          break;
        case Forme :             // Goes to the next form
          MenuTmp.Forme++;
          if (MenuTmp.Forme > SignalCarre) {
            MenuTmp.Forme = SignalSinus;
          }
          break;
        case Freq :              // Goes to the upper frequency
          MenuTmp.Frequence = MenuTmp.Frequence + 20u;
          if (MenuTmp.Frequence > 2000u) {    // Loop when ceiling reached
            MenuTmp.Frequence = 20u;
          }
          break;
        case Amp :               // Goes to the upper amplitude
          MenuTmp.Amplitude = MenuTmp.Amplitude + 100;
          if (MenuTmp.Amplitude > 10000u) {   // Loop when ceiling reached
            MenuTmp.Amplitude = 0;
          }
          break;
        case Offset :            // Goes to the upper offset
          MenuTmp.Offset = MenuTmp.Offset + 100;
          if (MenuTmp.Offset > 5000) {        // Ceils the offset
            MenuTmp.Offset = 5000;
          }
          break;
        default:
          break;
      }
    }

    if (Pec12IsMinus()) {           // Test if the Pec goes down
      Pec12ClearMinus();
      lcd_bl_on();
      ClearPrefix();
      switch (Menu) {
                                    // Check in which menu or submenu we are
          case Main :               // Select the submenu
            PosCurseur--;
            if (PosCurseur < 1) {
              PosCurseur = 4;
            }
            break;
          case Forme :              // Goes to the previous form
            MenuTmp.Forme--;
            if (MenuTmp.Forme < SignalSinus) {
              MenuTmp.Forme = SignalCarre;
            }
            break;
          case Freq :               // Goes to the lower frequency
            MenuTmp.Frequence = MenuTmp.Frequence - 20;
            if (MenuTmp.Frequence < 20) {       // Loop when floor reached
              MenuTmp.Frequence = 2000;
            }
            break;
          case Amp :                // Goes to the lower amplitude
            MenuTmp.Amplitude = MenuTmp.Amplitude - 100;
            if (MenuTmp.Amplitude < 0) {        // Loop when floor reached
              MenuTmp.Amplitude = 10000;
            }
            break;
          case Offset :             // Goes to the lower offset
            MenuTmp.Offset = MenuTmp.Offset - 100;
            if (MenuTmp.Offset < -5000) {       // Floors the offset
              MenuTmp.Offset = -5000;
            }
            break;
          default:
            break;
          }
    }
    
      
      if (Menu == Main) {           // Prints a prefix on the selected line 
        lcd_gotoxy(1, PosCurseur);
        printf_lcd("*");
      }


      if (ok == false && Pec12IsOK()) { // Check if we enter a submenu
        Pec12ClearOK();
        lcd_bl_on();                    // Exit sleep backlight
        ok = true;                      // Flag the entry into submenu
        
        // Save the generator's parameters
        MenuTmp.Amplitude = pParam->Amplitude;
        MenuTmp.Forme     = pParam->Forme;
        MenuTmp.Frequence = pParam->Frequence;
        MenuTmp.Offset    = pParam->Offset;
      }
      
      if (ok == true) {         // Process the entry into a submenu
        
        // Print a '?' at the beginning of the selected line and change the
        // current menu selection for the new one
        
        switch (PosCurseur) {
          case 1:
            Menu = Forme;
            lcd_gotoxy(1, 1);
            printf_lcd("?");
            break;
          case 2:
            Menu = Freq;
            lcd_gotoxy(1, 2);
            printf_lcd("?");
            break;
          case 3:
            Menu = Amp;
            lcd_gotoxy(1, 3);
            printf_lcd("?");
            break;
          case 4:
            Menu = Offset;
            lcd_gotoxy(1, 4);
            printf_lcd("?");
            break;
          default:
            break;
        }
        MENU_Print(&MenuTmp); // Prints a temporary menu with the live changes
      }
      else {
        MENU_Print(pParam);   // Prints the main menu if no submenu selected
      }

      if (ok == true && Pec12IsOK() == 1) { // Exit temporary submenu and 
                                            // validate done changes
        Pec12ClearOK();
        ok = false;                         // Flag the exit of the submenu
        Menu = Main;                        // Enters into main menu
        
        // Save the temporary menu into the main menu
        pParam->Amplitude = MenuTmp.Amplitude;
        pParam->Forme     = MenuTmp.Forme;
        pParam->Frequence = MenuTmp.Frequence;
        pParam->Offset    = MenuTmp.Offset;
        GENSIG_UpdateSignal(pParam);        // Update the signal with the new
      }                                     // values

      if (ok == true && Pec12IsESC()) { // Exits the submenu if ESC key is used
        Pec12ClearESC();
        ok = false;
        Menu = Main;
      }

      if (Pec12NoActivity()) {          // Turn off backlight if no activity
        Pec12ClearInactivity();
        lcd_bl_off();
      }
    }
} // Menu_Execute

/******************************************************************************/
/*                              Function Menu_Print                           */
/******************************************************************************/
/* This function prints the menu's parameters.                                */
/* Input parameters: S_ParamGen -> menu's values                              */
/* Output parameters: none                                                    */
/******************************************************************************/
void MENU_Print(S_ParamGen* pParam) {
  lcd_gotoxy(10, 1);            // Prints the form's name
  switch (pParam->Forme) {
    case SignalSinus :
      printf_lcd("Sinus   ");
      break;
    case SignalTriangle :
      printf_lcd("Triangle");
      break;
    case SignalDentDeScie :
      printf_lcd("Saw     ");
      break;
    case SignalCarre :
      printf_lcd("Square  ");
      break;
    default:
      break;
  }
  lcd_gotoxy(17, 2);            // Prints the frequency
  printf_lcd("%4ld", pParam->Frequence);
  lcd_gotoxy(16, 3);            // Prints the amplitude
  printf_lcd("%5ld", pParam->Amplitude);
  lcd_gotoxy(16, 4);            // Prints the offset
  printf_lcd("%5ld", pParam->Offset);
}

/******************************************************************************/
/*                             Function ClearPrefix                           */
/******************************************************************************/
/* This function allow to clear only the first column of the LCD screen where */
/* the prefix takes place.                                                    */
/* Input parameters: none                                                     */
/* Output parameters: none                                                    */
/******************************************************************************/
void  ClearPrefix(void) {
  int8_t i;
  for (i = 1; i < 5; i++) { // Clears first character on each lines from 1 to 4
    lcd_gotoxy(1, i);
    printf_lcd(" ");
  }
}

/******************************************************************************/
/*                               Function ClearLcd                            */
/******************************************************************************/
/* This function entirely clear the LCD screen.                               */
/* Input parameters: none                                                     */
/* Output parameters: none                                                    */
/******************************************************************************/
void ClearLcd(void)
{
  int8_t i;
  for (i = 0; i < 5; i++) {   // Clears each line from 1 to 4
    lcd_ClearLine(i);
  }
}

/******************************************************************************/
/*                             Function MenuCallback                          */
/******************************************************************************/
/* This function prints the menu's static text and values.                    */
/* Input parameters: S_ParamGen -> menu's values                              */
/* Output parameters: none                                                    */
/******************************************************************************/
void Menu_Callback(S_ParamGen * pParam)
{
  lcd_gotoxy(1, 1);                     // Go to the first line
  switch (pParam->Forme) {              // Prints a text depending of the
    case SignalSinus :                  // form's value.
      printf_lcd(" Forme = Sinus      ");
      break;
    case SignalTriangle :
      printf_lcd(" Forme = Triangle   ");
      break;
    case SignalDentDeScie :
      printf_lcd(" Forme = Saw        ");
      break;
    case SignalCarre :
      printf_lcd(" Forme = Square     ");
      break;
    default:
      break;
  }

  lcd_gotoxy(1, 2);                     // Prints the frequency on the 2nd line
  printf_lcd(" Freq [Hz] =    %4ld", pParam->Frequence);
  lcd_gotoxy(1, 3);                     // Prints the amplitude on the 3rd line
  printf_lcd(" Ampl [mV] =   %5ld", pParam->Amplitude);
  lcd_gotoxy(1, 4);                     // Prints the offset on the 4th line
  printf_lcd(" Offset [mV] = %5ld", pParam->Offset);
}
