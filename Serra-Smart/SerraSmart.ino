//importazione librerie
#include <DHT.h>
#define RELAISPIN 2
#define DHTPIN 6
#define DHTTYPE DHT11 //impostazione parametri libreria per il dht
#define LED1PIN 3
#define LED2PIN 4
DHT dht(DHTPIN, DHTTYPE);

//dichiarazione variabili
unsigned long prevUmid = 0;    //Memoria tempo umidita' suolo
unsigned long prevDht = 0;     //Memoria tempo DHT
unsigned long waitStampa = 0;   //Memoria per timer stampa
unsigned long startMotore = 0;  //Memoria inizio movimento finestra

bool finestra = false;
bool irrigazione = false;
bool inMovimento = false;       //Indica se la finestra sta aprendo/chiudendo

//setting interfacce
void setup(){
  Serial.begin(9600);
  dht.begin();
  pinMode(RELAISPIN, OUTPUT);
  pinMode(LED1PIN, OUTPUT);
  pinMode(LED2PIN, OUTPUT);
  
  digitalWrite(RELAISPIN, LOW);
  digitalWrite(LED1PIN, LOW);
  digitalWrite(LED2PIN, LOW);
}

//ciclo principale
void loop(){
  // Controllo fine corsa motore (spegne i LED dopo 5 secondi)
  gestisciMovimento();

  /*acquisizione umidita' terreno e check irrigazione*/
  if ( tempo(prevUmid, 2000) == true ) {
    float terreno = umiditaTerreno();
    Serial.print("Umidita' terreno:");
    Serial.print(terreno, 1);
    Serial.print("% \n");
    checkUmid(terreno, irrigazione);
  }

  /*lettura dht*/
  if ( tempo(prevDht, 1000) == true){
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    risultati(t, h, 10000); //Stampa ogni 10 sec temperatura e umidita' della serra

    /*Check per apertura/chiusura finestra (solo se il motore e' fermo)*/
    if (t > 30.0 && h > 70.0 && finestra == false && inMovimento == false){
      apriFinestra();
    } else if (t < 27.0 && h < 65.0 && finestra == true && inMovimento == false){
      chiudiFinestra();
    }
  }
}

/*FUNZIONI*/
bool tempo(unsigned long &tPrecedente, unsigned long interv){
  if (millis() - tPrecedente >= interv){
    tPrecedente = millis();
    return true;
  } else return false;
}

float umiditaTerreno(){
  float voltage = analogRead(A0);
  voltage = (voltage / 1023.0 * 100.0);
  return voltage;
}

void checkUmid(float &terreno, bool &irrigazione){
  if (terreno <= 30.0 && irrigazione == false){
    Serial.print("Umidita' terreno insufficiente: attivazione irrigazione \n");
    digitalWrite(RELAISPIN, HIGH);
    irrigazione = true;
  } else if (terreno >= 70.0 && irrigazione == true){
    Serial.print("Umidita' terreno sufficiente: disattivazione irrigazione \n");
    digitalWrite(RELAISPIN, LOW);
    irrigazione = false;
  }
}

void risultati(float &temperature, float &humidity, unsigned long waiting){
  if (tempo(waitStampa, waiting) == true) {
    Serial.print("Temperatura: ");
    Serial.print(temperature, 1);
    Serial.print("*C |");
    Serial.print(" Umidita': ");
    Serial.print(humidity, 1);
    Serial.print("% \n");
  }
}

void apriFinestra(){
  Serial.print("Temperatura e Umidita' fuori range. \n");
  Serial.print("Apertura finestra in corso... \n");
  digitalWrite(LED1PIN, LOW);   // Assicura spento il verso opposto
  digitalWrite(LED2PIN, HIGH);  // Accende il comando apertura
  startMotore = millis();       // Salva il timestamp di partenza
  inMovimento = true;
}

void chiudiFinestra(){
  Serial.print("Temperatura e Umidita' in range. \n");
  Serial.print("Chiusura finestra in corso... \n");
  digitalWrite(LED2PIN, LOW);   // Assicura spento il verso opposto
  digitalWrite(LED1PIN, HIGH);  // Accende il comando chiusura
  startMotore = millis();       // Salva il timestamp di partenza
  inMovimento = true;
}

void gestisciMovimento(){
  // Quando sono passati 5000 ms dall'inizio del comando
  if (inMovimento && (millis() - startMotore >= 5000)) {
    digitalWrite(LED1PIN, LOW); // Spegne motore chiusura
    digitalWrite(LED2PIN, LOW); // Spegne motore apertura
    inMovimento = false;        // Movimento completato
    
    // Aggiorna lo stato logico della finestra
    if (finestra == false) {
      finestra = true;
      Serial.print("Finestra aperta. \n");
    } else {
      finestra = false;
      Serial.print("Finestra Chiusa. \n");
    }
  }
}