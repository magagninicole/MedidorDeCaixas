from warnings import catch_warnings
import serial
import tkinter as tk
from PIL import ImageTk, Image  

root = tk.Tk()
root.geometry('800x650')


image1 = Image.open("caixa.png")
tkImage = ImageTk.PhotoImage(image1)
labelFoto = tk.Label(image=tkImage)
labelFoto.image = labelFoto
labelFoto.place(x=100,y=150)

distGeral = tk.Label(text="Valor medido: ")
distGeral.configure(font=("Arial",20))
distGeral.place(x=270,y=70)
distGeralVal = tk.Label(text="0")
distGeralVal.configure(font=("Arial",20))
distGeralVal.place(x=470,y=70)

distX = tk.Label(text="X: ")
distX.configure(font=("Arial",20))
distX.place(x=440,y=400)
distXVal = tk.Label(text="0")
distXVal.configure(font=("Arial",20))
distXVal.place(x=490,y=400)

distY = tk.Label(text="Y: ")
distY.configure(font=("Arial",20))
distY.place(x=160,y=370)
distYVal = tk.Label(text="0")
distYVal.configure(font=("Arial",20))
distYVal.place(x=200,y=370)

distZ = tk.Label(text="Z: ")
distZ.configure(font=("Arial",20))
distZ.place(x=630,y=250)
distZVal = tk.Label(text="0")
distZVal.configure(font=("Arial",20))
distZVal.place(x=670,y=250)

vol = tk.Label(text="VOLUME: ")
vol.configure(font=("Arial",20))
vol.place(x=300,y=520)
volVal = tk.Label(text="0")
volVal.configure(font=("Arial",20))
volVal.place(x=450,y=520)

data = False
try:
    root.update()
    port = serial.Serial("COM3",9600)
    data = True
except:
    print("No devices on PORT")

contador = 0

while data:
    line = port.readline()
    
    if(contador == 4):
        volVal["text"] = line
        contador = 0
    if(contador==3):
        distZVal['text'] = line
        contador =4
    if(contador == 2): 
        distYVal['text'] = line
        contador =3
    if(contador == 1):
        distXVal['text'] = line
        contador =2
    if(line == b'555\r\n'):
        contador = 1
    print(line)
    distGeralVal['text'] = line
    root.update()

while not data:
    root.update()
