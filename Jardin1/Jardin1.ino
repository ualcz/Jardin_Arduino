//####################################################################################
// Definição da classe Planta (POO)
//####################################################################################

class Planta {
  public:
    String nomePlanta;
    float limiteTempMin;
    float limiteTempMax;
    int limiteLuminosidadeMin;
    int limiteLuminosidadeMax;
    float limiteHumidadeSoloMin;
    float limiteHumidadeSoloMax;

    // Construtor
    Planta(String nome, float tempMin, float tempMax, int lumMin, int lumMax, float humMin, float humMax) {
      nomePlanta = nome;
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

    String Exibinome() {
      return nomePlanta;
    }
};

//####################################################################################
// Definição das plantas (adicionar novas plantas aqui)
//####################################################################################


Planta plantas[] = {
  Planta("sombra", 0.0, 25.0, 100, 300, 200.2, 500.0),   // Planta que gosta de sombra
  Planta("luz moderada", 0.0, 28.0, 300, 600, 200.2, 900.0),   // Planta que gosta de luz moderada
  Planta("luz intensa", 0.0, 35.0, 600, 1023, 200.2, 500.0)    // Planta que gosta de luz intensa
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
  pinMode(RelePin, OUTPUT);
  pinMode(LDR, INPUT);
  pinMode(PINO_SENSOR, INPUT);
  digitalWrite(RelePin, 1); 

  // Exibe opções para selecionar a planta
  Serial.println("Selecione o tipo de planta:");
  for (int i = 0; i < sizeof(plantas) / sizeof(plantas[0]); i++) {
    Serial.print(i + 1);
    Serial.print(" - Planta ");
    Serial.print(plantaSelecionada.Exibinome());
    Serial.println();
  }

  // Aguarda a seleção do usuário
  while (Serial.available() == 0) {}
  int tipoPlanta = Serial.parseInt() - 1;  // Ajusta para índice do array
  if (tipoPlanta >= 0 && tipoPlanta < sizeof(plantas) / sizeof(plantas[0])) {
    plantaSelecionada = plantas[tipoPlanta];
    Serial.print("Planta ");
    Serial.print(tipoPlanta + 1);
    Serial.print(plantaSelecionada.Exibinome());
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
    if (!bombaLigada) {  // Verifica se a bomba ainda não está ligada
      digitalWrite(RelePin, 0);      // Liga a bomba de irrigação
      Serial.println("Irrigação ativada.");
      inicioIrrigacao = millis();    // Armazena o tempo que a bomba foi ativada
      bombaLigada = true;
    }
  } else {
    if (bombaLigada) {  // Verifica se a bomba estava ligada
      digitalWrite(RelePin, 1);      // Desliga a bomba de irrigação
      Serial.println("Irrigação desativada.");
      tempoLigada += millis() - inicioIrrigacao;   // Calcula o tempo total que a bomba ficou ligada
      tempoUltimaIrrigacao = millis();             // Armazena o tempo da última irrigação
      bombaLigada = false;
    }
  }

  delay(5000);  // Aguarda 5 segundos antes de repetir
}



//####################################################################################
// Funções para leitura dos sensores
//####################################################################################



float lerTemperatura() {
  int reading = analogRead(pinLM35); 
  float voltage = reading * 5.0 / 1024.0;  
  float tempC = (voltage - 0.5) * 100;
  return tempC;
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

  // Exibe quanto tempo a bomba ficou ligada
  Serial.print("Tempo total que a bomba ficou ligada: ");
  Serial.print(tempoLigada / 1000);   // Converte de milissegundos para segundos
  Serial.println(" segundos");

  // Exibe quando foi a última irrigação
  if (tempoUltimaIrrigacao > 0) {
    Serial.print("Última irrigação há: ");
    Serial.print((millis() - tempoUltimaIrrigacao) / 1000);  // Tempo desde a última irrigação em segundos
    Serial.println(" segundos");
  } else {
    Serial.println("Ainda não houve irrigação.");
  }

  Serial.println("================================");
}