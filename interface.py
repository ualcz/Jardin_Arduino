import tkinter as tk
from tkinter import messagebox
from sistema_planta import Planta, ler_temperatura, ler_luminosidade, ler_umidade_solo, controlar_irrigacao

# Inicialização das plantas
plantas = [
    Planta("Planta Sombra", 0.0, 25.0, 100, 300, 200.2, 500.0),
    Planta("Planta Luz Moderada", 0.0, 28.0, 300, 600, 200.2, 900.0),
    Planta("Planta Luz Intensa", 0.0, 35.0, 600, 1023, 200.2, 500.0)
]
planta_selecionada = plantas[0]

# Função para cadastrar nova planta
def cadastrar_planta():
    nome = entry_nome.get()
    temp_min = float(entry_temp_min.get())
    temp_max = float(entry_temp_max.get())
    lum_min = int(entry_lum_min.get())
    lum_max = int(entry_lum_max.get())
    hum_min = float(entry_hum_min.get())
    hum_max = float(entry_hum_max.get())

    nova_planta = Planta(nome, temp_min, temp_max, lum_min, lum_max, hum_min, hum_max)
    plantas.append(nova_planta)
    planta_menu['menu'].add_command(label=nome, command=tk._setit(planta_var, nome))
    messagebox.showinfo("Sucesso", f"Planta '{nome}' cadastrada com sucesso!")

# Função para atualizar os dados dos sensores na interface
def atualizar_dados():
    temperatura = ler_temperatura()
    luminosidade = ler_luminosidade()
    umidade = ler_umidade_solo()

    # Atualizar os valores na interface
    label_temp_val.config(text=f"{temperatura:.2f} °C")
    label_lum_val.config(text=f"{luminosidade:.2f}")
    label_hum_val.config(text=f"{umidade:.2f}")

    # Verificar condições da planta selecionada
    for planta in plantas:
        if planta.nome == planta_var.get():
            planta_selecionada = planta

    # Controlar irrigação e atualizar o status
    status_irrigacao, cor_irrigacao = controlar_irrigacao(planta_selecionada, temperatura, luminosidade, umidade)
    label_irrigacao.config(text=status_irrigacao, fg=cor_irrigacao)

    # Atualizar a cada 5 segundos
    janela.after(5000, atualizar_dados)

# Configuração da interface gráfica
janela = tk.Tk()
janela.title("Monitoramento da Planta")

# Seleção de planta
planta_var = tk.StringVar(janela)
planta_var.set(plantas[0].nome)
planta_menu = tk.OptionMenu(janela, planta_var, *[planta.nome for planta in plantas])
planta_menu.grid(row=0, column=1)

tk.Label(janela, text="Planta Selecionada:").grid(row=0, column=0)

# Exibição dos valores dos sensores
tk.Label(janela, text="Temperatura:").grid(row=1, column=0)
label_temp_val = tk.Label(janela, text="0.00 °C")
label_temp_val.grid(row=1, column=1)

tk.Label(janela, text="Luminosidade:").grid(row=2, column=0)
label_lum_val = tk.Label(janela, text="0.00")
label_lum_val.grid(row=2, column=1)

tk.Label(janela, text="Umidade do Solo:").grid(row=3, column=0)
label_hum_val = tk.Label(janela, text="0.00")
label_hum_val.grid(row=3, column=1)

# Exibir status da irrigação
label_irrigacao = tk.Label(janela, text="Irrigação: Desativada", fg="red")
label_irrigacao.grid(row=4, column=0, columnspan=2)

# Cadastro de novas plantas
tk.Label(janela, text="Cadastro de Nova Planta").grid(row=5, column=0, columnspan=2)

tk.Label(janela, text="Nome:").grid(row=6, column=0)
entry_nome = tk.Entry(janela)
entry_nome.grid(row=6, column=1)

tk.Label(janela, text="Temp Min:").grid(row=7, column=0)
entry_temp_min = tk.Entry(janela)
entry_temp_min.grid(row=7, column=1)

tk.Label(janela, text="Temp Max:").grid(row=8, column=0)
entry_temp_max = tk.Entry(janela)
entry_temp_max.grid(row=8, column=1)

tk.Label(janela, text="Lum Min:").grid(row=9, column=0)
entry_lum_min = tk.Entry(janela)
entry_lum_min.grid(row=9, column=1)

tk.Label(janela, text="Lum Max:").grid(row=10, column=0)
entry_lum_max = tk.Entry(janela)
entry_lum_max.grid(row=10, column=1)

tk.Label(janela, text="Hum Min:").grid(row=11, column=0)
entry_hum_min = tk.Entry(janela)
entry_hum_min.grid(row=11, column=1)

tk.Label(janela, text="Hum Max:").grid(row=12, column=0)
entry_hum_max = tk.Entry(janela)
entry_hum_max.grid(row=12, column=1)

tk.Button(janela, text="Cadastrar Planta", command=cadastrar_planta).grid(row=13, column=0, columnspan=2)

# Iniciar atualização de dados
atualizar_dados()

# Iniciar loop da interface
janela.mainloop()
