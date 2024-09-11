//####################################################################################
// Definição da classe Planta (POO)
//####################################################################################
class Planta {
  public:
    float limiteTempMin;
    float limiteTempMax;
    int limiteLuminosidadeMin;
    int limiteLuminosidadeMax;
    float limiteHumidadeSoloMin;
    float limiteHumidadeSoloMax;

    // Construtor
    Planta(float tempMin, float tempMax, int lumMin, int lumMax, float humMin, float humMax) {
      limiteTempMin = tempMin;
      limiteTempMax = tempMax;
      limiteLuminosidadeMin = lumMin;
      limiteLuminosidadeMax = lumMax;
      limiteHumidadeSoloMin = humMin;
      limiteHumidadeSoloMax = humMax;
    }

    // Método para verificar se as condições estão adequadas
    bool condicoesAdequadas(float temperatura, int luminosidade, float humidade) {
      return (temperatura >= limiteTempMin && temperatura <= limiteTempMax &&
              luminosidade >= limiteLuminosidadeMin && luminosidade <= limiteLuminosidadeMax &&
              humidade >= limiteHumidadeSoloMin && humidade <= limiteHumidadeSoloMax);
    }
};

//####################################################################################
// Definição das plantas (adicionar novas plantas aqui)
//####################################################################################
Planta plantas[] = {
  Planta(0.0, 25.0, 100, 300, 200.2, 500.0),   // Planta que gosta de sombra
  Planta(0.0, 28.0, 300, 600, 200.2, 900.0),   // Planta que gosta de luz moderada
  Planta(0.0, 35.0, 600, 1023, 200.2, 500.0)   // Planta que gosta de luz intensa
};

Planta plantaSelecionada = plantas[0];  // Planta selecionada

//####################################################################################
// Definição dos pinos e variáveis globais
//####################################################################################


const int RelePin = 9;       // Pino ao qual o Módulo Relé está conectado
const int PINO_SENSOR = A3;  // Sensor de umidade do solo
const int LDR = A0;          // Sensor de luminosidade
const int pinLM35 = A2;      // Sensor de temperatura

float tempC = 0.0;           // Armazena o valor da temperatura
int valorldr = 0;            // Armazena o valor do LDR
float leituraUmidade = 0;    // Armazena a leitura do sensor de umidade

const unsigned long INTERVALO_REGA = 5* 1000; // em milissegundos
unsigned long ultimoTempoRega = 0;

void setup() {
  Serial.begin(9600);
  pinMode(RelePin, OUTPUT);
  pinMode(LDR, INPUT);
  pinMode(PINO_SENSOR, INPUT);
  digitalWrite(RelePin, 1 ); 

  // Exibe opções para selecionar a planta
  Serial.println("Selecione o tipo de planta:");
  for (int i = 0; i < sizeof(plantas) / sizeof(plantas[0]); i++) {
    Serial.print(i + 1);
    Serial.print(" - Planta ");
    Serial.print(i + 1);
    Serial.println();
  }

  // Aguarda a seleção do usuário
  while (Serial.available() == 0) {}
  int tipoPlanta = Serial.parseInt() - 1;  // Ajusta para índice do array
  if (tipoPlanta >= 0 && tipoPlanta < sizeof(plantas) / sizeof(plantas[0])) {
    plantaSelecionada = plantas[tipoPlanta];
    Serial.print("Planta ");
    Serial.print(tipoPlanta + 1);
    Serial.println(" selecionada.");
  } else {
    Serial.println("Seleção inválida. Planta 1 será usada por padrão.");
  }

  delay(1000);  // Pequeno atraso para estabilidade
}

void loop() {
  // Leitura dos sensores
  leituraUmidade = lerUmidadeSolo();
  valorldr = lerLuminosidade();
  tempC = lerTemperatura();

  // Exibe os valores a cada intervalo definido
  unsigned long atualTempo = millis();
  if (atualTempo - ultimoTempoRega >= INTERVALO_REGA) {
    exibirValores();
    ultimoTempoRega = atualTempo;
  }

  // Verifica as condições da planta selecionada
  bool condicoesAdequadas = plantaSelecionada.condicoesAdequadas(tempC, valorldr, leituraUmidade);

  // Controla a irrigação
  if (condicoesAdequadas) {
    digitalWrite(RelePin,0 );  // Liga a bomba de irrigação
    Serial.println("Irrigação ativada.");
  } else {
    digitalWrite(RelePin,1 );   // Desliga a bomba de irrigação
    Serial.println("Irrigação desativada.");
  }

  delay(5000);  // Aguarda 5 segundos antes de repetir
}

// Funções para leitura dos sensores
float lerTemperatura() {
  float leitura = (float(analogRead(pinLM35))*5/(1023))/0.01;
  float temp = leitura;  // Simulação de conversão de temperatura
  return temp;
}

int lerLuminosidade() {
  return analogRead(LDR);
}

float lerUmidadeSolo() {
  return analogRead(PINO_SENSOR);
}

// Função para exibir os valores no monitor serial
void exibirValores() {
  Serial.println("================================");
  Serial.print("Temperatura: ");
  Serial.println(tempC);
  Serial.print("Luminosidade: ");
  Serial.println(valorldr);
  Serial.print("Umidade do solo: ");
  Serial.println(leituraUmidade);
  Serial.println("================================");
}
