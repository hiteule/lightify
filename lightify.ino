#define DISTANCE_DETECT 300
#define DELAY_DETECT 50
#define MODE_PIN 11

struct Feu {int pinRed; int pinYellow; int pinGreen; int red; int yellow; int green;};
struct Pedestrian {int pinRed; int pinGreen; int red; int green;};
struct Distance {int pinTrigger; int pinEcho;};

Feu feuDroite = {A5, A4, A3, LOW, LOW, LOW};
Feu feuGauche = {A2, A1, A0, LOW, LOW, LOW};
Pedestrian pedestrian = {12, 13, LOW, LOW};

Distance distanceDroite = {2, 3};
Distance distanceGauche = {4, 5};
Distance distancePedestrian = {6, 7};

void setup() {
  Serial.begin(9600);

  pinMode(MODE_PIN, INPUT);
  
  pinMode(pedestrian.pinRed, OUTPUT);
  pinMode(pedestrian.pinGreen, OUTPUT);

  pinMode(distanceDroite.pinTrigger, OUTPUT);
  pinMode(distanceDroite.pinEcho, INPUT);
  pinMode(distanceGauche.pinTrigger, OUTPUT);
  pinMode(distanceGauche.pinEcho, INPUT);
  pinMode(distancePedestrian.pinTrigger, OUTPUT);
  pinMode(distancePedestrian.pinEcho, INPUT);

  digitalWrite(distanceDroite.pinTrigger, LOW);
  digitalWrite(distanceGauche.pinTrigger, LOW);
  digitalWrite(distancePedestrian.pinTrigger, LOW);

  setFeu(feuDroite, LOW, LOW, LOW);
  setFeu(feuDroite, LOW, LOW, LOW);
  setFeu(feuGauche, LOW, LOW, LOW);
  setPedestrian(pedestrian, LOW, LOW);

  // Showtime !
  setFeu(feuDroite, HIGH, LOW, LOW);
  delay(300);
  setFeu(feuDroite, LOW, HIGH, LOW);
  delay(300);
  setFeu(feuDroite, LOW, LOW, HIGH);
  delay(300);
  setFeu(feuDroite, LOW, LOW, LOW);
  setFeu(feuGauche, HIGH, LOW, LOW);
  delay(300);
  setFeu(feuGauche, LOW, HIGH, LOW);
  delay(300);
  setFeu(feuGauche, LOW, LOW, HIGH);
  delay(300);
  setFeu(feuGauche, LOW, LOW, LOW);
  setPedestrian(pedestrian, HIGH, LOW);
  delay(300);
  setPedestrian(pedestrian, LOW, HIGH);
  delay(300);
  setPedestrian(pedestrian, LOW, LOW);
  delay(300);
}

/**
 * Met à jour un feu routier
 * 
 * @param Feu feu - Le feu à mettre à jour
 * @param int red - L'état rouge du feu
 * @param int yellow - L'état jaune du feu
 * @param int green - L'état vert du feu
 */
void setFeu(Feu &feu, int red, int yellow, int green)
{
  feu.red = red;
  feu.yellow = yellow;
  feu.green = green;

  digitalWrite(feu.pinRed, feu.red);
  digitalWrite(feu.pinYellow, feu.yellow);
  digitalWrite(feu.pinGreen, feu.green);
}

/**
 * Met à jour un feu piéton
 * 
 * @param Pedestrian pedestrian - Le feu pétion à mettre à jour
 * @param int red - L'état du rouge du feu piéton
 * @param int green - L'état du vert du feu piéton
 */
void setPedestrian(Pedestrian &pedestrian, int red, int green)
{
  pedestrian.red = red;
  pedestrian.green = green;

  digitalWrite(pedestrian.pinRed, pedestrian.red);
  digitalWrite(pedestrian.pinGreen, pedestrian.green);
}

/**
 * Retourne la distance mesuré par un capteur en centimètre
 * 
 * @param Distance distance - Le capteur sur lequel la mesure doit s'effectuer
 * 
 * @return long - La distance en centimètre
 */
long getDistance(Distance &distance)
{
  digitalWrite(distance.pinTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(distance.pinTrigger, LOW);

  long measure = pulseIn(distance.pinEcho, HIGH);

  return (long) (measure / 58);
}

/**
 * Est-ce que un capteur detecte quelqu'un ?
 *
 * @param Distance distance - Le capteur sur lequel la mesure doit s'effectuer
 *
 * @return bool
 */
bool detectSomeone(Distance &distance)
{
  int nbDetect = 0;
  int nbLoop = 3;

  for (int i = 0; i < nbLoop; i++) {
    long dist = getDistance(distance);
    if (dist <= DISTANCE_DETECT) {
      nbDetect++;
    }
  }

  if (nbDetect >= nbLoop / 2) {
    return true;
  }

  return false;
}

/**
 * Loop
 */
void loop()
{
  if (digitalRead(MODE_PIN)) {
    detect();

    return;
  }

  warning();
}

/**
 * Pas de detection : tous au vert
 * Detection pieton : tricolore 1/2 orange (1 seconde) puis rouge & feu pieton vert - 3 secondes
 * Detection tricolore 1 : pieton rouge & tricolore 1 vert & tricolore 2 orange (1 seconde) puis rouge - 3 secondes
 * Detection tricolore 2 : pieton rouge & tricolore 2 vert & tricolore 1 orange (1 seconde) puis rouge - 3 secondes
 */
void detect()
{
  if (detectSomeone(distancePedestrian)) {
    setPedestrian(pedestrian, LOW, HIGH);
    setFeu(feuDroite, LOW, HIGH, LOW);
    setFeu(feuGauche, LOW, HIGH, LOW);
    delay(700);
    setFeu(feuDroite, HIGH, LOW, LOW);
    setFeu(feuGauche, HIGH, LOW, LOW);
    delay(700);
    do {
      delay(DELAY_DETECT);
    } while(detectSomeone(distancePedestrian));
  }

  else if (detectSomeone(distanceDroite) || detectSomeone(distanceGauche)) {
    setPedestrian(pedestrian, HIGH, LOW);
    setFeu(feuDroite, LOW, LOW, HIGH);
    setFeu(feuGauche, LOW, LOW, HIGH);
    do {
      delay(DELAY_DETECT);
    } while(detectSomeone(distanceDroite) || detectSomeone(distanceGauche));
  }

  else {
    setPedestrian(pedestrian, LOW, HIGH);
    setFeu(feuGauche, LOW, LOW, HIGH);
    setFeu(feuDroite, LOW, LOW, HIGH);
    delay(DELAY_DETECT);
  }
}

/**
 * Les feux routier clignote en jaune et le feu pieton clignote en vert.
 */
void warning()
{
  setPedestrian(pedestrian, LOW, HIGH);
  setFeu(feuDroite, LOW, HIGH, LOW);
  setFeu(feuGauche, LOW, HIGH, LOW);
  delay(600);
  setPedestrian(pedestrian, LOW, LOW);
  setFeu(feuDroite, LOW, LOW, LOW);
  setFeu(feuGauche, LOW, LOW, LOW);
  delay(600);
}
