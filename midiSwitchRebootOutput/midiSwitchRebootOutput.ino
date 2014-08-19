/*
* Commande une pédale 10 footswitch avec sortie MIDI sur DIN 5 broches
* license MIT
*/


#include <midi_Namespace.h>
#include <midi_Settings.h>
#include <midi_Defs.h>
#include <MIDI.h>

#include <SimpleTimer.h>

//#define IS_SERIAL 

const int LED_0 =  13;
const int N_BTN = 11;
const int ledTime = 255;
const int pushTime = 1000;
const int OFFSET = 2;


// numéro de bouton utlisé lors de l'appel de appuiLong
unsigned char numButton;
int timerAppuiLong;

int idx;
boolean isAppuiLong;
SimpleTimer timer;

// Gestion des statuts des boutons
boolean oldStatus[N_BTN] = {0,0};
boolean newStatus[N_BTN] = {0,0};
boolean *pNew, *pOld;

void eteindLed() {
  digitalWrite(LED_0, LOW); 
}

// Callback: allume lad correspondant au dernier bouton enfoncé
void allumeLed() {
  digitalWrite(LED_0, HIGH); 
}

// Callback: appelé après qu'un bouton soit enfoncé le temps requis
void appuiLong() {
  isAppuiLong = true;
  MIDI.sendControlChange(numButton+10,numButton+10,1);
  allumeLed();
  timer.setTimeout(ledTime<<2, eteindLed); 
}

// envoie un message si le bouton vient d'etre enclenché
int sendMessage(boolean isPushedNew, boolean isPushedOld, int tNumButton) {
  //si on vient d'appuyer
  if (!isPushedNew && isPushedOld) {
    isAppuiLong = false;
    numButton = tNumButton;
    timerAppuiLong = timer.setTimeout(pushTime, appuiLong);
  }  
  // si on relache
  else if (isPushedNew && !isPushedOld && !isAppuiLong) {
    allumeLed(); 
    numButton = tNumButton;
    timer.deleteTimer(timerAppuiLong);
    timerAppuiLong = 0;
    MIDI.sendControlChange(numButton,numButton,1);
    timer.setTimeout(ledTime, eteindLed);   
  }
  return 1; 
}

void setup()
{
  pNew = newStatus;
  pOld = oldStatus;
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);   
  pinMode(4, INPUT_PULLUP);  
  pinMode(5, INPUT_PULLUP);  
  pinMode(6, INPUT_PULLUP);   
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);   
  pinMode(10, INPUT_PULLUP);  
  pinMode(11, INPUT_PULLUP);  
  pinMode(12, INPUT_PULLUP);   

  pinMode(LED_0, OUTPUT);
  MIDI.begin();
  #ifdef IS_SERIAL
  //Serial.begin(115200);
  #endif
  isAppuiLong = false;
}

void loop()
{
  boolean *pSwap;
  boolean isChanged = false;
  boolean isReleased = false;
  boolean isPushed = false;
 
  timer.run(); 
  pSwap = pOld;
  pOld = pNew;
  pNew = pSwap;
  
  for (idx = 0; idx < N_BTN; idx++) {
    pNew[idx] = digitalRead(idx+OFFSET);
  }
  
  for (idx = 0; idx < N_BTN; idx++) {
    if (pOld[idx] != pNew[idx]) {
      sendMessage(pNew[idx], pOld[idx], idx);
    } 
  }
  delay(2);
}
