//####################################################################################
// Definição da classe Planta (POO)
//####################################################################################
#include <Wire.h>
#include "ClosedCube_HDC1080.h"
#include <avr/pgmspace.h>  // Para utilizar PROGMEM

// Definindo constantes para os nomes das plantas na memória flash
const char planta1[] PROGMEM = "Planta de sombra";
const char planta2[] PROGMEM = "Planta de luz moderada";
const char planta3[] PROGMEM = "Planta de luz intensa";

const char* const nomesPlantas[] PROGMEM = { planta1, planta2, planta3 };

// Estrutura compacta para as configurações de cada planta
struct ConfiguracaoPlanta {
  float tempMin, tempMax;
  uint16_t lumMin, lumMax;  // Mudando para uint16_t para otimizar memória
  float umidadeSoloMin, umidadeSoloMax;
};

// Configurações das plantas armazenadas em PROGMEM
const ConfiguracaoPlanta configuracoes[] PROGMEM = {
  {15.0, 25.0, 100, 300, 200.0, 500.0},    // Planta de sombra
  {18.0, 30.0, 300, 600, 250.0, 700.0},    // Planta de luz moderada
  {20.0, 35.0, 600, 1023, 300.0, 800.0}    // Planta de luz intensa
};

// Classe Planta
class Planta {
  private:
    const char* nome;
    ConfiguracaoPlanta config;

  public:
    // Construtor
    Planta(const char* nome, ConfiguracaoPlanta config) {
      this->nome = nome;
      this->config = config;
    }

    // Getters
    const char* getNome() { return nome; }
    float getTempMin() { return config.tempMin; }
    float getTempMax() { return config.tempMax; }
    uint16_t getLumMin() { return config.lumMin; }
    uint16_t getLumMax() { return config.lumMax; }
    float getUmidadeSoloMin() { return config.umidadeSoloMin; }
    float getUmidadeSoloMax() { return config.umidadeSoloMax; }

    // Verificações das condições
    bool verificarTemp(float temp) {
      return temp >= config.tempMin && temp <= config.tempMax;
    }

    bool verificarLuminosidade(uint16_t luminosidade) {
      return luminosidade >= config.lumMin && luminosidade <= config.lumMax;
    }

    bool verificarUmidadeSolo(float umidadeSolo) {
      return umidadeSolo >= config.umidadeSoloMin && umidadeSolo <= config.umidadeSoloMax;
    }

    bool condicoesAdequadas(float temp, uint16_t luminosidade, float umidadeSolo) {
      return verificarTemp(temp) && verificarLuminosidade(luminosidade) && verificarUmidadeSolo(umidadeSolo);
    }
};

// Lista de plantas
Planta plantas[] = {
  Planta(nomesPlantas[0], configuracoes[0]),
  Planta(nomesPlantas[1], configuracoes[1]),
  Planta(nomesPlantas[2], configuracoes[2])
};

Planta* plantaSelecionada = &plantas[0];  // Inicialmente seleciona a primeira planta

//####################################################################################
// Definição dos pinos e variáveis globais
//####################################################################################
ClosedCube_HDC1080 hdc1080;

const int RelePin = 9;       // Pino ao qual o Módulo Relé está conectado
const int PINO_SENSOR = A3;  // Sensor de umidade do solo
const int LDR = A0;          // Sensor de luminosidade
const int pinLM35 = A2;      // Sensor de temperatura

float tempC = 0.0;           
uint16_t valorldr = 0;       
float umidadeSolo = 0;       
float umidadeAr = 0.0;

const unsigned long INTERVALO_REGA = 5 * 1000; // em milissegundos
unsigned long ultimoTempoLeitura = 0;
unsigned long tempoLigada = 0;      
unsigned long tempoUltimaIrrigacao = 0;  
unsigned long inicioIrrigacao = 0;   
bool bombaLigada = false;

// Cache para armazenar últimas leituras
float ultimoTemp = 0.0;
uint16_t ultimoValorlDr = 0;
float ultimaUmidadeSolo = 0.0;

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
  // Leitura condicional dos sensores
  if (millis() - ultimoTempoLeitura >= INTERVALO_REGA) {
    umidadeSolo = lerUmidadeSolo();
    valorldr = lerLuminosidade();
    tempC = lerTemperatura();
    umidadeAr = lerUmidadeAr();

    // Atualiza cache
    ultimoTemp = tempC;
    ultimoValorlDr = valorldr;
    ultimaUmidadeSolo = umidadeSolo;

    exibirValores();
    ultimoTempoLeitura = millis();
  }

  // Controle de irrigação baseado nas leituras armazenadas
  controlarIrrigacao();
  
  delay(5000);
}

//####################################################################################
// Funções de Controle
//####################################################################################
void selecionarPlanta() {
  Serial.println(F("Selecione o tipo de planta:"));
  for (int i = 0; i < sizeof(plantas) / sizeof(plantas[0]); i++) {
    Serial.print(i + 1);
    Serial.print(F(" - Planta "));
    Serial.println(FPSTR(nomesPlantas[i])); // Utiliza a função FPSTR para strings em PROGMEM
  }

  while (Serial.available() == 0) {}
  int tipoPlanta = Serial.parseInt() - 1;  
  if (tipoPlanta >= 0 && tipoPlanta < sizeof(plantas) / sizeof(plantas[0])) {
    plantaSelecionada = &plantas[tipoPlanta];
    Serial.print(F("Planta "));
    Serial.print(FPSTR(nomesPlantas[tipoPlanta]));
    Serial.println(F(" selecionada."));
  } else {
    Serial.println(F("Seleção inválida. Planta 1 será usada por padrão."));
  }
}

void controlarIrrigacao() {
  bool condicoesAdequadas = plantaSelecionada->condicoesAdequadas(ultimoTemp, ultimoValorlDr, ultimaUmidadeSolo);

  if (condicoesAdequadas && !bombaLigada) {  
    ativarIrrigacao();
  } else if (!condicoesAdequadas && bombaLigada) {
    desativarIrrigacao();
  }
}

void ativarIrrigacao() {
  digitalWrite(RelePin, 0);
  Serial.println(F("Irrigação ativada."));
  inicioIrrigacao = millis();
  bombaLigada = true;
}

void desativarIrrigacao() {
  digitalWrite(RelePin, 1);
  Serial.println(F("Irrigação desativada."));
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

uint16_t lerLuminosidade() {
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
  Serial.println(F("================================"));
  Serial.print(F("Temperatura: "));
  Serial.println(ultimoTemp);
  Serial.print(F("Umidade do ar: "));
  Serial.println(umidadeAr);
  Serial.print(F("Luminosidade: "));
  Serial.println(ultimoValorlDr);
  Serial.print(F("Umidade do solo: "));
  Serial.println(ultimaUmidadeSolo);

  Serial.print(F("Tempo total que a bomba ficou ligada: "));
  Serial.print(tempoLigada / 1000); 
  Serial.println(F(" segundos"));

  if (tempoUltimaIrrigacao > 0) {
    Serial.print(F("Última irrigação há: "));
    Serial.print((millis() - tempoUltimaIrrigacao) / 1000); 
    Serial.println(F(" segundos"));
  } else {
    Serial.println(F("Ainda não houve irrigação."));
  }

  Serial.println(F("================================"));
}
