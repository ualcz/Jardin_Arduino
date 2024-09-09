//####################################################################################
//Criação de clase
//####################################################################################

// Definição da classe Planta (POO)
class Planta {
  public:
    // Atributos (dados)
    float limiteTempMin;
    float limiteTempMax;
    int limiteLuminosidadeMin;
    int limiteLuminosidadeMax;
    float limiteHumidadeSoloMin;
    float limiteHumidadeSoloMax;

    // Construtor (para inicializar o objeto Planta)
    Planta(float tempMin, float tempMax, int lumMin, int lumMax, float HumMin , float HumMax) {
      limiteTempMin = tempMin;
      limiteTempMax = tempMax;
      limiteLuminosidadeMin = lumMin;
      limiteLuminosidadeMax = lumMax;
      limiteHumidadeSoloMin = HumMin;
      limiteHumidadeSoloMax = HumMax;
    }

    // Método para verificar se as condições estão adequadas
    bool condicoesAdequadas(float temperatura, int luminosidade, float humidade) {
      return (temperatura >= limiteTempMin && temperatura <= limiteTempMax &&
              luminosidade >= limiteLuminosidadeMin && luminosidade <= limiteLuminosidadeMax  &&
              humidade <=limiteHumidadeSoloMin &&  humidade <= limiteHumidadeSoloMax );
    }
};


// Criação dos objetos planta Definição das plantas (tempMin, tempMax, lumMin, lunMax)
Planta planta1(18.0, 25.0, 100, 300, 200.2, 500.0);   // Planta que gosta de sombra
Planta planta2(20.0, 28.0, 300, 600, 200.2, 500.0);   // Planta que gosta de luz moderada
Planta planta3(25.0, 35.0, 600, 1023, 200.2, 500.0);  // Planta que gosta de luz intensa

Planta plantaSelecionada = planta1; // Inicialmente, planta1 é selecionada 


//####################################################################################
//Logo abaixo o controle de portas e função de inicialização e set do arduno 
//####################################################################################

// Definições dos pinos
const int RelePin = 9;       // Pino ao qual o Módulo Relé está conectado
const int PINO_SENSOR = A3;  // Sensor de umidade do solo
const int LDR = A0;          // Sensor de luminosidade
const int pinLM35 = A2;      // Sensor de temperatura

// Variáveis
float tempC = 0.0;          // Armazena o valor da temperatura
int valorldr = 0;           // Armazena o valor do LDR
float leituraUmidade = 0;   // Armazena a leitura do sensor de umidade

const unsigned long INTERVALO_REGA = 60 * 1000; // 8 horas em milissegundos
unsigned long ultimoTempoRega = 0; // Armazena o último tempo em que a rega foi ativada

void setup() {
  Serial.begin(9600); // Inicializa a comunicação serial
  pinMode(RelePin, OUTPUT); // Configura o pino do relé como saída
  pinMode(LDR, INPUT);      // Configura o pino LDR como entrada
  pinMode(PINO_SENSOR, INPUT); // Configura o pino do sensor de umidade como entrada
  digitalWrite(RelePin, LOW); // Relé desligado por padrão

  // Pergunta qual planta o usuário quer selecionar
  Serial.println("Selecione o tipo de planta:");
  Serial.println("1 - Planta 1 (Sombra e temperaturas baixas)");
  Serial.println("2 - Planta 2 (Sol e temperaturas médias)");
  Serial.println("3 - Planta 3 (Sol pleno e temperaturas altas)");

  // Aguarda a seleção do usuário
  while (Serial.available() == 0) {}
  int tipoPlanta = Serial.parseInt(); // Lê o valor digitado
  selecionarPlanta(tipoPlanta); // Seleciona a planta

  delay(1000); // Pequeno atraso para estabilidade
}

void loop() {
  // Leitura dos sensores
  leituraUmidade = lerUmidadeSolo();   // Leitura do sensor de umidade do solo
  valorldr = lerLuminosidade();        // Leitura do sensor de luminosidade
  tempC = lerTemperatura();            // Leitura do sensor de temperatura

  // Exibe os valores no monitor serial
  unsigned long atualTempo = millis();
  if (atualTempo - ultimoTempoRega >= INTERVALO_REGA) {
    exibirValores();
    ultimoTempoRega = atualTempo;
  }
  Serial.println("Ok");
  // Verifica se as condições de temperatura e luminosidade estão adequadas para a planta selecionada
  bool condicoesAdequadas = plantaSelecionada.condicoesAdequadas(tempC, valorldr, leituraUmidade);

  // Verifica se as condições são adequadas
  if (condicoesAdequadas) {
    
    digitalWrite(RelePin, HIGH);  // Liga a bomba de irrigação
    Serial.println("Irrigação ativada.");

  } else {

    digitalWrite(RelePin, LOW);   // Desliga a bomba de irrigação
    Serial.println("Irrigação desativada.");

  }

  delay(5000); // Aguarda 5 segundos antes de repetir
}


//########################################
//Logo abaixo estão as função
//########################################

// Função para selecionar o tipo de planta com base na entrada do usuário
void selecionarPlanta(int tipoPlanta) {
  switch (tipoPlanta) {
    case 1:
      plantaSelecionada = planta1;
      Serial.println("Planta 1 selecionada: Gosta de sombra e temperaturas baixas.");
      break;

    case 2:
      plantaSelecionada = planta2;
      Serial.println("Planta 2 selecionada: Gosta de sol e temperaturas médias.");
      break;

    case 3:
      plantaSelecionada = planta3;
      Serial.println("Planta 3 selecionada: Gosta de sol pleno e temperaturas altas.");
      break;

    default:
      Serial.println("Tipo de planta inválido.");
      break;
  }
}

// Função para ler a temperatura
float lerTemperatura() {
  int leitura = analogRead(pinLM35);
  // Converte o valor da temperatura 
  float temp = leitura  ;
  return temp;
  
}

// Função para ler a luminosidade
int lerLuminosidade() {
  int valorLdr = analogRead(LDR);
  return valorLdr;
}

// Função para ler a umidade do solo
float lerUmidadeSolo() {
  float umidade = analogRead(PINO_SENSOR);
  return umidade;
}

// Função para exibir os valores no monitor serial
void exibirValores() {
  Serial.print("Temperatura: ");
  Serial.println(tempC);

  Serial.print("Luminosidade: ");
  Serial.println(valorldr);

  Serial.print("Umidade do solo: ");
  Serial.println(leituraUmidade);
}