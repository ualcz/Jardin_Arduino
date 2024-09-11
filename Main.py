import time
from pyfirmata import Arduino, util

# Configuração da placa Arduino
board = Arduino('COM3')

# Definição dos pinos
RELE_PIN = 9
PINO_SENSOR = 'a:3:i'
LDR = 'a:0:i'
PIN_LM35 = 'a:2:i'

# Configuração do intervalo de rega
INTERVALO_REGA = 5  # em segundos
ultimo_tempo_rega = time.time()

# Definição da classe Planta
class Planta:
    def __init__(self, nome:str, temp_min:float, temp_max:float, lum_min:int, lum_max:int, hum_min:float, hum_max:float):
        self.nome=nome
        self.limite_temp_min = temp_min
        self.limite_temp_max = temp_max
        self.limite_luminosidade_min = lum_min
        self.limite_luminosidade_max = lum_max
        self.limite_humidade_solo_min = hum_min
        self.limite_humidade_solo_max = hum_max

    def condicoes_adequadas(self, temperatura, luminosidade, humidade):
        return (self.limite_temp_min <= temperatura <= self.limite_temp_max and
                self.limite_luminosidade_min <= luminosidade <= self.limite_luminosidade_max and
                self.limite_humidade_solo_min <= humidade <= self.limite_humidade_solo_max)

# Definição das plantas
plantas = [
    Planta('gosta de sombra',0.0, 25.0, 100, 300, 200.2, 500.0),   # Planta que gosta de sombra
    Planta('gosta de luz moderada',0.0, 28.0, 300, 600, 200.2, 900.0),   # Planta que gosta de luz moderada
    Planta(' gosta de luz intensa',0.0, 35.0, 600, 1023, 200.2, 500.0)   # Planta que gosta de luz intensa
]

# Seleção da planta
planta_selecionada = plantas[0]  # Por padrão, selecionar a primeira planta

# Função para ler a temperatura do sensor LM35
def ler_temperatura():
    leitura = board.analog[PIN_LM35].read()
    if leitura is not None:
        temperatura = leitura * 5 * 100  # Conversão para Celsius
        return temperatura
    return 0

# Função para ler a luminosidade
def ler_luminosidade():
    leitura = board.analog[LDR].read()
    if leitura is not None:
        return leitura * 1023  # Conversão para valor de 0 a 1023
    return 0

# Função para ler a umidade do solo
def ler_umidade_solo():
    leitura = board.analog[PINO_SENSOR].read()
    if leitura is not None:
        return leitura * 1023  # Conversão para valor de 0 a 1023
    return 0

# Função para exibir os valores no console
def exibir_valores(temperatura, luminosidade, umidade):
    print("================================")
    print(f"Temperatura: {temperatura:.2f} °C")
    print(f"Luminosidade: {luminosidade:.2f}")
    print(f"Umidade do solo: {umidade:.2f}")
    print("================================")

# Loop principal
while True:
    temperatura = ler_temperatura()
    luminosidade = ler_luminosidade()
    umidade = ler_umidade_solo()

    # Exibir valores a cada intervalo definido
    tempo_atual = time.time()
    if tempo_atual - ultimo_tempo_rega >= INTERVALO_REGA:
        exibir_valores(temperatura, luminosidade, umidade)
        ultimo_tempo_rega = tempo_atual

    # Verificar as condições da planta selecionada
    condicoes_adequadas = planta_selecionada.condicoes_adequadas(temperatura, luminosidade, umidade)

    # Controlar a irrigação
    if condicoes_adequadas:
        board.digital[RELE_PIN].write(0)  # Liga a bomba de irrigação
        print("Irrigação ativada.")
    else:
        board.digital[RELE_PIN].write(1)  # Desliga a bomba de irrigação
        print("Irrigação desativada.")

    time.sleep(5)  # Aguardar 5 segundos antes de repetir
