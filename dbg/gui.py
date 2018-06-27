#! /usr/bin/env python
#
# GUI module generated by PAGE version 4.5
# In conjunction with Tcl version 8.6
#    Jun 27, 2018 11:36:38 AM
import sys

try:
    from Tkinter import *
except ImportError:
    from tkinter import *

try:
    import ttk
    py3 = 0
except ImportError:
    import tkinter.ttk as ttk
    py3 = 1

import gui_support

def vp_start_gui():
    '''Starting point when module is the main routine.'''
    global val, w, root
    root = Tk()
    root.title('New_Toplevel_1')
    geom = "603x581+292+128"
    root.geometry(geom)
    gui_support.set_Tk_var()
    w = New_Toplevel_1 (root)
    gui_support.init(root, w)
    root.mainloop()

w = None
def create_New_Toplevel_1(root, param=None):
    '''Starting point when module is imported by another program.'''
    global w, w_win, rt
    rt = root
    w = Toplevel (root)
    w.title('New_Toplevel_1')
    geom = "603x581+292+128"
    w.geometry(geom)
    gui_support.set_Tk_var()
    w_win = New_Toplevel_1 (w)
    gui_support.init(w, w_win, param)
    return w_win

def destroy_New_Toplevel_1():
    global w
    w.destroy()
    w = None


