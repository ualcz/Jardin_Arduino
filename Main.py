from flask import Flask, jsonify, render_template
import serial
import random
import time
import threading

app = Flask(__name__)


dados_sensor = {"temperatura": 0, "luminosidade": 0, "umidade": 0}
# Configurações da porta serial
'''
ser = serial.Serial('COM3', 9600, timeout=1)  # Substitua 'COM3' pela sua porta serial correta

# Função para ler dados do Arduino
def ler_dados_arduino():
    global dados_sensor
    while True:
        if ser.in_waiting > 0:
            linha = ser.readline().decode('utf-8').strip()
            if "Temperatura" in linha:
                try:
                    valor_temp = int(linha.split(':')[1].strip())
                    dados_sensor['temperatura'] = valor_temp
                except:
                    pass
            elif "Luminosidade" in linha:
                try:
                    valor_lum = int(linha.split(':')[1].strip())
                    dados_sensor['luminosidade'] = valor_lum
                except:
                    pass
            elif "Umidade do solo" in linha:
                try:
                    valor_umid = int(linha.split(':')[1].strip())
                    dados_sensor['umidade'] = valor_umid
                except:
                    pass
'''

# Função para simular a geração de dados do "Arduino"
def simular_dados_arduino():
    global dados_sensor
    while True:
        # Gera valores aleatórios simulando as leituras dos sensores
        dados_sensor['temperatura'] = random.randint(18, 35)  # Temperatura entre 18°C e 35°C
        dados_sensor['luminosidade'] = random.randint(100, 1023)  # Luminosidade entre 100 e 1023
        dados_sensor['umidade'] = random.randint(200, 800)  # Umidade entre 200 e 800

        # Imita o comportamento de atualização de dados a cada 5 segundos
        time.sleep(1)

# Thread para leitura contínua dos dados da serial
thread = threading.Thread(target=simular_dados_arduino)
#thread = threading.Thread(target=ler_dados_arduino)
thread.daemon = True
thread.start()

@app.route('/')
def index():
    return render_template('index.html', dados=dados_sensor)

@app.route('/dados')
def obter_dados():
    return jsonify(dados_sensor)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
