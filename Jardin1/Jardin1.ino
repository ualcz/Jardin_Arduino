//####################################################################################
// Definição da classe Planta (POO)
//####################################################################################
class Planta {
  private:
    String nome;
    float tempMin, tempMax, umidadeSoloMin, umidadeSoloMax;
    int lumMin, lumMax;

  public:
    // Construtor
    Planta(String nome, float tempMin, float tempMax, int lumMin, int lumMax, float umidadeSoloMin, float umidadeSoloMax) {
      this->nome = nome;
      this->tempMin = tempMin;
      this->tempMax = tempMax;
      this->lumMin = lumMin;
      this->lumMax = lumMax;
      this->umidadeSoloMin = umidadeSoloMin;
      this->umidadeSoloMax = umidadeSoloMax;
    }

    // Getters
    String getNome() { return nome; }
    float getTempMin() { return tempMin; }
    float getTempMax() { return tempMax; }
    int getLumMin() { return lumMin; }
    int getLumMax() { return lumMax; }
    float getUmidadeSoloMin() { return umidadeSoloMin; }
    float getUmidadeSoloMax() { return umidadeSoloMax; }

    // Método para verificar se a temperatura está adequada
    bool verificarTemp(float temp) {
      return temp >= tempMin && temp <= tempMax;
    }

    // Método para verificar se a luminosidade está adequada
    bool verificarLuminosidade(int luminosidade) {
      return luminosidade >= lumMin && luminosidade <= lumMax;
    }

    // Método para verificar se a umidade do solo está adequada
    bool verificarUmidadeSolo(float umidadeSolo) {
      return umidadeSolo >= umidadeSoloMin && umidadeSolo <= umidadeSoloMax;
    }

    // Método para verificar se todas as condições estão adequadas
    bool condicoesAdequadas(float temp, int luminosidade, float umidadeSolo) {
      return verificarTemp(temp) && verificarLuminosidade(luminosidade) && verificarUmidadeSolo(umidadeSolo);
    }
};

//####################################################################################
// Definição das plantas (adicionar novas plantas aqui)
//####################################################################################

Planta plantas[] = {
  Planta("Planta de sombra", 15.0, 25.0, 100, 300, 200.0, 500.0),
  Planta("Planta de luz moderada", 18.0, 30.0, 300, 600, 250.0, 700.0),
  Planta("Planta de luz intensa", 20.0, 35.0, 600, 1023, 300.0, 800.0)
};

Planta plantaSelecionada = plantas[0];  // Inicialmente seleciona a primeira planta

//####################################################################################
// Definição dos pinos e variáveis globais
//####################################################################################
#include <Wire.h>
#include "ClosedCube_HDC1080.h"

ClosedCube_HDC1080 hdc1080;

const int RelePin = 9;       // Pino ao qual o Módulo Relé está conectado
const int PINO_SENSOR = A3;  // Sensor de umidade do solo
const int LDR = A0;          // Sensor de luminosidade
const int pinLM35 = A2;      // Sensor de temperatura

float tempC = 0.0;           // Armazena o valor da temperatura
int valorldr = 0;            // Armazena o valor do LDR
float umidadeSolo = 0;       // Armazena a leitura do sensor de umidade
float umidadeAr = 0.0;

const unsigned long INTERVALO_REGA = 5 * 1000; // em milissegundos
unsigned long ultimoTempoRega = 0;
unsigned long tempoLigada = 0;      // Armazena o tempo total que a bomba ficou ligada
unsigned long tempoUltimaIrrigacao = 0;  // Armazena o tempo da última irrigação
unsigned long inicioIrrigacao = 0;   // Armazena o momento em que a bomba foi ligada
bool bombaLigada = false;            // Estado da bomba

