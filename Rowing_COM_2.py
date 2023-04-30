# -*- coding: utf-8 -*-
"""
Created on Wed Jan 25 22:09:41 2023

@author: javic
"""

#Importamos las librerias necesarias
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure
from tkinter import *
import os
from PIL import Image, ImageTk
import numpy as np
import serial as sr
from matplotlib.lines import Line2D
import collections
import time
from tkinter import ttk


#------Variables globales para las gráficas------
port_selected = False
data=np.array([])
data=[[0],[0],[0]]
cond = False
lines = []
Samples = 100
numData = 2
calibrated = False


#------Definimos las paleta de colores de las ventanas----
fondo_ventana = "#F2F3F4"
fondo_encabezado = "#0f54a1"
fondo_boton = "#2874A6"
letra_boton = "#FBFCFC"
letra_encabezado = letra_boton


#Creamos la función para la detección de los puertos COM.


def puertos_seriales():
    ports = ['COM%s' % (i + 1) for i in range(256)]
    encontrados = []
    for port in ports:
        try:
            s = sr.Serial(port)
            s.close()
            encontrados.append(port)
        except (OSError, sr.SerialException):
            pass
    #print("Puertos encontrados: "+str(encontrados))
    return encontrados


#Creamos la ventana principal

def ventana_inicio():
    global ventana_principal
    global fondo_ventana, fondo_encabezado, fondo_boton, letra_boton, letra_encabezado
    
    ventana_principal = Tk()
    ventana_principal.geometry("300x180")#Dimensiones de la ventana
    ventana_principal.title("Concept 2")#Título de la ventana
    ventana_principal.config(bg="#F2F3F4")
    Label(text="Control Concept 2", bg=fondo_encabezado,width="300", height="2", fg=letra_encabezado, font=("Calibri",16,'bold')).pack()
    Label(text="", bg=fondo_ventana).pack()
    Button(text="Ingresar", height="2",width="30", bg=fondo_boton, fg=letra_boton,font=("Calibri",12,'bold'), command=login).pack()
    Label(text="",bg=fondo_ventana).pack()
    
    ventana_principal.mainloop()

#Definimos la función para el Login del Usuario

def login():
    global ventana_login
    global fondo_ventana, fondo_encabezado, fondo_boton, letra_boton, letra_encabezado
    global entrada_login_usuario, entrada_login_clave
    global verifica_usuario, verifica_clave
    
    ventana_login = Toplevel(ventana_principal)
    ventana_login.title("Ingreso al Sistema")
    ventana_login.geometry("300x250")
    ventana_login.config(bg=fondo_ventana)
    Label(ventana_login, text="Ingrese credenciales", width="300",height="2",bg=fondo_encabezado, fg="white",font=('Calibri',12,'bold')).pack()
    Label(ventana_login,text="", bg=fondo_ventana).pack()
	
    verifica_usuario = StringVar()
    verifica_clave = StringVar()

    Label(ventana_login, text="Usuario: ", fg=fondo_encabezado,font=("Calibri",12,'bold')).pack()
    entrada_login_usuario = Entry(ventana_login, textvariable=verifica_usuario)
    entrada_login_usuario.pack()
    Label(ventana_login, text="").pack()
    Label(ventana_login, text="Contraseña: ",fg=fondo_encabezado,font=("Calibri",12,'bold')).pack()
    entrada_login_clave = Entry(ventana_login, textvariable=verifica_clave, show="*")
    entrada_login_clave.pack()
    Label(ventana_login,text="", bg=fondo_ventana).pack()
    Button(ventana_login, text="Ingresar", width="10", height="1",fg=letra_boton, bg=fondo_boton,font=("Calibri",12,"bold"), command=verifica_login).pack()
    
    
# Realizamos la verificación de usuario y contraseña

def verifica_login():
	usuario1 = verifica_usuario.get()
	clave1 = verifica_clave.get()
	entrada_login_usuario.delete(0,END)
	entrada_login_clave.delete(0,END)
	
	lista_archivos = os.listdir()#
	if usuario1 in lista_archivos:
		archivo1 = open(usuario1,"r")
		verifica = archivo1.read().splitlines()
		if clave1 in verifica:
			interfaz_principal()
		else:
			no_clave()
	else:
		no_usuario()
        
