# Station météo LoRa

## Description du projet

Le projet consiste à réaliser une station météo permettant à la fois de récupérer des informations mais aussi de piloter un relais.

## Liste de matériel utilisé

- Carte Heltec lora esp32 v2
- Capteur Ultrason Elegoo HC-SR04
- Capteur intensité lumineuse DollaTek
- Capteur d’humidité du sol ICQUANZX
- Capteur de pluie AZDelivery
- Relais
- Capteur de température, d’altitude et pression atmosphérique BMP280

## schéma de cablage

![Alt text](images/cablage.png?raw=true "cablage de la station météo")

## Cablage

| N° PIN heltec  | élément              | Broche élément    |
|:---------------|:---------------------|:------------------|
| 17             | Relais               | S                 |
| 13             | Capteur pluie        |  D0               |
| 38             | Capteur luminosité   |  D0               |
| 15             | BMP 280              | SCL               |
| 4              | BMP 280              | SDA               |
| 39             | Capteur humidité sol | OUT               |
| 23             | HCSR04               | Trigger           |
|22              | HCSR04               | Echo              |

## Interface QT

![Alt text](images/interfaceqt.png?raw=true "Interface")

## resultats

![Alt text](images/relaisTTN.PNG?raw=true "Resultat sur TTN")
![Alt text](images/decoded_payload.PNG?raw=true "Format du payload decodé")
![Alt text](images/terminal.PNG?raw=true "Resultat du terminal")