//####################################################################################
// Função de controle do Arduino
//####################################################################################
void setup() {
  Serial.begin(9600);

  hdc1080.begin(0x40);
  Serial.print("Manufacturer ID=0x");
  Serial.println(hdc1080.readManufacturerId(), HEX);
  Serial.print("Device ID=0x");
  Serial.println(hdc1080.readDeviceId(), HEX);

  pinMode(RelePin, OUTPUT);
  pinMode(LDR, INPUT);
  pinMode(PINO_SENSOR, INPUT);
  digitalWrite(RelePin, 1); 

  // Exibe opções para selecionar a planta
  selecionarPlanta();
}

void loop() {
  // Leitura dos sensores
  umidadeSolo = lerUmidadeSolo();
  valorldr = lerLuminosidade();
  tempC = lerTemperatura();
  umidadeAr = lerUmidadeAr();

  // Exibe os valores a cada intervalo definido
  if (millis() - ultimoTempoRega >= INTERVALO_REGA) {
    exibirValores();
    ultimoTempoRega = millis();
  }

  // Controla a irrigação
  controlarIrrigacao();
  
  delay(5000);
}

//####################################################################################
// Funções de Controle
//####################################################################################
void selecionarPlanta() {
  Serial.println("Selecione o tipo de planta:");
  for (int i = 0; i < sizeof(plantas) / sizeof(plantas[0]); i++) {
    Serial.print(i + 1);
    Serial.print(" - Planta ");
    Serial.println(plantas[i].getNome());
  }

  while (Serial.available() == 0) {}
  int tipoPlanta = Serial.parseInt() - 1;  
  if (tipoPlanta >= 0 && tipoPlanta < sizeof(plantas) / sizeof(plantas[0])) {
    plantaSelecionada = plantas[tipoPlanta];
    Serial.print("Planta ");
    Serial.print(plantaSelecionada.getNome());
    Serial.println(" selecionada.");
  } else {
    Serial.println("Seleção inválida. Planta 1 será usada por padrão.");
  }
}

void controlarIrrigacao() {
  bool condicoesAdequadas = plantaSelecionada.condicoesAdequadas(tempC, valorldr, umidadeSolo);

  if (condicoesAdequadas && !bombaLigada) {  
    ativarIrrigacao();
  } else if (!condicoesAdequadas && bombaLigada) {
    desativarIrrigacao();
  }
}

void ativarIrrigacao() {
  digitalWrite(RelePin, 0);
  Serial.println("Irrigação ativada.");
  inicioIrrigacao = millis();
  bombaLigada = true;
}

void desativarIrrigacao() {
  digitalWrite(RelePin, 1);
  Serial.println("Irrigação desativada.");
  tempoLigada += millis() - inicioIrrigacao;
  tempoUltimaIrrigacao = millis();
  bombaLigada = false;
}

//####################################################################################
// Funções para leitura dos sensores
//####################################################################################
float lerTemperatura() {
  return hdc1080.readTemperature();
}

int lerLuminosidade() {
  return analogRead(LDR);
}

float lerUmidadeSolo() {
  return analogRead(PINO_SENSOR);
}

float lerUmidadeAr() {
  return hdc1080.readHumidity();
}

// Função para exibir os valores no monitor serial
void exibirValores() {
  Serial.println("================================");
  Serial.print("Temperatura: ");
  Serial.println(tempC);
  Serial.print("Umidade do ar: ");
  Serial.println(umidadeAr);
  Serial.print("Luminosidade: ");
  Serial.println(valorldr);
  Serial.print("Umidade do solo: ");
  Serial.println(umidadeSolo);

  Serial.print("Tempo total que a bomba ficou ligada: ");
  Serial.print(tempoLigada / 1000); 
  Serial.println(" segundos");

  if (tempoUltimaIrrigacao > 0) {
    Serial.print("Última irrigação há: ");
    Serial.print((millis() - tempoUltimaIrrigacao) / 1000); 
    Serial.println(" segundos");
  } else {
    Serial.println("Ainda não houve irrigação.");
  }

  Serial.println("================================");
}