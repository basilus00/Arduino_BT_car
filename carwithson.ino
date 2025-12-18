// -------------------- Motor Pins --------------------
#define IN1  11
#define IN2  12
#define IN3  9
#define IN4  10
#define ENA  5
#define ENB  6

// -------------------- Ultrasonic Pins --------------------
#define TRIG 7
#define ECHO 8

// -------------------- Variables --------------------
char command = 'S';
int btSpeed = 250;       // Speed from Bluetooth
int autoSpeed = 250;     // Speed limited by obstacle
long distance = 0;

// ===================================================
// SETUP
// ===================================================
void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  Serial.begin(9600);
  Serial.println("Bluetooth Robot Ready with Obstacle Detection!");
  stopCar();
}

// ===================================================
// MAIN LOOP
// ===================================================
void loop() {

  // ---------------------------------------------------
  // 1 — OBSTACLE CHECK ONLY IF NOT MOVING BACKWARD
  // ---------------------------------------------------
  if (command != 'B') {
    distance = getDistance();
    autoSpeed = adjustSpeed(distance);
  } else {
    autoSpeed = btSpeed;  // Allow full backward movement
  }

  // ---------------------------------------------------
  // 2 — READ BLUETOOTH COMMANDS
  // ---------------------------------------------------
  if (Serial.available()) {
    command = Serial.read();

    // Speed control (0–9)
    if (command >= '0' && command <= '9') {
      btSpeed = (command - '0') * 28;  // 0→0 , 9→252
      Serial.print("BT Speed set to ");
      Serial.println(btSpeed);
    }

    // Direction commands
    switch (command) {
      case 'F': forward();  break;
      case 'B': backward(); break;
      case 'L': left();     break;
      case 'R': right();    break;
      case 'S': stopCar();  break;
    }
  }
}

// ===================================================
// Ultrasonic Distance Function
// ===================================================
long getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(3);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000); // timeout = 30ms
  long distanceCm = duration * 0.034 / 2;

  return distanceCm;
}

// ===================================================
// Dynamic Speed Control
// ===================================================
int adjustSpeed(long d) {

  if (d == 0 || d > 100) {
    return btSpeed;        // No obstacle
  } 
  else if (d > 40) {
    return btSpeed;        // Normal
  } 
  else if (d > 20) {
    return btSpeed * 0.6;  // Medium slow
  } 
  else if (d > 10) {
    return btSpeed * 0.3;  // Very slow
  } 
  else {
    stopCar();             // Danger zone
    return 0;
  }
}

// ===================================================
// Motor Control
// ===================================================
void forward() {
  digitalWrite(IN1, HIGH);  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);  digitalWrite(IN4, LOW);
  analogWrite(ENA, autoSpeed);
  analogWrite(ENB, autoSpeed);
  Serial.println("Forward");
}

void backward() {
  digitalWrite(IN1, LOW);   digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);   digitalWrite(IN4, HIGH);
  analogWrite(ENA, btSpeed);
  analogWrite(ENB, btSpeed);
  Serial.println("Backward");
}

void left() {
  digitalWrite(IN1, LOW);   digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);  digitalWrite(IN4, LOW);
  analogWrite(ENA, autoSpeed);
  analogWrite(ENB, autoSpeed);
  Serial.println("Left");
}

void right() {
  digitalWrite(IN1, HIGH);  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);   digitalWrite(IN4, HIGH);
  analogWrite(ENA, autoSpeed);
  analogWrite(ENB, autoSpeed);
  Serial.println("Right");
}

void stopCar() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  Serial.println("STOP");
}
