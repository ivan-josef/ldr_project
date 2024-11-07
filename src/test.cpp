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

// Variáveis para contagem de frequência
int last_read_ldr = 0;      // Última leitura do LDR
unsigned long last_time = 0; // Último tempo da leitura
int frequency_count = 0;     // Contador de mudanças
unsigned long sample_time = 500; // Intervalo de amostragem em ms

void setup()
{
  Serial.begin(9600);
  pinMode(pin_ldr, INPUT);
  pinMode(pin_pot, INPUT);

  // Configuração do led como saída PWM 
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
  if (restart - var >= sample_time) {

    // Lê o valor do potenciômetro
    int read_pot = analogRead(pin_pot);
    int pwm_value = map(read_pot, 0, 4095, 0, 255);
    ledcWrite(pwm_channel, pwm_value);  // Ajusta a luminosidade do LED

    // Lê o valor do LDR
    int read_ldr = analogRead(pin_ldr);

    // Aplica a fórmula de calibração
    int lum = (0.1198 * read_ldr - 43.307); // ou outra fórmula que você usa

    // Filtro de média móvel
    total = total - readings[readIndex];
    readings[readIndex] = lum;
    total = total + readings[readIndex];
    readIndex = (readIndex + 1) % numReadings;
    average = total / numReadings;

    // Contagem de mudanças para frequência
    if (read_ldr != last_read_ldr) {
      frequency_count++; // Incrementa a contagem se houver mudança
      last_read_ldr = read_ldr; // Atualiza a última leitura
    }

    // Envia os dados formatados para o Serial Plotter
    Serial.print("Média:");
    Serial.print(average);
    Serial.print(" ");
    Serial.print("Pot:");
    Serial.print(read_pot);
    Serial.print(" ");
    
    // A cada intervalo de amostragem, envie a frequência
    if (restart - last_time >= 1000) { // a cada segundo
      Serial.print("Frequência:");
      Serial.println(frequency_count); // Frequência em mudanças por segundo
      frequency_count = 0; // Reseta o contador
      last_time = restart; // Atualiza o último tempo
    }

    var = restart;
  }
}