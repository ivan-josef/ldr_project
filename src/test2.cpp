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

// Variáveis para armazenar as leituras finais
const int numLeiturasMax = 200;  // Quantidade de leituras a serem feitas antes de encerrar
int leiturasAverage[numLeiturasMax];
int leituraAtual = 0;

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
  if (leituraAtual < numLeiturasMax) {  // Verifica se ainda não atingiu o número máximo de leituras
    unsigned long int restart = millis();
    if (restart - var >= 6) {  // Intervalo de 100 ms

      // Lê o valor do potenciômetro e ajusta a luminosidade do LED
      int read_pot = analogRead(pin_pot);
      int pwm_value = map(read_pot, 0, 4095, 0, 255);
      ledcWrite(pwm_channel, pwm_value);

      // Lê o valor do LDR
      int read_ldr = analogRead(pin_ldr);
      int lum = (0.1198 * read_ldr - 43.307);  // Eq linear (melhor leitura)

      // Filtro de média móvel
      total = total - readings[readIndex];  // Subtrai a leitura mais antiga
      readings[readIndex] = lum;            // Substitui pela nova leitura
      total = total + readings[readIndex];  // Adiciona a nova leitura ao total
      readIndex = (readIndex + 1) % numReadings;  // Avança para o próximo índice

      // Calcula a média
      average = total / numReadings;

      // Armazena a leitura da média no array de leituras
      leiturasAverage[leituraAtual] = average;
      leituraAtual++;  // Incrementa o contador de leituras

      var = restart;
    }
  } else {
    // Imprime todas as leituras no final e encerra o código
    Serial.println("Leituras do average:");
    for (int i = 0; i < numLeiturasMax; i++) {
      Serial.print("Leitura ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(leiturasAverage[i]);
    }

    while (true);  // Entra em loop infinito para parar a execução
  }
}