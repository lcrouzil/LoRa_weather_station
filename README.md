# Station météo LoRa

## Description du projet

Le projet consiste à réaliser une station météo permettant à la fois de récupérer des informations mais aussi de piloter un relai.

## Liste de matériel utilisé

- Carte Heltec lora esp32 v2
- Capteur Ultrason Elegoo HC-SR04 (6)
- Capteur intensité lumineuse DollaTek (1)
- Capteur d’humidité du sol ICQUANZX (2)
- Capteur de pluie AZDelivery (4)
- Relais (3)
- Capteur de température, d’altitude et pression atmosphérique BMP280 (5)

## Schéma de cablage

![Alt text](images/cablage.png?raw=true "Câblage de la station météo")

## Exemple montage

![Alt text](images/montage.jpg?raw=true "Exemple de montage fonctionnel")

## Câblage

| N° PIN heltec  | élément              | Broche élément    |
|:---------------|:---------------------|:------------------|
| 17             | Relais               | S                 |
| 13             | Capteur pluie        |  D0               |
| 38             | Capteur luminosité   |  D0               |
| 15             | BMP 280              | SCL               |
| 4              | BMP 280              | SDA               |
| 39             | Capteur humidité sol | OUT               |
| 23             | HCSR04               | Trigger           |
| 22             | HCSR04               | Echo              |

## Interface QT

![Alt text](images/interfaceqt.png?raw=true "Interface")

## Résultats

![Alt text](images/relaisTTN.PNG?raw=true "Résultat sur TTN")
![Alt text](images/decoded_payload.PNG?raw=true "Format du payload decodé")
![Alt text](images/terminal.PNG?raw=true "Résultat du terminal")



