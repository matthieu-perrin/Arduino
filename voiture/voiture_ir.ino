#include <IRremote.h>  
const int irReceiverPin = 2;  

IRrecv irrecv(irReceiverPin); 
decode_results results; 
 
// moteur
int motor1_pin1 = 3;  // PIN L298N IN1
int motor1_pin2 = 4;  // PIN L298N IN2
int motor2_pin1 = 5;  // PIN L298N IN3
int motor2_pin2 = 6;  // PIN L298N IN4
uint8_t etat_moteur[4] = {0, 0, 0, 0};

// capteur avant
int trig = 8;         // PIN HC-SR04 TRIG
int echo = 9;         // PIN HC-SR04 ECHO
long lecture_echo; 
long cm;
long avant_max=15;
bool obstacle_av = 0;

// capteur arriere
int avoidPin = 10;    // PIN IR Avoidance
bool obstacle_ar = 0;

// boutton
int buttonPin = 12; //the button pin attach to

int roule_avant =0;
int roule_arriere = 0;
long randNumber=0;


int action_droite=0;
int action_gauche=0;
int action_avant=0;
int action_arriere=0;

// Arret des moteurs
void moteur_arret()
{
	digitalWrite(motor1_pin1, LOW);
	digitalWrite(motor1_pin2, LOW);
	digitalWrite(motor2_pin1, LOW);
	digitalWrite(motor2_pin2, LOW);	
}

void setup()
{
  irrecv.enableIRIn();  
  Serial.begin(9600);   
  
	pinMode(motor1_pin1, OUTPUT);
	pinMode(motor1_pin2, OUTPUT);
	pinMode(motor2_pin1, OUTPUT);
	pinMode(motor2_pin2, OUTPUT);	
	moteur_arret();
 
  pinMode(trig, OUTPUT); 
  digitalWrite(trig, LOW); 
  pinMode(echo, INPUT);

  pinMode(avoidPin, INPUT);

  randomSeed(analogRead(0));
  //delay(2000);
}

long distance()
{
  long cm;
  digitalWrite(trig, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trig, LOW); 
  lecture_echo = pulseIn(echo, HIGH); 
  cm = lecture_echo / 58; 
  return cm; 
}

bool obstacle_avant()
{  
  bool obstacle = 0;
  cm = distance();
  if ( cm < avant_max ) { obstacle = 1; }
  return obstacle;
}

bool obstacle_arriere()
{
  bool obstacle = 0;
  int avoidVal = digitalRead(avoidPin);
  if(avoidVal == LOW)  { obstacle = 1; }
  return obstacle;
}

void applique_etat_moteur()
{
	moteur_arret();	
	digitalWrite(motor1_pin1, etat_moteur[0]);
	digitalWrite(motor1_pin2, etat_moteur[1]);
	digitalWrite(motor2_pin1, etat_moteur[2]);
	digitalWrite(motor2_pin2, etat_moteur[3]);
}

void change_etat_moteur(uint8_t m1, uint8_t m2, uint8_t m3, uint8_t m4)
{
	etat_moteur[0] = m1;
	etat_moteur[1] = m2;
	etat_moteur[2] = m3;
	etat_moteur[3] = m4;
	applique_etat_moteur();
}

// marche avant
void moteur_avant()
{
 change_etat_moteur(LOW, HIGH, HIGH, LOW);
}

// marche arriere
void moteur_arriere()
{
 change_etat_moteur(HIGH, LOW, LOW, HIGH);
}

// tourne a droite
void moteur_droite()
{
 change_etat_moteur(LOW, HIGH, LOW, HIGH);
}

// tourne a gauche
void moteur_gauche()
{
 change_etat_moteur(HIGH, LOW, HIGH, LOW);
}

// attend avant arret moteur 
void attend(int temps)
{
	if ( temps > 1000 ) { temps = 1000; }
	delay(temps);
	moteur_arret();	
}

