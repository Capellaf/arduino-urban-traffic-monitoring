#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <DS1307.h>
#include <string.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
DS1307 rtc(A4, A5);

#define trigger 9
#define echo 10
#define reset 13
float distancia;
float tempo;
int carros = 0;
int botao = 0;
int amostras[100];
int contador = 0;
int i;
int amostras_positivas = 0;
String dataR = "";

void setup() {
  rtc.halt(false);
  rtc.setSQWRate(SQW_RATE_1);
  rtc.enableSQW(true);
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(reset, INPUT);
  digitalWrite(reset, LOW);
  pinMode(trigger, OUTPUT);
  digitalWrite(trigger, LOW);
  pinMode(echo, INPUT);
  carros = EEPROM.read(0);
  for (int x = 1; x<11; x++) {
  dataR+=char(EEPROM.read(x));
 }
}

void loop() {
  Serial.begin(9600);
  contador = 0;
  amostras_positivas = 0;
  botao = digitalRead(reset);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Carros: ");
  lcd.print(carros, DEC);
  lcd.setCursor(0, 1);
  lcd.print("Desde:");
  lcd.print(dataR);
  if (botao == HIGH)
  {
    carros = 0;
    dataR = rtc.getDateStr();
    for (int x = 1; x<11; x++){
    EEPROM.write(x, dataR[x-1]);
    }
    EEPROM.write(0, carros);
  }
  else
  {
    digitalWrite(trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);
    tempo = pulseIn(echo, HIGH);
    distancia = tempo / 29.4 / 2;
    while (distancia > 100 && distancia < 300)
    {
      digitalWrite(trigger, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigger, LOW);
      tempo = pulseIn(echo, HIGH);
      distancia = tempo / 29.4 / 2;
      if (distancia > 100 && distancia < 300)
      {
        amostras[contador] = 1;
      } else
      {
        amostras[contador] = 0;
        delay(200);
        digitalWrite(trigger, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigger, LOW);
        tempo = pulseIn(echo, HIGH);
        distancia = tempo / 29.4 / 2;
      }
      if (contador < 100)
      {
        contador++;
      }
      Serial.print(contador);
       Serial.print("\n");
      digitalWrite(trigger, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigger, LOW);
      tempo = pulseIn(echo, HIGH);
      distancia = tempo / 29.4 / 2;
      delay(100);
    }
    for (i = 0; i <= contador; i++)
    {
      if (amostras[i] == 1)
      {
        amostras_positivas++;
        Serial.print(amostras_positivas);
      }
    }
    if (amostras_positivas >= 4)
    {
      carros++;
      EEPROM.write(0, carros);
    }
  }
  delay(100);
}
