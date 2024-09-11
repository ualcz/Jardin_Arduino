import time
from pyfirmata import Arduino

# Configuração da placa Arduino
board = Arduino('COM3')  # Substitua 'COM3' pela sua porta correta

# Definição dos pinos
RELE_PIN = 9
PINO_SENSOR = 'a:3:i'
LDR = 'a:0:i'
PIN_LM35 = 'a:2:i'

# Definição da classe Planta
class Planta:
    def __init__(self, nome, temp_min, temp_max, lum_min, lum_max, hum_min, hum_max):
        self.nome = nome
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

# Função para controlar a irrigação com base nas condições da planta
def controlar_irrigacao(planta, temperatura, luminosidade, umidade):
    if planta.condicoes_adequadas(temperatura, luminosidade, umidade):
        board.digital[RELE_PIN].write(0)  # Liga a bomba de irrigação
        return "Irrigação: Ativada", "green"
    else:
        board.digital[RELE_PIN].write(1)  # Desliga a bomba de irrigação
        return "Irrigação: Desativada", "red"
