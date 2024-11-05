#include <Arduino.h>

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

void setup() {
  Serial.begin(9600);
  
  // Configura os pinos
  pinMode(pin_ldr, INPUT);
  pinMode(pin_pot, INPUT);
  
  // Configuração do LED como saída PWM
  ledcSetup(pwm_channel, pwm_freq, pwm_res);
  ledcAttachPin(pin_led, pwm_channel);

  // Inicializa o array de leituras do filtro de média móvel
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;  // Inicializa com valores 0
  }
}

void loop() {
  unsigned long int restart = millis();
  
  if (restart - var >= 500) {
    
    // Leitura do potenciômetro para ajustar o brilho do LED
    int read_pot = analogRead(pin_pot);
    int pwm_value = map(read_pot, 0, 4095, 0, 255);
    ledcWrite(pwm_channel, pwm_value);

    // Leitura do LDR
    int read_ldr = analogRead(pin_ldr);

    // Aplica o filtro de média móvel na leitura do LDR
    total = total - readings[readIndex];      // Subtrai a leitura mais antiga
    readings[readIndex] = read_ldr;           // Substitui pela nova leitura
    total = total + readings[readIndex];      // Adiciona a nova leitura ao total
    readIndex = (readIndex + 1) % numReadings;  // Move o índice circularmente

    // Calcula a média
    average = total / numReadings;

    // Calibração de LUX (substituir os fatores abaixo pela calibração adequada)
    float voltage = (average / 4095.0) * 3.3;   // Converte para tensão (supondo Vcc = 3.3V)
    float resistance_ldr = (3.3 / voltage - 1.0) * 10000;  // Calcula resistência do LDR
    float lux = 500 / (resistance_ldr / 1000);  // Fórmula aproximada para calcular Lux

    // Exibe os valores no Serial Monitor
    Serial.print("Ldr (média): ");
    Serial.print(average);           // Valor suavizado da leitura do LDR
    Serial.print(", Lux: ");
    Serial.print(lux);               // Valor estimado em Lux
    Serial.print(", Pot: ");
    Serial.println(read_pot);        // Valor lido do potenciômetro
    
    var = restart;
  }
}