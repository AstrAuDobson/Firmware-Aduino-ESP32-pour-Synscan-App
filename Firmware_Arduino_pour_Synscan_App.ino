/*
  Firmware Arduino pour Synscan app by "Astr' Au Dobson" astraudobson@gmail.com
  Code nécessitant une carte ESP32
*/

// Librairies permettant de gérer une connexion wifi
#include <WiFi.h>
#include <WiFiUdp.h>


// Constantes & variables
const char* ip_serveur = "192.168.4.2";      // Adresse IP du téléescope (serveur)
const char* ssid = "SynScanDuino";           // Nom du réseau de connexion au télescope
const char* password = "123456789";          // Mot de passe du réseau
int local_port = 11881;                      // Port de communication pour l'échange UDP

#define bufferSize 8192
uint8_t udpBuffer[bufferSize];

// Instances 
WiFiUDP udp;


void setup() {
  WiFi.disconnect();                        // Si une instance précédente existe, elle est fermée
  delay(2000);
  Serial.begin(115200);                     // INITIALISATION COMMUNICATION AVEC PC avec un baudrate de 115200
  Serial.println("-------------------------------------------------------");
  Serial.print("Connexion au téléscope en cours");  //Affiche ce message sur le PC si la carte arduino y est connecté en USB
  
  WiFi.mode(WIFI_AP);                       // INITIALISATION WIFI en mode Access Point
  IPAddress ip(192, 168, 4, 2);
  IPAddress subnet(255, 255, 255, 0);
  //WiFi.softAPConfig(ip, ip, subnet);
   WiFi.softAP("SynScanDuino");
 
  WiFi.begin(ssid, password);               // Démarre le wifi
  while (WiFi.status() != WL_CONNECTED) {delay(500); Serial.print(".");} // Tant que la connexion est pas OK, on boucle
  Serial.println("");
  Serial.println("Connexion au téléscope établie");

  Serial.println("Démarrage udp");
  Serial.println("-------------------------------------------------------");
  udp.begin(local_port);                    // Démarrage de la connexion UDP sur le port 11880
  delay(1000);

 Serial.println("Sync position Télescope");
 udp.beginPacket(ip_serveur, local_port);
 udp.print("SyncToCoordinates,13,27");
 udp.endPacket();
 delay(1000);
 affiche_monitoring();
 delay(1000);   
}



void loop() {
 udp.flush();
 Serial.println("Position Télescope AZ/ALT");
 udp.beginPacket(ip_serveur, local_port);
 udp.print("AzimuthAltitudeGet");
 udp.endPacket();
 delay(500);
 affiche_monitoring();
 
 Serial.println("Position Télescope AD/DEC");
 udp.beginPacket(ip_serveur, local_port);
 udp.print("RightAscensionDeclinationGet");
 udp.endPacket();
 delay(500);
 affiche_monitoring();
 delay(4000);  
 Serial.println("-------------------------------------------------------");

}


// LIT LA REPONSE DU TELESCOPE ET L'AFFICHE SUR LE PC 
void affiche_monitoring()
{
   int packetSize = udp.parsePacket();
   if (packetSize)
   {
      //Serial.printf("Reçus %d bytes de %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
      int len = udp.read(udpBuffer, 255);
      if (len > 0)
         {
            udpBuffer[len] = '\0';
         }
      Serial.printf("Pos. : %s\n", udpBuffer);
   }
}
