#include <Wire.h>

#define ADRESSE_MOTEUR 8

#define TRIG_AV_G 2
#define ECHO_AV_G 3
#define TRIG_AV_D 4
#define ECHO_AV_D 5
#define TRIG_AR_G 6
#define ECHO_AR_G 7
#define TRIG_AR_D 10
#define ECHO_AR_D 11

const int SEUIL_CM = 50;
const unsigned long TIMEOUT_US = 30000;

bool obstacleDetecte = false;

void setup() {
  Serial.begin(9600);

  Wire.begin();

  pinMode(TRIG_AV_G, OUTPUT);
  pinMode(ECHO_AV_G, INPUT);
  pinMode(TRIG_AV_D, OUTPUT);
  pinMode(ECHO_AV_D, INPUT);
  pinMode(TRIG_AR_G, OUTPUT);
  pinMode(ECHO_AR_G, INPUT);
  pinMode(TRIG_AR_D, OUTPUT);
  pinMode(ECHO_AR_D, INPUT);

  digitalWrite(TRIG_AV_G, LOW);
  digitalWrite(TRIG_AV_D, LOW);
  digitalWrite(TRIG_AR_G, LOW);
  digitalWrite(TRIG_AR_D, LOW);

  int erreurInit = envoyerEtatObstacle(false);
  if (erreurInit != 0) {
    Serial.print("Erreur I2C: echec transmission initiale (code=");
    Serial.print(erreurInit);
    Serial.println(")");
  }
}

void loop() {
  bool obstacle = detecterObstacle();

  if (obstacle != obstacleDetecte) {
    obstacleDetecte = obstacle;
    int erreurEnvoi = envoyerEtatObstacle(obstacleDetecte);
    if (erreurEnvoi != 0) {
      Serial.print("Erreur I2C: echec envoi etat obstacle (etat=");
      Serial.print(obstacleDetecte ? "O" : "N");
      Serial.print(", code=");
      Serial.print(erreurEnvoi);
      Serial.println(")");
    }
  }

  delay(50);
}

bool detecterObstacle() {
  long distanceAvG = mesurerDistanceCm(TRIG_AV_G, ECHO_AV_G);
  if (distanceAvG > 0 && distanceAvG <= SEUIL_CM) {
    return true;
  }

  long distanceAvD = mesurerDistanceCm(TRIG_AV_D, ECHO_AV_D);
  if (distanceAvD > 0 && distanceAvD <= SEUIL_CM) {
    return true;
  }

  long distanceArG = mesurerDistanceCm(TRIG_AR_G, ECHO_AR_G);
  if (distanceArG > 0 && distanceArG <= SEUIL_CM) {
    return true;
  }

  long distanceArD = mesurerDistanceCm(TRIG_AR_D, ECHO_AR_D);
  return distanceArD > 0 && distanceArD <= SEUIL_CM;
}

long mesurerDistanceCm(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duree = pulseIn(echoPin, HIGH, TIMEOUT_US);
  if (duree == 0) {
    return -1;
  }

  return duree / 58;
}

int envoyerEtatObstacle(bool obstacle) {
  Wire.beginTransmission(ADRESSE_MOTEUR);
  Wire.write(obstacle ? 'O' : 'N');
  return Wire.endTransmission();
}
