/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    appgen.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "appgen.h"
#include "Mc32DriverLcd.h"
#include "Mc32gestSpiDac.h"
#include "MenuGen.h"
#include "GesPec12.h"
#include "Generateur.h"
#include "DefMenuGen.h"
#include "Mc32NVMUtil.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APPGEN_DATA appgenData;
S_ParamGen LocalParamGen;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APPGEN_Initialize ( void )

  Remarks:
    See prototype in appgen.h.
 */

void APPGEN_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appgenData.state = APPGEN_STATE_INIT;

    
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}

void LedOFF(void)
{
  BSP_LEDOff(LED0_BIT);
  BSP_LEDOff(LED1_BIT);
  BSP_LEDOff(LED2_BIT);
  BSP_LEDOff(LED3_BIT);
  BSP_LEDOff(LED4_BIT);
  BSP_LEDOff(LED5_BIT);
  BSP_LEDOff(LED6_BIT);
  BSP_LEDOff(LED7_BIT);
}
/******************************************************************************
  Function:
    void APPGEN_Tasks ( void )

  Remarks:
    See prototype in appgen.h.
 */

void APPGEN_Tasks ( void )
{
   static bool print = true;
    /* Check the application's current state. */
    switch ( appgenData.state )
    {
        /* Application's initial state. */
        case APPGEN_STATE_INIT:
        {
          lcd_init();
          lcd_bl_on();
          LedOFF();

          // Init SPI DAC
          SPI_InitLTC2604();
          // Initialisation PEC12
          Pec12Init();
          // Initialisation du generateur
          GENSIG_Initialize(&LocalParamGen);
          lcd_gotoxy(1, 1);
          printf_lcd("Tp5 GenSig 18-19 eth");
          // A adapter pour les 2 noms sur 2 lignes
          lcd_gotoxy(1, 2);
          printf_lcd("Cedric Morard       ");
          lcd_gotoxy(1, 3);
          printf_lcd("Yann da Costa       ");

          // Active les timers
          DRV_TMR0_Start();
          DRV_TMR1_Start();
          appgenData.state = APPGEN_STATE_WAIT;
          break;
        }
    case APPGEN_STATE_WAIT : {
      // nothing to do
      break;
    }
        case APPGEN_STATE_SERVICE_TASKS:
        {
      // Initialisation du menu
          while(print == true){
            MENU_Initialize(&LocalParamGen);
            print = false;
          }
          BSP_LEDToggle(BSP_LED_2);

          // Execution du menu
          MENU_Execute(&LocalParamGen);
          GENSIG_UpdateSignal(&LocalParamGen);
          //GENSIG_Execute();

          appgenData.state = APPGEN_STATE_WAIT;

      break;
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

 
void APP_UpdateState(APPGEN_STATES NewState)
{
  appgenData.state = NewState;
}
/*******************************************************************************
 End of File
 */
