// Mc32gestSpiDac.C
// Gestion master SPI avec slave DAC

// Pilote du DAC LTC2604
// Auteur C. Huber  30.04.2012
// Adaptation plib_spi 10.02.2015 CHR
//      Modifications :
//          CHR 06.03.2015  ajout fonction SPI_CfgWriteToDac
//          CHR 24.05.2016  ajout #include "Mc32Delays.h"
//          CHR 25.05.2016  utilisation de SYS_CLK_PeripheralFrequencyGet(CLK_BUS_PERIPHERAL_1)

#include "app.h"   // pour system_config et autre
#include "Mc32gestSpiDac.h"
#include "Mc32SpiUtil.h"
#include "peripheral\SPI\plib_spi.h"
#include "Mc32Delays.h"

// SPI_ID_1 correspond au SPI1 !
#define KitSpi1 (SPI_ID_1)

uint32_t ConfigReg;     // pour lecture de SPI1CON
uint32_t BaudReg;       // pour lecture de SPI1BRG

// Initialisation de la communication SPI et du DAC
// ------------------------------------------------

// Pour observation des signaux SPI
// SPI-SCK   RD10 pin 70
// SPI-MOSI  RD0  pin 72
// SPI-MISO  RC4  pin  9
// SPI-CS_DA RD4  pin 81

// Ancienne configuration pour macro XC32
// uint32_t  config = SPI_OPEN_MODE8 | SPI_OPEN_ON | SPI_OPEN_CKP_HIGH |
//                    SPI_OPEN_MSTEN ;


void SPI_ConfigureLTC2604(void)
{
  PLIB_SPI_Disable(KitSpi1);

  PLIB_SPI_BufferClear(KitSpi1);
  PLIB_SPI_StopInIdleDisable(KitSpi1);
  PLIB_SPI_PinEnable(KitSpi1, SPI_PIN_DATA_OUT);
  PLIB_SPI_CommunicationWidthSelect(KitSpi1, SPI_COMMUNICATION_WIDTH_8BITS);
  // Config SPI clock � 20 MHz
  PLIB_SPI_BaudRateSet(KitSpi1, SYS_CLK_PeripheralFrequencyGet(CLK_BUS_PERIPHERAL_1), 20000000);
  // Config polarit� traitement des signaux SPI
  // pour input � confirmer
  // Polarit� clock OK
  // Phase output � confirmer
  PLIB_SPI_InputSamplePhaseSelect(KitSpi1, SPI_INPUT_SAMPLING_PHASE_IN_MIDDLE);
  PLIB_SPI_ClockPolaritySelect(KitSpi1, SPI_CLOCK_POLARITY_IDLE_HIGH);
  PLIB_SPI_OutputDataPhaseSelect(KitSpi1, SPI_OUTPUT_DATA_PHASE_ON_IDLE_TO_ACTIVE_CLOCK);
  PLIB_SPI_MasterEnable(KitSpi1);
  PLIB_SPI_FramedCommunicationDisable(KitSpi1);
  PLIB_SPI_FIFOEnable(KitSpi1);     // Enhenced buffer mode

  PLIB_SPI_Enable(KitSpi1);

  // Contr�le le la configuration
  ConfigReg = SPI1CON;
  BaudReg = SPI1BRG;
}

void SPI_InitLTC2604(void)
{

  //Initialisation SPI DAC
  CS_DAC = 1;
  // Impulsion reset du DAC
  DAC_CLEAR = 0;
  delay_us(500);
  DAC_CLEAR = 1;

  // LTC2604 MAX 50 MHz choix 20 MHz
  SPI_ConfigureLTC2604();
}


// Envoi d'une valeur sur le DAC  LTC2604
// Sans reconfiguration du SPI
// Indication du canal 0 � 3
void SPI_WriteToDac(uint8_t NoCh, uint16_t DacVal)
{

  //D�claration des variables
  uint8_t MSB;
  uint8_t LSB;

  //S�lection du canal
  //3 -> Set and Update, 0/1/2/3 S�lection canal A/B/C/D, F tous canaux
  NoCh = NoCh + 0x30;

  // Selon canal
  MSB =  DacVal >> 8;
  LSB =  DacVal;

  CS_DAC = 0;
  spi_write1(NoCh);
  spi_write1(MSB);
  spi_write1(LSB);

  //Fin de transmission
  CS_DAC = 1;

} // SPI_WriteToDac

// Envoi d'une valeur sur le DAC  LTC2604
// Avec reconfiguration du SPI
// Indication du canal 0 � 3
void SPI_CfgWriteToDac(uint8_t NoCh, uint16_t DacVal)
{

  //D�claration des variables
  uint8_t MSB;
  uint8_t LSB;

  // Reconfiguration du SPI
  SPI_ConfigureLTC2604();

  //S�lection du canal
  //3 -> Set and Update, 0/1/2/3 S�lection canal A/B/C/D, F tous canaux
  NoCh = NoCh + 0x30;

  // Selon canal
  MSB =  DacVal >> 8;
  LSB =  DacVal;

  CS_DAC = 0;
  spi_write1(NoCh);
  spi_write1(MSB);
  spi_write1(LSB);

  // Fin de transmission
  CS_DAC = 1;

} // SPI_CfgWriteToDac






