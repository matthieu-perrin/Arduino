
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
  delay(2000);
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
  obstacle_av = obstacle_avant();
  obstacle_ar = obstacle_arriere();
  randNumber = random(10, 60);

  // SI non bloque
  if ( obstacle_av == 0 || obstacle_ar == 0 )
  {

    if( obstacle_ar == 1)
    {
        roule_arriere=0;
    }
    else if( obstacle_av == 1)
    {
        roule_avant=0;
    }
    
    if ( obstacle_av == 0 )
    {
      if ( roule_arriere == 0)
      {        
        roule_avant=1;
        avant(100);
      }
    }
    
    if( obstacle_ar == 0)
    {
      if (roule_avant == 0)
      {
        roule_arriere=1;
        arriere(100);
      }
    }

  }
  else
  {    
    moteur_arret(); 
    delay(100);
  }

  // tourne
  if ( randNumber == 15 )
  {
    droite(500);
  }
  if ( randNumber == 16)
  {
    gauche(500);
  }
    
   moteur_arret(); 

}
