/****************************************************************************************
*               INCLUDE                                                                 *
*****************************************************************************************/

#include<Arduino.h>
#include <Wire.h> 
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_BMP280.h>
#include <ESP32_LoRaWAN.h>
#include <HCSR04.h>


/****************************************************************************************
*               DEFINE                                                                  *
*****************************************************************************************/

#define PLUIE_SENSOR 13
#define LUMINO_SENSOR 38
#define SOL_SENSOR 39
#define TRIG 23
#define ECHO 22
#define RELAIS 17


#define SOL_MIN 0
#define SOL_MAX 3000

/****************************************************************************************
*               Initialisation                                                          *
*****************************************************************************************/


Adafruit_BMP280 bmp; 
UltraSonicDistanceSensor ultra(5,18,4000,500000);


/****************************************************************************************
*               OTAA                                                                    *
*****************************************************************************************/

//uint32_t license[4] = {0xD5397DF0, 0x8573F814, 0x7A38C73D, 0x48E68607}; //licence an dernier
//uint32_t license[4] = {0x2C439F9B,0xC671C1CE,0x072D24F4,0x6C62D258};//licence carte 1 
uint32_t license[4] = {0x643B5140,0x825FACF4,0x7F76F751,0xC35B629D}; //licence carte 2

/* OTAA para*/

uint8_t DevEui[] = {0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x05, 0x83, 0xD9};

uint8_t AppEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; 

uint8_t AppKey[] = {0x64, 0x5E, 0xD5, 0x91, 0x61, 0x4E, 0x07, 0xA0, 0xA5, 0xF0, 0x30, 0xEA, 0x20, 0xAB, 0xAB, 0xDB};

/* ABP para*/
uint8_t NwkSKey[] = { 0xAE, 0xB5, 0x4A, 0xFD, 0x2D, 0xBF, 0x0F, 0x1C, 0x74, 0x4F, 0x74, 0x17, 0x12, 0x40, 0x96, 0xE7 };
uint8_t AppSKey[] = { 0xCA, 0x0B, 0xC5, 0x77, 0x77, 0xBF, 0xC4, 0xF8, 0x18, 0x06, 0x6D, 0x6A, 0x00, 0xF5, 0xF9, 0xFA };
uint32_t DevAddr = (uint32_t)0x260BEE9E;

