#include <Arduino.h>
#include <math.h>

// definição dos pinos
#define pin_ldr 36 
#define pin_led 23
#define pin_pot 35

// Configuração do PWM
#define pwm_channel 0  // usando o canal pwm 0
#define pwm_freq 1000  // Frequência do PWM (1khz)
#define pwm_res 8      // Resolução do PWM (8 bits -> valores de 0 a 255)
int var = 0;

// Parâmetros do filtro de média móvel
const int numReadings = 10;  // Quantidade de leituras para a média
int readings[numReadings];   // Array para armazenar as leituras
int readIndex = 0;           // Índice da leitura atual
int total = 0;               // Total acumulado das leituras
int average = 0;             // Média das leituras

void setup()
{
  Serial.begin(9600);
  pinMode(pin_ldr, INPUT);
  pinMode(pin_pot, INPUT);

  // Configuração do led como saida pwm 
  ledcSetup(pwm_channel, pwm_freq, pwm_res);
  ledcAttachPin(pin_led, pwm_channel);

  // Inicializa o array de leituras com 0
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }
}

void loop()
{
  unsigned long int restart = millis();
  if (restart - var >= 250) {

    // Lê o valor do potenciômetro
    int read_pot = analogRead(pin_pot);
    int pwm_value = map(read_pot, 0, 4095, 0, 255);
    ledcWrite(pwm_channel, pwm_value);  // Ajusta a luminosidade do LED

    // Lê o valor do LDR
    int read_ldr = analogRead(pin_ldr);
    

    //grafico 1.1
    //int lum = 0.0336*pow(read_ldr,1.0297); //eq potencia
    //int lum = (0.1198*read_ldr - 43.307); //eq linear (melhor leitura)
    //int lum = 5.314*exp(0.0013*read_ldr); //eq exp
    //int lum = 67.564*log(read_ldr - 346.92); //eq ln  

    //grafico 2.2
    //int lum = 0.3697*pow(read_ldr,0.7435); //eq potencia
    //int lum = (0.0834*read_ldr + 1.0181); //eq linear (melhor leitura)
    int lum = 6.1065*exp(0.0036*read_ldr); //eq exp
    //int lum = 13.745*log(read_ldr - 46.198); //eq ln  

  

  
    // Filtro de média móvel
    total = total - readings[readIndex];  // Subtrai a leitura mais antiga
    readings[readIndex] = lum;       // Substitui pela nova leitura
    total = total + readings[readIndex];  // Adiciona a nova leitura ao total
    readIndex = (readIndex + 1) % numReadings;  // Avança para o próximo índice

    // Calcula a média
    average = total / numReadings;

    // Exibe os valores no Serial Monitor
    if (average <= 0){
      Serial.print("Ldr (média):");
      Serial.print(0);
      Serial.print(",");
      Serial.print("Pot:");
      Serial.println(read_pot);
    }
    else{
      Serial.print("Ldr (média):");
      Serial.print(average);
      Serial.print(",");
      Serial.print("Pot:");
      Serial.println(read_pot);
    }

    var = restart;
  }
}