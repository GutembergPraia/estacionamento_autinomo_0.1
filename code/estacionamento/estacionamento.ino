#include <Servo.h>
#include <LiquidCrystal.h>

Servo entrada,saida;

LiquidCrystal lcd(13, 12, 11, 8, 1, 0);

unsigned long currentMillis;

typedef struct
{
   int iLedGreen; //pinagem do led  verde
   int iLedRed;   //pinagem do led Vermelho
   int iSensor;   //porta do sensor
   int iDistMin;  //distancia minima de acionamento
   int iValue;    //valor da ultima leitura
} garagem_type;

//Iniciar vetor garagem
garagem_type garagem[3]={
  {2,3,A2,500,0},
  {4,5,A3,500,0},
  {6,7,A4,500,0}
};

const int tamGaragem = sizeof(garagem)/sizeof(garagem_type);

int iGaragemDisp = tamGaragem; //Quantidade de vaga disponivel

typedef struct
{
   int iTipo;     //0-entrada 1 -saida
   int iSensor;   //porta do sensor
   int iDistMin;  //distancia minima de acionamento
   int iValue;    //valor da ultima leitura
   Servo servo;   //Objeto Servo
   int iServo;    //porta do servo
   int iAngIni;   //angulo inicial
   int iAngFin;   //angulo Final
   int iStatus;   //0-StandBy  1-Open 2-Close
   unsigned long previousMillis; //tempo da ultima execução
   const long interval; //Tempo de espera ate o fechamento em millisegundos

} gate_type;

//Iniciar vetor portão
gate_type gate[2]={
  {0,A0,300,0,entrada,9,90,0,0,0,5000},
  {1,A1,500,0,saida,10,90,0,0,0,5000}
};

const int tamGate = sizeof(gate)/sizeof(gate_type);

void readSensor (){
  for (int i = 0; i < tamGate; i++) {
    gate[i].iValue = analogRead(gate[i].iSensor);
    if(gate[i].iValue < gate[i].iDistMin && (iGaragemDisp > 0 || gate[i].iTipo==1)){
      if(gate[i].iStatus == 0){
        gate[i].servo.write(gate[i].iAngFin);
        gate[i].iStatus = 1;
        gate[i].previousMillis = currentMillis;
        delay(10);
      }
    }else{
      if(gate[i].iStatus == 1){
        if (currentMillis - gate[i].previousMillis >= gate[i].interval) {
          gate[i].servo.write(90);
          gate[i].iStatus = 0;
          if(gate[i].iTipo){
            iGaragemDisp+=1;
          }else{
            iGaragemDisp-=1;
          }
          lcd.setCursor(13, 1);
      lcd.print(iGaragemDisp);

          delay(10);

        }
      }
    }
  }

  for (int i = 0; i < tamGaragem; i++) {
    garagem[i].iValue = analogRead(garagem[i].iSensor);
    if(garagem[i].iValue < garagem[i].iDistMin){
      digitalWrite(garagem[i].iLedGreen, HIGH);
      digitalWrite(garagem[i].iLedRed, LOW);
    }else{
      digitalWrite(garagem[i].iLedGreen, LOW);
      digitalWrite(garagem[i].iLedRed, HIGH);
    }
  }
}




void setup() {
  //Serial.begin(115200);

  //definir pinos dos leds como pino de saida
  for (int i = 0; i < tamGaragem; i++) {
    pinMode(garagem[i].iLedGreen, OUTPUT);
    pinMode(garagem[i].iLedRed, OUTPUT);
  }

  //iniciar servos
  for (int i = 0; i < tamGate; i++) {
    gate[i].servo.attach(gate[i].iServo);
    gate[i].servo.write(gate[i].iAngIni);
    delay(10);
  }

  lcd.begin(16, 2);
  lcd.print("Seja Bem vindo!");
  lcd.setCursor(0, 1);
  lcd.print("Vagas Disp: 0");
  lcd.setCursor(13, 1);
  lcd.print(iGaragemDisp);
}

void loop() {

  currentMillis = millis();

  readSensor();
  delay(20);
}