# Función para capturar el puerto seleccionado y abrirlo
def seleccion_puerto():
    global combo,s, port_selected,opcion
    port_selected = False
    
    
    if(opcion.get()):
        port_selected = True
        puerto=opcion.get()    
        s = sr.Serial(puerto,115200); #COM4 arduino, COM6 ESP32
        s.reset_input_buffer()
        #print("puerto "+ puerto + " abierto")
    else:
        print("No se seleccionó un puerto.")
    
    
    

# Función de éxito de Login

def interfaz_principal():
    global ventana_interfaz_principal
    global fondo_ventana, fondo_encabezado, fondo_boton, letra_boton, letra_encabezado
    global ax1,ax2, ax3, lines, canvas
    global estado_operacion
    global Samples, numData
    global l1,l2,l3,l4
    global bt_calibrar, bt_iniciar, bt_detener, combo,s, port_selected,opcion
    estado_operacion = "Bienvenid@"
    ventana_interfaz_principal = Toplevel(ventana_login)
    ventana_interfaz_principal.title("Control Concept 2")
    ventana_interfaz_principal.geometry("800x700")
    ventana_interfaz_principal.configure(bg=fondo_ventana)
    
    opcion=StringVar()
    
    l0 = Label(ventana_interfaz_principal, text="Control Concept 2", bg=fondo_encabezado, width="800", height ="2", fg= "white", font=("futura",18,"bold")).pack()
    la = Label(ventana_interfaz_principal, text="", bg=fondo_ventana, fg= fondo_boton, font=("calibri",12,"bold")).pack(anchor="nw")
    l1 = Label(ventana_interfaz_principal, text="Seleccione el puerto COM:", bg=fondo_ventana, fg= fondo_boton, font=("calibri",12,"bold")).pack(anchor="nw")
    combo = ttk.Combobox(ventana_interfaz_principal, width="17",state="readonly",textvariable=opcion,values=puertos_seriales())
    combo.place(x=40,y=110)
    combo.current(0)
    lc = Label(ventana_interfaz_principal,text="", bg=fondo_ventana).pack()
    bt_seleccionar = Button(ventana_interfaz_principal, text="Seleccionar Puerto", height="2", width="30", command=seleccion_puerto).pack(anchor="nw") #cuando se le pone los paréntesis a la función la ejecuta antes de oprimir el botón
    ld = Label(ventana_interfaz_principal,text="", bg=fondo_ventana).pack()
    lb = Label(ventana_interfaz_principal, text="", bg=fondo_ventana, fg= fondo_boton, font=("calibri",12,"bold")).pack(anchor="nw")
    bt_calibrar = Button(ventana_interfaz_principal, text="Calibrar", height="2", width="30", command=btn_calibrar).pack(anchor="nw")
    l2 = Label(ventana_interfaz_principal,text="", bg=fondo_ventana).pack()
    bt_iniciar = Button(ventana_interfaz_principal, text="Iniciar", height="2", width="30",command=btn_iniciar).pack(anchor="nw") #Desactivamos el botón por defecto.
    l3 = Label(ventana_interfaz_principal,text="", bg=fondo_ventana).pack()
    bt_detener = Button(ventana_interfaz_principal, text="Detener", height="2", width="30",command=btn_detener).pack(anchor="nw")
    l4 = Label(ventana_interfaz_principal,text=estado_operacion, bg=fondo_ventana, fg=fondo_boton,padx=70, font=('calibri',12,'bold')).pack(anchor="nw")
           
    for i in range(3):    	
    	lines.append(Line2D([],[],color='blue'))

#-----Creamos las gráficas sobre el objeto GUI---------
# Añadimos las figuras al Canvas
    fig = Figure();
    (ax1, ax2, ax3) = fig.subplots(3)

#--------Configuración del 1er subplot-------------    
    fig.suptitle('Variables Cinemáticas',fontsize=20)
    ax1.set_ylabel('posición [cm]')
    ax1.set_xlim(0,100)
    ax1.set_ylim(0,110)#límites de eje y
    ax1.grid()
    lines[0] = ax1.plot([],[])[0]
    
#--------Configuración del 2do subplot-------------    
    ax2.set_ylabel('velocidad [cm/s]')
    ax2.set_xlim(0,100)
    ax2.set_ylim(0,25)#límites de eje y
    ax2.grid()
    lines[1] = ax2.plot([],[],'tab:red')[0]

