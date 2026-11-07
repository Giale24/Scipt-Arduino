# 🌿 Smart Greenhouse System - Arduino Uno

Progetto di automazione e monitoraggio ambientale per una serra smart basato su scheda **Arduino Uno**. Il sistema gestisce in modo autonomo e non-bloccante l'irrigazione del terreno e la ventilazione tramite simulazione di movimentazione finestra.

---

## 📋 Indice
- [Caratteristiche](#-caratteristiche)
- [Componenti Hardware](#-componenti-hardware)
- [Schema Circuitale e Cablaggio](#-schema-circuitale-e-cablaggio)
- [Logica di Funzionamento](#-logica-di-funzionamento)
- [Installazione e Configurazione](#-installazione-e-configurazione)

---

## 🚀 Caratteristiche
- **Monitoraggio continuo:** Lettura periodica di umidità del suolo, temperatura e umidità dell'aria.
- **Irrigazione intelligente con isteresi:** Attivazione della pompa quando il terreno è secco ($\le 30\%$) e disattivazione al raggiungimento della corretta umidità ($\ge 70\%$).
- **Ventilazione automatica:** Apertura finestra se i parametri ambientali superano le soglie critiche ($T > 30^\circ\text{C}$ e $H > 70\%$), con chiusura automatica al rientro nei valori nominali ($T < 27^\circ\text{C}$ e $H < 65\%$).
- **Architettura non-bloccante:** Temporizzazione gestita interamente tramite la funzione `millis()`, senza blocchi da `delay()`.
- **Timer di fine corsa:** Spegnimento automatico dei comandi motore/LED dopo 5 secondi di corsa.

---

## 🛠️ Componenti Hardware
| Componente | Quantità | Note |
|---|---|---|
| Arduino Uno R3 | 1 | Microcontrollore principale |
| Sensore DHT11 | 1 | Rilevamento temperatura e umidità aria |
| Sensore Igrometro Suolo | 1 | Sonda a forcella con modulo comparatore |
| Modulo Relè 5V (1 canale) | 1 | Comando pompa di irrigazione |
| LED 5mm (Verde/Rosso o Giallo) | 2 | Simulazione apertura/chiusura finestra |
| Resistenze $220\,\Omega$ | 2 | Resistenze di pull-down/limitazione per i LED |
| Breadboard e Cavi Jumper | q.b. | Connessioni prototipali |

---

## 🔌 Schema Circuitale e Cablaggio

### Tabella Collegamenti

| Periferica | Pin Modulo | Pin Arduino Uno | Note |
|---|---|---|---|
| **DHT11** | VCC | `5V` | Alimentazione |
| | GND | `GND` | Massa |
| | DATA / OUT | `D6` | Canale dati digitale |
| **Sensore Suolo** | VCC | `5V` (o `3.3V`) | Alimentazione modulo comparatore |
| | GND | `GND` | Massa |
| | A0 | `A0` | Uscita analogica ($0 - 1023$) |
| **Modulo Relè** | VCC | `5V` | Alimentazione bobina |
| | GND | `GND` | Massa |
| | IN | `D2` | Segnale pilotaggio relè |
| **LED 1 (Chiusura)** | Anodo (+) | `D3` | Tramite resistenza da $220\,\Omega$ in serie |
| | Catodo (-) | `GND` | Massa |
| **LED 2 (Apertura)** | Anodo (+) | `D4` | Tramite resistenza da $220\,\Omega$ in serie |
| | Catodo (-) | `GND` | Massa |

---

### Schema a Blocchi (Pinout)

```text
                  +-------------------+
                  |    ARDUINO UNO    |
                  |                   |
[DHT11]           |                   |
  VCC ------------+ 5V                |
  GND ------------+ GND               |
  DATA -----------+ D6                |
                  |                   |
[Sensore Suolo]   |                   |
  VCC ------------+ 5V                |
  GND ------------+ GND               |
  A0 -------------+ A0                |
                  |                   |
[Modulo Relè]     |                   |
  VCC ------------+ 5V                |
  GND ------------+ GND               |
  IN -------------+ D2                |
                  |                   |
[LED 1 - Chiusura]|                   |
  Anodo --[220Ω]--+ D3                |
  Catodo ---------+ GND               |
                  |                   |
[LED 2 - Apertura]|                   |
  Anodo --[220Ω]--+ D4                |
  Catodo ---------+ GND               |
                  +-------------------+
```

---

## ⚙️ Logica di Funzionamento

### 1. Controllo Terreno (Ciclo ogni 2000 ms)
- Il valore analogico letto su `A0` viene normalizzato in percentuale:
  $$\text{Umidità \%} = \frac{\text{Valore Analogico}}{1023.0} \times 100$$
- Se $\text{Umidità} \le 30.0\% \rightarrow$ attiva il relè (`RELAISPIN = HIGH`).
- Se $\text{Umidità} \ge 70.0\% \rightarrow$ spegne il relè (`RELAISPIN = LOW`).

### 2. Controllo Ventilazione Finestra (Ciclo ogni 1000 ms)
- Il sensore DHT11 campiona temperatura ($T$) e umidità relativa ($H$).
- Stampa di diagnostica ogni 10 secondi su porta seriale a **9600 baud**.
- **Apertura:** se $T > 30.0^\circ\text{C}$ e $H > 70.0\%$, attiva `LED2PIN` per 5 secondi, poi lo disattiva e registra la finestra come aperta.
- **Chiusura (Isteresi):** se $T < 27.0^\circ\text{C}$ e $H < 65.0\%$, attiva `LED1PIN` per 5 secondi, poi lo disattiva e registra la finestra come chiusa.

---

## 💻 Installazione e Configurazione

1. Clona il repository o scarica la cartella del progetto:
   ```bash
   git clone https://github.com/tuo-username/tuo-repo.git
   ```
2. Installa la libreria dipendente tramite l'**Arduino IDE Library Manager**:
   - `DHT sensor library` by Adafruit
3. Apri il file `Codice_esame.ino`.
4. Collega la scheda Arduino via USB, seleziona la porta corretta dal menu **Strumenti > Porta** e clicca su **Carica**.
5. Apri il **Monitor Seriale** impostando il baud rate a `9600`.