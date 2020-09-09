/*
* Desenvolvido por Gutemberg Praia
*/

#include <Servo.h>
#include <LiquidCrystal.h>

#define TOTAL_GARAGEM  3   //Quantidade total de garagens
#define TOTAL_PORTAO   2   //Quantidade total de Portões de entrada e saida

LiquidCrystal lcd(13, 12, 11, 8, 1, 0);

unsigned long currentMillis;

int iPortao = 0;
int iGaragem = 0;

//Estrutura garagem
typedef struct
{
   int iLedGreen; //porta led  verde
   int iLedRed;   //porta led Vermelho
   int iSensor;   //porta sensor
   int iDistMin;  //distancia minima de acionamento
   int iValue;    //valor da ultima leitura
} garagem_type;

//Iniciar vetor garagem
garagem_type garagem[3]={
  {2,3,A2,500,0},
  {4,5,A3,500,0},
  {6,7,A4,500,0}
};

int iGaragemDisp = TOTAL_GARAGEM; //Quantidade de vaga disponivel

//Inicia Vetor de servo
Servo sPortao[TOTAL_PORTAO];

//Estrutura da portão
typedef struct
{
   int iServoId;   //id-Servo
   int iTipo;     //0-entrada 1 -saida
   int iSensor;   //porta do sensor
   int iDistMin;  //distancia minima de acionamento
   int iValue;    //valor da ultima leitura
   int iServo;    //porta do servo
   int iMsIni;   //micro segundos de aciomaneto inicial
   int iMsFin;   //micro segundos de aciomaneto Final
   int iStatus;   //0-Close  1-Open
   unsigned long previousMillis; //tempo da ultima execução
   const long interval; //Tempo de espera ate o fechamento em millisegundos

} gate_type;

//Iniciar vetor portão de entrada/saida
gate_type gate[2]={
  {0,0,A0,300,0,1500,300,0,0,0,5000},
  {1,1,A1,500,0,1500,300,0,0,0,5000}
};


//Função resposavel pela leitura dos sensores
void readSensorPortao (){
  gate[iPortao].iValue = analogRead(gate[iPortao].iSensor);
  
  if(gate[iPortao].iValue < gate[iPortao].iDistMin && ((gate[iPortao].iTipo==0 && iGaragemDisp > 0)||(gate[iPortao].iTipo==1 && (iGaragemDisp >= 0 && iGaragemDisp < TOTAL_GARAGEM)))){
  	sPortao[gate[iPortao].iServoId].writeMicroseconds(gate[iPortao].iMsFin);
    gate[iPortao].previousMillis = currentMillis;
  	gate[iPortao].iStatus = 1;
  }else{
    if (currentMillis - gate[iPortao].previousMillis >= gate[iPortao].interval) {
  	  sPortao[gate[iPortao].iServoId].writeMicroseconds(gate[iPortao].iMsIni);
      if(gate[iPortao].iStatus == 1){
    	  gate[iPortao].iStatus = 0;
          if(gate[iPortao].iTipo == 1){
            iGaragemDisp+=1;
          }else{
            iGaragemDisp-=1;
          }
      	printDisplay(2);
      }
    }
  }
}

void readSensorGaragem (){
  garagem[iGaragem].iValue = analogRead(garagem[iGaragem].iSensor);
    if(garagem[iGaragem].iValue < garagem[iGaragem].iDistMin){
      digitalWrite(garagem[iGaragem].iLedGreen, HIGH);
      digitalWrite(garagem[iGaragem].iLedRed, LOW);
    }else{
      digitalWrite(garagem[iGaragem].iLedGreen, LOW);
      digitalWrite(garagem[iGaragem].iLedRed, HIGH);
    }
}

/*
* Função resposavel pela escrita no dispaly
*  Paramentros de entrada: 1 - Primeira escrita - escreve todos os dados
*                          2 - Atualização - atualiza a quantidade de vagas 
*/
void printDisplay(int iStatus){
  switch (iStatus) {
    case 1:
        lcd.begin(16, 2);
        lcd.print("Seja Bem vindo!");
        lcd.setCursor(0, 1);
        lcd.print("Vagas Disp: 0");
        lcd.setCursor(13, 1);
        lcd.print(iGaragemDisp);
      break;
    case 2:
        lcd.setCursor(13, 1);
        lcd.print(iGaragemDisp);
      break;
  }
}

void updateConts(){
 
  if(iPortao == TOTAL_PORTAO-1){
  	iPortao = 0;
  }else{
    iPortao += 1;
  }

  if(iGaragem == TOTAL_GARAGEM-1){
  	iGaragem = 0;
  }else{
    iGaragem += 1;
  }

}



void setup() {
  //Serial.begin(115200);

  //definir pinos dos leds como pino de saida
  for (int i = 0; i < TOTAL_GARAGEM; i++) {
    pinMode(garagem[i].iLedGreen, OUTPUT);
    pinMode(garagem[i].iLedRed, OUTPUT);
    
     //Verifica se possui algum carro na garagem ao iniciar o sistema
    garagem[i].iValue = analogRead(garagem[i].iSensor);
    if(garagem[i].iValue < garagem[i].iDistMin){
      iGaragemDisp-=1;
    }
  }

  //iniciar servos
  for (int i = 0; i < TOTAL_PORTAO; i++) {
    sPortao[gate[i].iServoId].attach(gate[i].iServo);
    sPortao[gate[i].iServoId].write(gate[i].iMsIni);
    delay(10);
  }
 
  printDisplay(1);
}

void loop() {

  currentMillis = millis();
  
  readSensorPortao();
  readSensorGaragem ();
  
  updateConts();
  
  delay(10);
  
}
