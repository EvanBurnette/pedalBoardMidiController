##program to quickly test expression mod to my wah pedal

import serial

ser = serial.Serial('COM5', 9600)

while(True):
    msg = ser.read(size=1)
    print(msg)
