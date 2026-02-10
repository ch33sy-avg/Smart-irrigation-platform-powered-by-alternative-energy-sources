#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Сенсорлар мен пиндер
const int rainSensorPin = 2;
const int floatSensor1Pin = 4;
const int floatSensor2Pin = 5;
const int tdsSensorPin = A0;
const int soilMoisturePin = A1;
const int relay1Pin = 6;
const int relay2Pin = 7;
const int servoPin = 3;

// Компоненттер
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myServo;

bool isRaining = false;
bool watering = false;

void setup() {
  Serial.begin(9600);
  lcd.init(); lcd.backlight();

  pinMode(rainSensorPin, INPUT);
  pinMode(floatSensor1Pin, INPUT);
  pinMode(floatSensor2Pin, INPUT);
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  
  myServo.attach(servoPin);
  myServo.write(0); // бастапқы күйі

  lcd.setCursor(0, 0);
  lcd.print("Smart Irrigation");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Сенсор мәндерін оқу
  int rainValue = digitalRead(rainSensorPin);
  int float1 = digitalRead(floatSensor1Pin);
  int float2 = digitalRead(floatSensor2Pin);
  int tdsValue = analogRead(tdsSensorPin);
  int soilValue = analogRead(soilMoisturePin);
  
  float tds_ppm = tdsValue * (5.0 / 1023.0) * 2000.0; // ppm шамамен

  // 1. Жаңбыр және сервомотор
  if (rainValue == LOW) { // LOW = жаңбыр бар
    isRaining = true;
    myServo.write(90);
  } else if (float1 == HIGH) {
    isRaining = false;
    myServo.write(0);
  } else {
    isRaining = false;
    myServo.write(0);
  }

  // 2. Судың сапасы тексеріледі
  if (tds_ppm > 1000) {
    digitalWrite(relay1Pin, HIGH); // Насос 1 іске қосылады
    if (float2 == HIGH) {
      digitalWrite(relay1Pin, LOW); // 2-резервуар толса — тоқтайды
    }
  } else {
    digitalWrite(relay1Pin, LOW);
  }

  // 3. Топырақ ылғалдылығы (% масштабтау)
  float moisturePercent = map(soilValue, 1023, 0, 0, 100);
  if (moisturePercent < 40) {
    digitalWrite(relay2Pin, HIGH);
    watering = true;
  } else if (moisturePercent >= 60) {
    digitalWrite(relay2Pin, LOW);
    watering = false;
  }

  // 4. LCD ақпарат көрсету
  lcd.setCursor(0, 0);
  lcd.print("TD:");
  lcd.print((int)tds_ppm);
  lcd.print(" Rain:");
  lcd.print(isRaining ? "Yes " : "No  ");
 

  lcd.setCursor(0, 1);
  lcd.print("M:");
  lcd.print((int)moisturePercent);
  lcd.print("% ");
  lcd.print(watering ? " Watering" : " Stopped  ");

  delay(1000); // 1 секунд сайын жаңартылады
}