// arriere
void arriere(int temps)
{
	moteur_arriere();
	attend(temps);
}

// avance
void avant(int temps)
{
  moteur_avant();
	attend(temps);
}

// tourne a droite 
void droite(int temps)
{
	moteur_droite();
	attend(temps);	
}

// tourne a gauche
void gauche(int temps)
{
	moteur_gauche();
	attend(temps);
}


void loop()
{
  results.value = 0;
  if (irrecv.decode(&results)) { 
    
    //Serial.println("irCode: ");            
    Serial.println(results.value, HEX);
    irrecv.resume();   

  }

  if(results.value == 0xFFC23D) { action_avant = 0; action_arriere = 0; action_droite = 1; action_gauche = 0; } // >> FFC23D
  if(results.value == 0xFF02FD) { action_avant = 0; action_arriere = 0; action_droite = 0; action_gauche = 1; } // << FF02FD
  if(results.value == 0xFF906F) { action_avant = 1; action_arriere = 0; action_droite = 0; action_gauche = 0; } // +  FF906F
  if(results.value == 0xFFA857) { action_avant = 0; action_arriere = 1; action_droite = 0; action_gauche = 0; } // -  FFA857
  if(results.value == 0xFFE01F) { action_avant = 0; action_arriere = 0; action_droite = 0; action_gauche = 0; } // EQ FFE01F
  if(results.value == 0xFF22DD) { action_avant = 0; action_arriere = 0; action_droite = 0; action_gauche = 0; } // >| FF22DD
  if(results.value == 0xFF38C7) { action_avant = 0; action_arriere = 0; action_droite = 0; action_gauche = 0; } // 5  FF38C7
  if(results.value == 0xFF10EF) { action_avant = 0; action_arriere = 0; action_droite = 0; action_gauche = 1; } // 4  FF10EF
  if(results.value == 0xFF5AA5) { action_avant = 0; action_arriere = 0; action_droite = 1; action_gauche = 0; } // 6  FF5AA5
  if(results.value == 0xFF18E7) { action_avant = 1; action_arriere = 0; action_droite = 0; action_gauche = 0; } // 2  FF18E7
  if(results.value == 0xFF4AB5) { action_avant = 0; action_arriere = 1; action_droite = 0; action_gauche = 0; } // 8  FF4AB5

  // ON FFA25D; MODE FF629D; SOUND FFE21D; 0 FF6897; <> FF9867; USD FFB04F; 1 FF30CF; 3 FF7A85; 7 FF42BD; 9 FF52AD
  
  obstacle_av = obstacle_avant();
  obstacle_ar = obstacle_arriere();
  //randNumber = random(10, 40);

  // SI non bloque
  if ( obstacle_av == 0 || obstacle_ar == 0 )
  {

    if( obstacle_ar == 1)
    {
        roule_arriere=0;
        action_arriere=0;        
    }
    else if( obstacle_av == 1)
    {
        roule_avant=0;
        action_avant=0;
    }
    
    if ( obstacle_av == 0 )
    {
      //if ( roule_arriere == 0)
      if ( action_avant == 1)
      {        
        roule_avant=1;
        avant(90);
        moteur_arret();
        attend(5);
      }
    }
    
    if( obstacle_ar == 0)
    {
      //if (roule_avant == 0)
      if (action_arriere == 1)
      {
        roule_arriere=1;
        arriere(90);
        moteur_arret();
        attend(5);
      }
    }

  }
  else
  {    
    moteur_arret(); 
    delay(100);
  }


  // tourne
  //if ( randNumber == 15 )
  if ( action_droite == 1 )
  {
    action_droite =0;
    droite(200);
    attend(100);
  }
  //if ( randNumber == 16)
  if ( action_gauche == 1 )
  {
    action_gauche = 0;
    gauche(200);
    attend(100);
  }
     
  action_droite = 0;
  action_gauche = 0;

   moteur_arret(); 

}