#--------Configuración del 3er subplot-------------    
    #ax3.set_title('Aceleración');
    ax3.set_xlabel('tiempo[s]')
    ax3.set_ylabel('$aceleración [cm/s^2]$')
    ax3.set_xlim(0,100)
    ax3.set_ylim(-500,500)#límites de eje y
    ax3.grid()
    lines[2] = ax3.plot([],[],'tab:green')[0]
    
    canvas =  FigureCanvasTkAgg(fig, master = ventana_interfaz_principal) # A tk.DrawingArea.
    canvas.get_tk_widget().place(x=250,y=65,width=500, height=600)
    canvas.draw()

    ventana_interfaz_principal.update();
    if (port_selected == True):
        s.reset_input_buffer()
        #print("Entramos")
        
    ventana_interfaz_principal.after(1, plot_data) #Actualización cada 1 ms.
    ventana_interfaz_principal.mainloop()

#Definición de función de no clave

def no_clave():
	global ventana_no_clave
	ventana_no_clave = Toplevel(ventana_login)
	ventana_no_clave.title("ERROR")
	ventana_no_clave.geometry("150x100")
	Label(ventana_no_clave, text="Contraseña incorrecta ").pack()
	Button(ventana_no_clave, text="OK", command=borrar_no_clave).pack()

def borrar_interfaz_principal():
	ventana_interfaz_principal.destroy()

def borrar_no_clave():
	ventana_no_clave.destroy()			

def borrar_no_usuario():
    ventana_no_clave.destroy()

def btn_calibrar():
    global cond, calibrated
    global estado_operacion, l4, ventana_interfaz_principal,s
    cond = True
    s.write(b'a')#Envía el número 97 a la ESP32
    calibrated = True
    estado_operacion = "Calibrando..."
    l4 = Label(ventana_interfaz_principal,text=estado_operacion,fg=fondo_encabezado,font=("Calibri",12,'bold')).pack(anchor="nw")
    ventana_interfaz_principal.update()
    
def btn_iniciar():
    global estado_operacion, l4, ventana_interfaz_principal,s
    s.write(b'b')#Envía el número 98 a la ESP32
    estado_operacion = "Iniciando operación"
    l4 = Label(ventana_interfaz_principal,text=estado_operacion,fg=fondo_encabezado,font=("Calibri",12,'bold')).pack(anchor="nw")
    ventana_interfaz_principal.update()
    
    

def btn_detener():
    global estado_operacion, l4, ventana_interfaz_principal,s
    s.write(b'c')#Envía el número 99 a la ESP32
    estado_operacion = "Deteniendo..."
    l4 = Label(ventana_interfaz_principal,text=estado_operacion,fg=fondo_encabezado,font=("Calibri",12,'bold')).pack(anchor="nw")
    ventana_interfaz_principal.update()
    s.close();
    
    
    
        
#-------Funciones para el representación de las gráficas--------
def plot_data():
    global cond, data, lines, canvas, Samples,s
    pos_inicial = 36
    
    millis_act = 0.0
    millis_desp = 0.0
    if (cond == True):
        a = s.readline()
        millis_act = time.time()*1000
        value = float(a.strip()) #Leer sensor
        value = value - pos_inicial
        #print ("pos: "+str(value))
        if value < 0 :
            value = 0
        if (len(data[0])<100):
            data[0].append(value) #Permite acrecentar el tamaño del vector hasta llegar a 100
        else: # Se renuevan las 100 muestras del vector en un solo paso, esto da el efecto de continuidad en la grafica después de las 100 muestras
            data[0][0:99] = data[0][1:100]
            data[0][99] = value
            
        a = s.readline()
        millis_desp = time.time()*1000
        value = float(a.strip()) #Leer sensor
        value = (value*5.5)/60.0  # Transformado a velocidad lineal de la silla
        #print ("Velocidad: "+str(value))
        
        if (len(data[1])<100):            
            data[1].append(value) #Guarda lectura en la ultima posición
        else:
            data[1][0:99] = data[1][1:100]
            data[1][99] = value
            
        acel = np.diff(data[1])/((millis_desp/1000.0)-(millis_act/1000.0))
        
        if (len(data[2])<100):
            data[2].append(acel[-1])
        else:
            data[2][0:99] = data[2][1:100]
            data[2][99] = acel[-1]
        #print("acel: "+str(acel[-1]))            
        
        lines[0].set_xdata(np.arange(0,len(data[0])))
        lines[0].set_ydata(data[0])
        lines[1].set_xdata(np.arange(0,len(data[1])))
        lines[1].set_ydata(data[1])
        lines[2].set_xdata(np.arange(0,len(data[2])))
        lines[2].set_ydata(data[2])
        canvas.draw()
        
    ventana_interfaz_principal.after(1,plot_data)

ventana_inicio()
