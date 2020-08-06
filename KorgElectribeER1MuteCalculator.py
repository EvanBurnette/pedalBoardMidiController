#!/usr/bin/env python
# -*- coding: utf-8 -*-

import tkinter as tk

muteDict = {'none': 3903, 'synth1': 3647, 'synth2': 3391, 'synth3': 2879, 'synth4': 1855, 'audio1': 3887, 'audio2': 3871, 'closed high hat': 3902, 'open high hat': 3901, 'crash': 3899, 'hand clap': 3895}


window = tk.Tk()
window.title('KORG Electribe ER1 Mute State NRPN Message Calculator')
window.geometry('700x900')
#window.configure(bg="black")
title = tk.Label(window, bg="white", width=20, text="Binary representation:")
title.pack()
                 
label = tk.Label(window, bg='lightgrey', width=20, borderwidth=2, relief='solid', text=format(muteDict['none'], '016b'))
label.pack()

title2 = tk.Label(window, bg="white", width=20, text="Hex representation:")
title2.pack()

info2 = tk.Label(window, bg='lightgrey', width=20, borderwidth=2, relief='solid', text='0x' + format(muteDict['none'], '04x').upper())

info2.pack()
 
def print_selection():
    combo = var1.get() & var2.get() & var3.get() & var4.get() & var5.get() & var6.get() & var7.get() & var8.get() & var9.get() & var10.get() & muteDict['none']
    nrpnBoiler = "0xB9, 0x63, 0x02\n0xB9, 0x62, 0x6D\n0xB9, 0x06, 0x" + format(combo>>8, '02x').upper() + "\n\n0xB9, 0x63, 0x02\n0xB9, 0x62, 0x6E\n0xB9, 0x06, 0x" + format(combo & 0xFF, '02x').upper()
    guide2.config(text=nrpnBoiler)
    label.config(text=format(combo, '016b'))
    info2.config(text='0x' + format(combo, '04x').upper())
 
var1 = tk.IntVar(value=0xFFFF)
var2 = tk.IntVar(value=0xFFFF)
var3 = tk.IntVar(value=0xFFFF)
var4 = tk.IntVar(value=0xFFFF)
var5 = tk.IntVar(value=0xFFFF)
var6 = tk.IntVar(value=0xFFFF)
var7 = tk.IntVar(value=0xFFFF)
var8 = tk.IntVar(value=0xFFFF)
var9 = tk.IntVar(value=0xFFFF)
var10 = tk.IntVar(value=0xFFFF)

p1 = tk.Checkbutton(window, text='Synth 1',variable=var1, onvalue=muteDict['synth1'], offvalue=0xFFFF, command=print_selection)
p1.pack()
p2 = tk.Checkbutton(window, text='Synth 2',variable=var2, onvalue=muteDict['synth2'], offvalue=0xFFFF, command=print_selection)
p2.pack()
p3 = tk.Checkbutton(window, text='Synth 3',variable=var3, onvalue=muteDict['synth3'], offvalue=0xFFFF, command=print_selection)
p3.pack()
p4 = tk.Checkbutton(window, text='Synth 4',variable=var4, onvalue=muteDict['synth4'], offvalue=0xFFFF, command=print_selection)
p4.pack()
p5 = tk.Checkbutton(window, text='Audio 1',variable=var5, onvalue=muteDict['audio1'], offvalue=0xFFFF, command=print_selection)
p5.pack()
p6 = tk.Checkbutton(window, text='Audio 2',variable=var6, onvalue=muteDict['audio2'], offvalue=0xFFFF, command=print_selection)
p6.pack()
p7 = tk.Checkbutton(window, text='Closed High Hat',variable=var7, onvalue=muteDict['closed high hat'], offvalue=0xFFFF, command=print_selection)
p7.pack()
p8 = tk.Checkbutton(window, text='Open High Hat',variable=var8, onvalue=muteDict['open high hat'], offvalue=0xFFFF, command=print_selection)
p8.pack()
p9 = tk.Checkbutton(window, text='Crash',variable=var9, onvalue=muteDict['crash'], offvalue=0xFFFF, command=print_selection)
p9.pack()
p10 = tk.Checkbutton(window, text='Hand Clap',variable=var10, onvalue=muteDict['hand clap'], offvalue=0xFFFF, command=print_selection)
p10.pack()

guide = tk.Label(window, bg="white", width=60, text="Check boxes to unmute ER-1 parts\n\nSend the following bytes via midi to achieve selected mute state:\n(This is the format for NRPN messages for the KORG Electribe ER-1)\n")
guide.pack()

nrpnBoiler = "0xB9, 0x63, 0x02\n0xB9, 0x62, 0x6D\n0xB9, 0x06, 0x" + format(0, '02x').upper() + "\n\n0xB9, 0x63, 0x02\n0xB9, 0x62, 0x6E\n0xB9, 0x06, 0x" + format(0, '02x').upper()
guide2 = tk.Label(window, bg="lightgrey", width=20, borderwidth=2, relief='solid', text=nrpnBoiler)
guide2.pack()

spacer = tk.Label(window, bg="white", width=20, text="")
spacer.pack()

panelImage = tk.PhotoImage(file="ER1PartsPanel.png")
panel = tk.Label(window, bg="white", borderwidth=2, relief='solid', image=panelImage)
panel.pack()
window.mainloop()