class New_Toplevel_1:
    def __init__(self, master=None):
        _bgcolor = '#d9d9d9'  # X11 color: 'gray85'
        _fgcolor = '#000000'  # X11 color: 'black'
        _compcolor = '#d9d9d9' # X11 color: 'gray85'
        _ana1color = '#d9d9d9' # X11 color: 'gray85' 
        _ana2color = '#d9d9d9' # X11 color: 'gray85' 
        font11 = "-family {Courier New} -size 10 -weight normal -slant"  \
            " roman -underline 0 -overstrike 0"
        self.style = ttk.Style()
        if sys.platform == "win32":
            self.style.theme_use('winnative')
        self.style.configure('.',background=_bgcolor)
        self.style.configure('.',foreground=_fgcolor)
        self.style.configure('.',font="TkDefaultFont")
        self.style.map('.',background=
            [('selected', _compcolor), ('active',_ana2color)])
        master.configure(background="#d9d9d9")
        master.configure(highlightbackground="#d9d9d9")
        master.configure(highlightcolor="black")


        self.style.configure('TNotebook.Tab', background=_bgcolor)
        self.style.configure('TNotebook.Tab', foreground=_fgcolor)
        self.style.map('TNotebook.Tab', background=
            [('selected', _compcolor), ('active',_ana2color)])
        self.TNotebook1 = ttk.Notebook(master)
        self.TNotebook1.place(relx=0.02, rely=0.08, relheight=0.87
                , relwidth=0.94)
        self.TNotebook1.configure(width=569)
        self.TNotebook1.configure(takefocus="")
        self.TNotebook1_pg0 = ttk.Frame(self.TNotebook1)
        self.TNotebook1.add(self.TNotebook1_pg0, padding=3)
        self.TNotebook1.tab(0, text="Seriale",underline="-1",)
        self.TNotebook1_pg1 = ttk.Frame(self.TNotebook1)
        self.TNotebook1.add(self.TNotebook1_pg1, padding=3)
        self.TNotebook1.tab(1, text="Varie",underline="-1",)
        self.TNotebook1_pg2 = ttk.Frame(self.TNotebook1)
        self.TNotebook1.add(self.TNotebook1_pg2, padding=3)
        self.TNotebook1.tab(2, text="Prod",underline="-1",)
        self.TNotebook1_pg3 = ttk.Frame(self.TNotebook1)
        self.TNotebook1.add(self.TNotebook1_pg3, padding=3)
        self.TNotebook1.tab(3, text="HW",underline="-1",)

        self.Entry1 = Entry(self.TNotebook1_pg0)
        self.Entry1.place(relx=0.23, rely=0.23, relheight=0.05, relwidth=0.69)
        self.Entry1.configure(background="white")
        self.Entry1.configure(disabledforeground="#a3a3a3")
        self.Entry1.configure(font=font11)
        self.Entry1.configure(foreground="#000000")
        self.Entry1.configure(highlightbackground="#d9d9d9")
        self.Entry1.configure(highlightcolor="black")
        self.Entry1.configure(insertbackground="black")
        self.Entry1.configure(justify=CENTER)
        self.Entry1.configure(selectbackground="#c4c4c4")
        self.Entry1.configure(selectforeground="black")
        self.Entry1.configure(textvariable=gui_support.portaSeriale)

        self.Button1 = Button(self.TNotebook1_pg0)
        self.Button1.place(relx=0.05, rely=0.23, height=25, width=80)
        self.Button1.configure(activebackground="#d9d9d9")
        self.Button1.configure(activeforeground="#000000")
        self.Button1.configure(background=_bgcolor)
        self.Button1.configure(command=self.apriSeriale)
        self.Button1.configure(disabledforeground="#a3a3a3")
        self.Button1.configure(foreground="#000000")
        self.Button1.configure(highlightbackground="#d9d9d9")
        self.Button1.configure(highlightcolor="black")
        self.Button1.configure(pady="0")
        self.Button1.configure(text='''Usa questa''')

        self.Button47 = Button(self.TNotebook1_pg0)
        self.Button47.place(relx=0.05, rely=0.34, height=25, width=80)
        self.Button47.configure(activebackground="#d9d9d9")
        self.Button47.configure(activeforeground="#000000")
        self.Button47.configure(background=_bgcolor)
        self.Button47.configure(command=self.apriFTDI)
        self.Button47.configure(disabledforeground="#a3a3a3")
        self.Button47.configure(foreground="#000000")
        self.Button47.configure(highlightbackground="#d9d9d9")
        self.Button47.configure(highlightcolor="black")
        self.Button47.configure(pady="0")
        self.Button47.configure(text='''Usa FTDI''')

        self.Button4 = Button(self.TNotebook1_pg0)
        self.Button4.place(relx=0.05, rely=0.45, height=24, width=82)
        self.Button4.configure(activebackground="#d9d9d9")
        self.Button4.configure(activeforeground="#000000")
        self.Button4.configure(background=_bgcolor)
        self.Button4.configure(command=self.apriSocket)
        self.Button4.configure(disabledforeground="#a3a3a3")
        self.Button4.configure(foreground="#000000")
        self.Button4.configure(highlightbackground="#d9d9d9")
        self.Button4.configure(highlightcolor="black")
        self.Button4.configure(pady="0")
        self.Button4.configure(text='''TCP''')

        self.Entry4 = Entry(self.TNotebook1_pg0)
        self.Entry4.place(relx=0.23, rely=0.45, relheight=0.05, relwidth=0.35)
        self.Entry4.configure(background="white")
        self.Entry4.configure(disabledforeground="#a3a3a3")
        self.Entry4.configure(font=font11)
        self.Entry4.configure(foreground="#000000")
        self.Entry4.configure(highlightbackground="#d9d9d9")
        self.Entry4.configure(highlightcolor="black")
        self.Entry4.configure(insertbackground="black")
        self.Entry4.configure(justify=CENTER)
        self.Entry4.configure(selectbackground="#c4c4c4")
        self.Entry4.configure(selectforeground="black")
        self.Entry4.configure(textvariable=gui_support.indirizzoIP)

        self.Button2 = Button(self.TNotebook1_pg1)
        self.Button2.place(relx=0.05, rely=0.05, height=25, width=50)
        self.Button2.configure(activebackground="#d9d9d9")
        self.Button2.configure(activeforeground="#000000")
        self.Button2.configure(background=_bgcolor)
        self.Button2.configure(command=self.Eco)
        self.Button2.configure(disabledforeground="#a3a3a3")
        self.Button2.configure(foreground="#000000")
        self.Button2.configure(highlightbackground="#d9d9d9")
        self.Button2.configure(highlightcolor="black")
        self.Button2.configure(pady="0")
        self.Button2.configure(text='''Eco''')

        self.TLabelframe1 = ttk.Labelframe(self.TNotebook1_pg1)
        self.TLabelframe1.place(relx=0.18, rely=0.02, relheight=0.42
                , relwidth=0.4)
        self.TLabelframe1.configure(text='''Echi''')
        self.TLabelframe1.configure(width=230)

        self.Label2 = Label(self.TLabelframe1)
        self.Label2.place(relx=0.09, rely=0.12, height=25, width=39)
        self.Label2.configure(activebackground="#f9f9f9")
        self.Label2.configure(activeforeground="black")
        self.Label2.configure(background=_bgcolor)
        self.Label2.configure(disabledforeground="#a3a3a3")
        self.Label2.configure(foreground="#000000")
        self.Label2.configure(highlightbackground="#d9d9d9")
        self.Label2.configure(highlightcolor="black")
        self.Label2.configure(text='''Invia''')

        self.Entry2 = Entry(self.TLabelframe1)
        self.Entry2.place(relx=0.33, rely=0.12, relheight=0.12, relwidth=0.3)
        self.Entry2.configure(background="white")
        self.Entry2.configure(disabledforeground="#a3a3a3")
        self.Entry2.configure(font=font11)
        self.Entry2.configure(foreground="#000000")
        self.Entry2.configure(highlightbackground="#d9d9d9")
        self.Entry2.configure(highlightcolor="black")
        self.Entry2.configure(insertbackground="black")
        self.Entry2.configure(justify=CENTER)
        self.Entry2.configure(selectbackground="#c4c4c4")
        self.Entry2.configure(selectforeground="black")
        self.Entry2.configure(textvariable=gui_support.numEco)

        self.Button3 = Button(self.TLabelframe1)
        self.Button3.place(relx=0.3, rely=0.55, height=25, width=90)
        self.Button3.configure(activebackground="#d9d9d9")
        self.Button3.configure(activeforeground="#000000")
        self.Button3.configure(background=_bgcolor)
        self.Button3.configure(disabledforeground="#a3a3a3")
        self.Button3.configure(foreground="#000000")
        self.Button3.configure(highlightbackground="#d9d9d9")
        self.Button3.configure(highlightcolor="black")
        self.Button3.configure(pady="0")
        self.Button3.configure(text='''Esegui''')
        self.Button3.bind('<ButtonRelease-1>',self.ecoProva)

        self.TProgressbar1 = ttk.Progressbar(self.TLabelframe1)
        self.TProgressbar1.place(relx=0.11, rely=0.76, relwidth=0.78
                , relheight=0.0, height=22)
        self.TProgressbar1.configure(mode="indeterminate")

        self.Label3 = Label(self.TLabelframe1)
        self.Label3.place(relx=0.67, rely=0.12, height=21, width=55)
        self.Label3.configure(activebackground="#f9f9f9")
        self.Label3.configure(activeforeground="black")
        self.Label3.configure(background=_bgcolor)
        self.Label3.configure(disabledforeground="#a3a3a3")
        self.Label3.configure(foreground="#000000")
        self.Label3.configure(highlightbackground="#d9d9d9")
        self.Label3.configure(highlightcolor="black")
        self.Label3.configure(text='''pacchetti''')

        self.Label4 = Label(self.TLabelframe1)
        self.Label4.place(relx=0.15, rely=0.33, height=21, width=29)
        self.Label4.configure(activebackground="#f9f9f9")
        self.Label4.configure(activeforeground="black")
        self.Label4.configure(background=_bgcolor)
        self.Label4.configure(disabledforeground="#a3a3a3")
        self.Label4.configure(foreground="#000000")
        self.Label4.configure(highlightbackground="#d9d9d9")
        self.Label4.configure(highlightcolor="black")
        self.Label4.configure(text='''da''')

        self.Entry3 = Entry(self.TLabelframe1)
        self.Entry3.place(relx=0.33, rely=0.31, relheight=0.12, relwidth=0.3)
        self.Entry3.configure(background="white")
        self.Entry3.configure(disabledforeground="#a3a3a3")
        self.Entry3.configure(font=font11)
        self.Entry3.configure(foreground="#000000")
        self.Entry3.configure(highlightbackground="#d9d9d9")
        self.Entry3.configure(highlightcolor="black")
        self.Entry3.configure(insertbackground="black")
        self.Entry3.configure(justify=CENTER)
        self.Entry3.configure(selectbackground="#c4c4c4")
        self.Entry3.configure(selectforeground="black")
        self.Entry3.configure(textvariable=gui_support.dimEco)

        self.Label5 = Label(self.TLabelframe1)
        self.Label5.place(relx=0.7, rely=0.33, height=21, width=29)
        self.Label5.configure(activebackground="#f9f9f9")
        self.Label5.configure(activeforeground="black")
        self.Label5.configure(background=_bgcolor)
        self.Label5.configure(disabledforeground="#a3a3a3")
        self.Label5.configure(foreground="#000000")
        self.Label5.configure(highlightbackground="#d9d9d9")
        self.Label5.configure(highlightcolor="black")
        self.Label5.configure(text='''byte''')

        self.TLabelframe2 = ttk.Labelframe(self.TNotebook1_pg2)
        self.TLabelframe2.place(relx=0.04, rely=0.06, relheight=0.24
                , relwidth=0.68)
        self.TLabelframe2.configure(text='''Codice prodotto''')
        self.TLabelframe2.configure(width=385)

        self.Entry5 = Entry(self.TLabelframe2)
        self.Entry5.place(relx=0.23, rely=0.25, relheight=0.21, relwidth=0.5)
        self.Entry5.configure(background="white")
        self.Entry5.configure(disabledforeground="#a3a3a3")
        self.Entry5.configure(font=font11)
        self.Entry5.configure(foreground="#000000")
        self.Entry5.configure(highlightbackground="#d9d9d9")
        self.Entry5.configure(highlightcolor="black")
        self.Entry5.configure(insertbackground="black")
        self.Entry5.configure(justify=CENTER)
        self.Entry5.configure(selectbackground="#c4c4c4")
        self.Entry5.configure(selectforeground="black")
        self.Entry5.configure(textvariable=gui_support.cp)

        self.Button5 = Button(self.TLabelframe2)
        self.Button5.place(relx=0.05, rely=0.25, height=29, width=55)
        self.Button5.configure(activebackground="#d9d9d9")
        self.Button5.configure(activeforeground="#000000")
        self.Button5.configure(background=_bgcolor)
        self.Button5.configure(command=self.leggi_cp)
        self.Button5.configure(disabledforeground="#a3a3a3")
        self.Button5.configure(foreground="#000000")
        self.Button5.configure(highlightbackground="#d9d9d9")
        self.Button5.configure(highlightcolor="black")
        self.Button5.configure(pady="0")
        self.Button5.configure(text='''Leggi''')

        self.Button7 = Button(self.TLabelframe2)
        self.Button7.place(relx=0.4, rely=0.58, height=29, width=65)
        self.Button7.configure(activebackground="#d9d9d9")
        self.Button7.configure(activeforeground="#000000")
        self.Button7.configure(background=_bgcolor)
        self.Button7.configure(command=self.inventa_cp)
        self.Button7.configure(disabledforeground="#a3a3a3")
        self.Button7.configure(foreground="#000000")
        self.Button7.configure(highlightbackground="#d9d9d9")
        self.Button7.configure(highlightcolor="black")
        self.Button7.configure(pady="0")
        self.Button7.configure(text='''Inventa''')

        self.Button6 = Button(self.TLabelframe2)
        self.Button6.place(relx=0.78, rely=0.25, height=29, width=54)
        self.Button6.configure(activebackground="#d9d9d9")
        self.Button6.configure(activeforeground="#000000")
        self.Button6.configure(background=_bgcolor)
        self.Button6.configure(command=self.scrivi_cp)
        self.Button6.configure(disabledforeground="#a3a3a3")
        self.Button6.configure(foreground="#000000")
        self.Button6.configure(highlightbackground="#d9d9d9")
        self.Button6.configure(highlightcolor="black")
        self.Button6.configure(pady="0")
        self.Button6.configure(text='''Scrivi''')

        self.TLabelframe3 = ttk.Labelframe(self.TNotebook1_pg2)
        self.TLabelframe3.place(relx=0.04, rely=0.33, relheight=0.24
                , relwidth=0.68)
        self.TLabelframe3.configure(text='''Codice scheda''')
        self.TLabelframe3.configure(width=385)

        self.Entry6 = Entry(self.TLabelframe3)
        self.Entry6.place(relx=0.23, rely=0.25, relheight=0.21, relwidth=0.5)
        self.Entry6.configure(background="white")
        self.Entry6.configure(disabledforeground="#a3a3a3")
        self.Entry6.configure(font=font11)
        self.Entry6.configure(foreground="#000000")
        self.Entry6.configure(highlightbackground="#d9d9d9")
        self.Entry6.configure(highlightcolor="black")
        self.Entry6.configure(insertbackground="black")
        self.Entry6.configure(justify=CENTER)
        self.Entry6.configure(selectbackground="#c4c4c4")
        self.Entry6.configure(selectforeground="black")
        self.Entry6.configure(textvariable=gui_support.cs)

        self.Button8 = Button(self.TLabelframe3)
        self.Button8.place(relx=0.05, rely=0.25, height=29, width=55)
        self.Button8.configure(activebackground="#d9d9d9")
        self.Button8.configure(activeforeground="#000000")
        self.Button8.configure(background=_bgcolor)
        self.Button8.configure(command=self.leggi_cs)
        self.Button8.configure(disabledforeground="#a3a3a3")
        self.Button8.configure(foreground="#000000")
        self.Button8.configure(highlightbackground="#d9d9d9")
        self.Button8.configure(highlightcolor="black")
        self.Button8.configure(pady="0")
        self.Button8.configure(text='''Leggi''')

        self.Button9 = Button(self.TLabelframe3)
        self.Button9.place(relx=0.4, rely=0.58, height=29, width=65)
        self.Button9.configure(activebackground="#d9d9d9")
        self.Button9.configure(activeforeground="#000000")
        self.Button9.configure(background=_bgcolor)
        self.Button9.configure(command=self.inventa_cs)
        self.Button9.configure(disabledforeground="#a3a3a3")
        self.Button9.configure(foreground="#000000")
        self.Button9.configure(highlightbackground="#d9d9d9")
        self.Button9.configure(highlightcolor="black")
        self.Button9.configure(pady="0")
        self.Button9.configure(text='''Inventa''')

        self.Button10 = Button(self.TLabelframe3)
        self.Button10.place(relx=0.78, rely=0.25, height=29, width=54)
        self.Button10.configure(activebackground="#d9d9d9")
        self.Button10.configure(activeforeground="#000000")
        self.Button10.configure(background=_bgcolor)
        self.Button10.configure(command=self.scrivi_cs)
        self.Button10.configure(disabledforeground="#a3a3a3")
        self.Button10.configure(foreground="#000000")
        self.Button10.configure(highlightbackground="#d9d9d9")
        self.Button10.configure(highlightcolor="black")
        self.Button10.configure(pady="0")
        self.Button10.configure(text='''Scrivi''')

        self.TLabelframe4 = ttk.Labelframe(self.TNotebook1_pg3)
        self.TLabelframe4.place(relx=0.04, rely=0.04, relheight=0.19
                , relwidth=0.5)
        self.TLabelframe4.configure(text='''Tasto''')
        self.TLabelframe4.configure(width=285)

        self.Button11 = Button(self.TLabelframe4)
        self.Button11.place(relx=0.11, rely=0.33, height=34, width=62)
        self.Button11.configure(activebackground="#d9d9d9")
        self.Button11.configure(activeforeground="#000000")
        self.Button11.configure(background=_bgcolor)
        self.Button11.configure(command=self.tst_zero)
        self.Button11.configure(disabledforeground="#a3a3a3")
        self.Button11.configure(foreground="#000000")
        self.Button11.configure(highlightbackground="#d9d9d9")
        self.Button11.configure(highlightcolor="black")
        self.Button11.configure(pady="0")
        self.Button11.configure(text='''Azzera''')

        self.Button12 = Button(self.TLabelframe4)
        self.Button12.place(relx=0.72, rely=0.39, height=29, width=62)
        self.Button12.configure(activebackground="#d9d9d9")
        self.Button12.configure(activeforeground="#000000")
        self.Button12.configure(background=_bgcolor)
        self.Button12.configure(command=self.tst_lgg)
        self.Button12.configure(disabledforeground="#a3a3a3")
        self.Button12.configure(foreground="#000000")
        self.Button12.configure(highlightbackground="#d9d9d9")
        self.Button12.configure(highlightcolor="black")
        self.Button12.configure(pady="0")
        self.Button12.configure(text='''Leggi''')

        self.Entry7 = Entry(self.TLabelframe4)
        self.Entry7.place(relx=0.39, rely=0.39, relheight=0.28, relwidth=0.28)
        self.Entry7.configure(background="white")
        self.Entry7.configure(disabledforeground="#a3a3a3")
        self.Entry7.configure(font=font11)
        self.Entry7.configure(foreground="#000000")
        self.Entry7.configure(highlightbackground="#d9d9d9")
        self.Entry7.configure(highlightcolor="black")
        self.Entry7.configure(insertbackground="black")
        self.Entry7.configure(justify=CENTER)
        self.Entry7.configure(selectbackground="#c4c4c4")
        self.Entry7.configure(selectforeground="black")
        READONLY = 'readonly'
        self.Entry7.configure(state=READONLY)
        self.Entry7.configure(textvariable=gui_support.tst)

        self.TLabelframe5 = ttk.Labelframe(self.TNotebook1_pg3)
        self.TLabelframe5.place(relx=0.58, rely=0.04, relheight=0.19
                , relwidth=0.37)
        self.TLabelframe5.configure(text='''Cavo in RJ45''')
        self.TLabelframe5.configure(width=210)

        self.Checkbutton1 = Checkbutton(self.TLabelframe5)
        self.Checkbutton1.place(relx=0.12, rely=0.39, relheight=0.28
                , relwidth=0.35)
        self.Checkbutton1.configure(activebackground="#d9d9d9")
        self.Checkbutton1.configure(activeforeground="#000000")
        self.Checkbutton1.configure(background=_bgcolor)
        self.Checkbutton1.configure(disabledforeground="#a3a3a3")
        self.Checkbutton1.configure(foreground="#000000")
        self.Checkbutton1.configure(highlightbackground="#d9d9d9")
        self.Checkbutton1.configure(highlightcolor="black")
        self.Checkbutton1.configure(justify=LEFT)
        self.Checkbutton1.configure(state=DISABLED)
        self.Checkbutton1.configure(text='''presente''')
        self.Checkbutton1.configure(variable=gui_support.cavo)

        self.Button13 = Button(self.TLabelframe5)
        self.Button13.place(relx=0.6, rely=0.33, height=34, width=65)
        self.Button13.configure(activebackground="#d9d9d9")
        self.Button13.configure(activeforeground="#000000")
        self.Button13.configure(background=_bgcolor)
        self.Button13.configure(command=self.cavo)
        self.Button13.configure(disabledforeground="#a3a3a3")
        self.Button13.configure(foreground="#000000")
        self.Button13.configure(highlightbackground="#d9d9d9")
        self.Button13.configure(highlightcolor="black")
        self.Button13.configure(pady="0")
        self.Button13.configure(text='''Leggi''')
        self.Button13.configure(width=65)

        self.Label1 = Label(master)
        self.Label1.place(relx=0.03, rely=0.02, height=21, width=559)
        self.Label1.configure(activebackground="#f9f9f9")
        self.Label1.configure(activeforeground="black")
        self.Label1.configure(background=_bgcolor)
        self.Label1.configure(disabledforeground="#a3a3a3")
        self.Label1.configure(foreground="#000000")
        self.Label1.configure(highlightbackground="#d9d9d9")
        self.Label1.configure(highlightcolor="black")
        self.Label1.configure(textvariable=gui_support.Messaggio)



    def Eco(self):
            print('self.Eco')
            sys.stdout.flush()

    def apriFTDI(self):
            print('self.apriFTDI')
            sys.stdout.flush()

    def apriSeriale(self):
            print('self.apriSeriale')
            sys.stdout.flush()

    def apriSocket(self):
            print('self.apriSocket')
            sys.stdout.flush()

    def cavo(self):
            print('self.cavo')
            sys.stdout.flush()

    def ecoProva(self,p1):
            print('self.ecoProva')
            sys.stdout.flush()

    def inventa_cp(self):
            print('self.inventa_cp')
            sys.stdout.flush()

    def inventa_cs(self):
            print('self.inventa_cs')
            sys.stdout.flush()

    def leggi_cp(self):
            print('self.leggi_cp')
            sys.stdout.flush()

    def leggi_cs(self):
            print('self.leggi_cs')
            sys.stdout.flush()

    def scrivi_cp(self):
            print('self.scrivi_cp')
            sys.stdout.flush()

    def scrivi_cs(self):
            print('self.scrivi_cs')
            sys.stdout.flush()

    def tst_lgg(self):
            print('self.tst_lgg')
            sys.stdout.flush()

    def tst_zero(self):
            print('self.tst_zero')
            sys.stdout.flush()





if __name__ == '__main__':
    vp_start_gui()



