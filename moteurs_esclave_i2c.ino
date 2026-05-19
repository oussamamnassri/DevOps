#include <Wire.h>

#define ADRESSE_MOTEUR 8

// Module L298N 1
#define ENA1 3
#define IN1_1 12
#define IN2_1 11

#define ENB1 9
#define IN3_1 13
#define IN4_1 8

// Module L298N 2
#define ENA2 6
#define IN1_2 4
#define IN2_2 5

#define ENB2 10
#define IN3_2 7
#define IN4_2 2

int vitesse = 200;

char commande = 'S';
char ancienneCommande = 'X';

volatile bool obstacleDetecte = false;
bool robotBloque = false;

void setup() {
  Serial.begin(9600);

  Wire.begin(ADRESSE_MOTEUR);
  Wire.onReceive(recevoirI2C);

  pinMode(ENA1, OUTPUT);
  pinMode(IN1_1, OUTPUT);
  pinMode(IN2_1, OUTPUT);

  pinMode(ENB1, OUTPUT);
  pinMode(IN3_1, OUTPUT);
  pinMode(IN4_1, OUTPUT);

  pinMode(ENA2, OUTPUT);
  pinMode(IN1_2, OUTPUT);
  pinMode(IN2_2, OUTPUT);

  pinMode(ENB2, OUTPUT);
  pinMode(IN3_2, OUTPUT);
  pinMode(IN4_2, OUTPUT);

  stopMoteurs();

  Serial.println("Arduino moteurs pret");
  Serial.println("Commandes : F=avant, B=arriere, L=gauche, R=droite, S=stop");
  Serial.println("Vitesses : 1=100, 2=150, 3=200, 4=255");
}

void loop() {
  lireCommandeUtilisateur();

  if (obstacleDetecte) {
    stopMoteurs();

    if (!robotBloque) {
      Serial.println("Obstacle detecte <= 50 cm : ROBOT STOP");
      robotBloque = true;
    }

    return;
  }

  if (robotBloque) {
    Serial.println("Obstacle disparu : le robot continue");
    robotBloque = false;
    ancienneCommande = 'X';
  }

  executerCommande();
}

void lireCommandeUtilisateur() {
  if (Serial.available() > 0) {
    char c = Serial.read();

    if (c == 'F' || c == 'f') {
      commande = 'F';
    } else if (c == 'B' || c == 'b') {
      commande = 'B';
    } else if (c == 'L' || c == 'l') {
      commande = 'L';
    } else if (c == 'R' || c == 'r') {
      commande = 'R';
    } else if (c == 'S' || c == 's') {
      commande = 'S';
    } else if (c == '1') {
      vitesse = 100;
      ancienneCommande = 'X';
      Serial.println("Vitesse = 100");
    } else if (c == '2') {
      vitesse = 150;
      ancienneCommande = 'X';
      Serial.println("Vitesse = 150");
    } else if (c == '3') {
      vitesse = 200;
      ancienneCommande = 'X';
      Serial.println("Vitesse = 200");
    } else if (c == '4') {
      vitesse = 255;
      ancienneCommande = 'X';
      Serial.println("Vitesse = 255");
    }
  }
}

void recevoirI2C(int nombreOctets) {
  (void)nombreOctets;
  while (Wire.available()) {
    char data = Wire.read();

    if (data == 'O') {
      obstacleDetecte = true;
    } else if (data == 'N') {
      obstacleDetecte = false;
    }
  }
}

void executerCommande() {
  if (commande != ancienneCommande) {
    ancienneCommande = commande;

    Serial.print("Commande executee : ");
    Serial.println(commande);

    if (commande == 'F') {
      avant();
    } else if (commande == 'B') {
      arriere();
    } else if (commande == 'L') {
      tournerGauche();
    } else if (commande == 'R') {
      tournerDroite();
    } else if (commande == 'S') {
      stopMoteurs();
    }
  }
}

void avant() {
  digitalWrite(IN1_1, LOW);
  digitalWrite(IN2_1, HIGH);
  analogWrite(ENA1, vitesse);

  digitalWrite(IN3_1, HIGH);
  digitalWrite(IN4_1, LOW);
  analogWrite(ENB1, vitesse);

  digitalWrite(IN1_2, HIGH);
  digitalWrite(IN2_2, LOW);
  analogWrite(ENA2, vitesse);

  digitalWrite(IN3_2, HIGH);
  digitalWrite(IN4_2, LOW);
  analogWrite(ENB2, vitesse);
}

void arriere() {
  digitalWrite(IN1_1, HIGH);
  digitalWrite(IN2_1, LOW);
  analogWrite(ENA1, vitesse);

  digitalWrite(IN3_1, LOW);
  digitalWrite(IN4_1, HIGH);
  analogWrite(ENB1, vitesse);

  digitalWrite(IN1_2, LOW);
  digitalWrite(IN2_2, HIGH);
  analogWrite(ENA2, vitesse);

  digitalWrite(IN3_2, LOW);
  digitalWrite(IN4_2, HIGH);
  analogWrite(ENB2, vitesse);
}

void tournerGauche() {
  // Module 1 en arriere
  digitalWrite(IN1_1, HIGH);
  digitalWrite(IN2_1, LOW);
  analogWrite(ENA1, vitesse);

  digitalWrite(IN3_1, LOW);
  digitalWrite(IN4_1, HIGH);
  analogWrite(ENB1, vitesse);

  // Module 2 en avant
  digitalWrite(IN1_2, HIGH);
  digitalWrite(IN2_2, LOW);
  analogWrite(ENA2, vitesse);

  digitalWrite(IN3_2, HIGH);
  digitalWrite(IN4_2, LOW);
  analogWrite(ENB2, vitesse);
}

void tournerDroite() {
  // Module 1 en avant
  digitalWrite(IN1_1, LOW);
  digitalWrite(IN2_1, HIGH);
  analogWrite(ENA1, vitesse);

  digitalWrite(IN3_1, HIGH);
  digitalWrite(IN4_1, LOW);
  analogWrite(ENB1, vitesse);

  // Module 2 en arriere
  digitalWrite(IN1_2, LOW);
  digitalWrite(IN2_2, HIGH);
  analogWrite(ENA2, vitesse);

  digitalWrite(IN3_2, LOW);
  digitalWrite(IN4_2, HIGH);
  analogWrite(ENB2, vitesse);
}

void stopMoteurs() {
  analogWrite(ENA1, 0);
  analogWrite(ENB1, 0);
  analogWrite(ENA2, 0);
  analogWrite(ENB2, 0);

  digitalWrite(IN1_1, LOW);
  digitalWrite(IN2_1, LOW);

  digitalWrite(IN3_1, LOW);
  digitalWrite(IN4_1, LOW);

  digitalWrite(IN1_2, LOW);
  digitalWrite(IN2_2, LOW);

  digitalWrite(IN3_2, LOW);
  digitalWrite(IN4_2, LOW);
}