/*LoraWan channelsmask, default channels 0-7*/
uint16_t userChannelsMask[6] = { 0x00FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

DeviceClass_t  loraWanClass = CLASS_A;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = 15000;

/*OTAA or ABP*/
bool overTheAirActivation = true;

/*ADR enable*/
bool loraWanAdr = true;

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = true;

/* Application port */
uint8_t appPort = 2;

uint8_t confirmedNbTrials = 8;

uint8_t debugLevel = LoRaWAN_DEBUG_LEVEL;

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/*fonction recuperation distance, capteur HCSR04*/
int getDistance()
{
  float distance;
  digitalWrite(TRIG,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG,LOW);
  while(!digitalRead(ECHO));
  long temps_debut = micros();
  while(digitalRead(ECHO));
  long temps_fin = micros();
 
    
  // CALCUL DE LA DISTANCE
  long ecart_temps = temps_fin - temps_debut;
  distance = 0.000340 * (float)ecart_temps / 2.0;
  
  Serial.print("temps debut"); 
  Serial.println(temps_debut); 
  Serial.print("temps fin"); 
  Serial.println(temps_fin);  
  Serial.print("distance"); 
  Serial.println(distance); 
  
  int dist=distance*1000; // en mm 
  
  Serial.print("dist"); 
  Serial.println(dist); 

  return dist;
}

/*preparation de la trame*/
static void prepareTxFrame(uint8_t port) {

  Serial.println("-----trame debut -----");

  float sensorTemp = bmp.readTemperature(); //retour en celcius
  uint16_t sensorTemperature = sensorTemp*10 + 400;
  
  uint8_t sensorHum = bmp.readAltitude(1013.25);// altitude car pas d'humidite sur le bmp280

  uint32_t sensorPres = bmp.readPressure() ; // a diviser par 100 car en Pa et on veux des hPa
  uint8_t sensorSol = analogRead(SOL_SENSOR); //hum sol
  uint16_t sensorDistance = getDistance(); // distance
  uint8_t sensorPluie = digitalRead(PLUIE_SENSOR); // capteur pluie
  uint8_t sensorLumino = digitalRead(LUMINO_SENSOR); // capteur lumino
  uint16_t sensorPression = sensorPres/100; //pression
  int humSol = map(sensorSol, SOL_MIN, SOL_MAX, 0, 100); // permet de modifier la plage de valeur entre 0 et 100 pour %
  
  /*verification*/
  Serial.print("Temperature lu : ");
  Serial.println(sensorTemperature-400);

  Serial.print("Altitude lu : ");
  Serial.println(sensorHum);

  Serial.print("Pression lu : ");
  Serial.println(sensorPres);
  Serial.print("Pression convertie : ");
  Serial.println(sensorPres/100);
  
  Serial.print("Hum sol lu : ");
  Serial.println(humSol);

  Serial.print("Lumino lu : ");
  Serial.println(sensorLumino);

  Serial.print("Pluie lu : ");
  Serial.println(sensorPluie);

  Serial.print("distance lu : ");
  Serial.println(sensorDistance);
  

  /*creation de la trame*/
  appDataSize = 6;
  appData[0] = sensorTemperature;
  appData[1] = (sensorTemperature >> 8 & 0b00000011) | sensorHum << 2;
  appData[2] = (sensorHum >> 6 &0b00001111) | sensorPression << 4;
  appData[3] = (sensorPression >> 4 &0b00111111) | humSol<< 6;
  appData[4] = (humSol >> 2 &0b00011111) | sensorDistance << 5;
  appData[5] = (sensorDistance >> 3 &0b00111111) | sensorLumino << 6 | sensorPluie << 7;
  
  Serial.println("-----trame fin -----");

}

void  downLinkDataHandle(McpsIndication_t *mcpsIndication)
{

    lora_printf("+REV DATA:%s,RXSIZE %d,PORT %d\r\n",mcpsIndication->RxSlot?"RXWIN2":"RXWIN1",mcpsIndication->BufferSize,mcpsIndication->Port);

    lora_printf("+REV DATA:");

  for(uint8_t i=0;i<mcpsIndication->BufferSize;i++)
  {

    lora_printf("%02X",mcpsIndication->Buffer[i]);
    if(mcpsIndication->Buffer[i] == false){
      digitalWrite(17,LOW);
      Serial.println("relais low");
    }else {
      digitalWrite(17,HIGH);
      Serial.println("relais haut");
    }
  }
  
  lora_printf("\r\n");

}

/****************************************************************************************
*               SETUP                                                                  *
*****************************************************************************************/

void setup()
{

    Serial.begin(115200);
    pinMode(LUMINO_SENSOR, INPUT);
    pinMode(PLUIE_SENSOR, INPUT);
    pinMode(17, OUTPUT); //relais
    pinMode(TRIG,OUTPUT);
    pinMode(ECHO,INPUT);

    SPI.begin(SCK, MISO, MOSI, SS);
    Mcu.init(SS, RST_LoRa, DIO0, DIO1, license);
    deviceState = DEVICE_STATE_INIT;
    Display.init();

    while (!Serial || !bmp.begin(0x76))    {
        Serial.println("BMP en PLS ...");
        delay(2000);
    }

}

/****************************************************************************************
*               LOOP                                                                    *
*****************************************************************************************/

void loop() {

    switch (deviceState) {
        case DEVICE_STATE_INIT:
        {
            Serial.println("init");
            #if (LORAWAN_DEVEUI_AUTO)
                LoRaWAN.generateDeveuiByChipID();
            #endif
            LoRaWAN.init(loraWanClass,loraWanRegion);
            break;
        }
        case DEVICE_STATE_JOIN:
        {
          Serial.println("join");
            LoRaWAN.join();
            break;
        }
        case DEVICE_STATE_SEND:
        {
            Serial.println("send");
            prepareTxFrame(appPort);
            LoRaWAN.send(loraWanClass);
            deviceState = DEVICE_STATE_CYCLE;
            break;
        }
        case DEVICE_STATE_CYCLE:
        {
        // Schedule next packet transmission
            txDutyCycleTime = appTxDutyCycle + randr(-APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND);
            LoRaWAN.cycle(txDutyCycleTime);
            deviceState = DEVICE_STATE_SLEEP;
            break;
        }
        case DEVICE_STATE_SLEEP:
        {
            LoRaWAN.sleep(loraWanClass, debugLevel);
            break;
        }
        default:
        {
            deviceState = DEVICE_STATE_INIT;
            break;
        }
    }
}
