#!/usr/bin/env python
# Plot a graph of Data which is comming in on the fly
# uses pylab
# Author: Norbert Feurle
# Date: 12.1.2012
# License: if you get any profit from this then please share it with me
import pylab
from pylab import *
import Tkinter as Tk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
#from user import PID
execfile("PID.py")
import math
import serial
import time

p_y = PID(.002,.0005,.0005,0,0,10000,-10000)
p_y.setPoint(0)

root = Tk.Tk()
root.wm_title("Embedding in TK")


ser = serial.Serial('COM2', 115200)
points = 200
t = [time.time()]
ax = [0]
ay = [0]
az = [0]
gx = [0]
gy = [0]
gz = [0]

c1 = 0
c2 = [0]
c3 = 0
c4 = 0

m1 = 0
m2 = 0
m3 = 0
m4 = 0

j = 0


xAchse=pylab.arange(0,points,1)
yAchse=pylab.array([0]*points)

fig = pylab.figure(1, figsize = (8,8))
a1 = fig.add_subplot(211)
a1.grid(True)
a1.set_title("Accelerometer")
a1.set_xlabel("Time")
a1.set_ylabel("Acceleration (g)")
a1.axis([0,points,-1.5,1.5])
line1,line2,line3 =a1.plot(t,ax,t,ay,t,az)

#fig = pylab.figure(2)
a2 = fig.add_subplot(212)
a2.grid(True)
a2.set_title("Gyros")
a2.set_xlabel("Time")
a2.set_ylabel("Angular Velocity (deg/s)")
a2.axis([0,points,-1.5,1.5])
line4,line5,line6,line7 =a2.plot(t,gx,t,gy,t,gz,t,c2)

#manager = pylab.get_current_fig_manager()

values=[]
values = [0 for x in range(points)]

Ta=0.01
fa=1.0/Ta
fcos=3.5

Konstant=cos(2*pi*fcos*Ta)
T0=1.0
T1=Konstant


canvas = FigureCanvasTkAgg(fig, master=root)
canvas.show()
canvas.get_tk_widget().pack(side=Tk.TOP, fill=Tk.BOTH, expand=1)
canvas._tkcanvas.pack(side=Tk.TOP, fill=Tk.BOTH, expand=1)
#button = Tk.Button(master=root, text='Quit', command=sys.exit)
#button.pack(side=Tk.BOTTOM)


def SinwaveformGenerator(arg):
	#print time.time()*1000
	global values,T1,Konstant,T0,c1,c2,c3,c4,p_y,j
	data = ser.readline().rstrip() # read data from serial # port and strip line endings
	#print data
	#print data
	ser.flushInput()
	if len(data.split(",")) == 15:
		sp = data.split(",")
		try:
			float(sp[0])
			float(sp[1])
			float(sp[2])
			float(sp[3])
			float(sp[4])
			float(sp[5])
			float(sp[6])
			float(sp[7])
			float(sp[8])
			float(sp[9])
			float(sp[10])
			
			t.append(time.time())
			ax.append(float(sp[1]))
			ay.append(float(sp[2]))
			az.append(float(sp[3]))	
			gx.append((float(sp[4])+225.5)*500/655.35) 	# 100thdeg/sec
			gy.append((float(sp[5])-144.0)*500/655.35)	# 100thdeg/sec	
			gz.append((float(sp[6])+24.57)*500/655.35)	# 100thdeg/sec
			
			c1 = float(sp[7])
			c2.append(float(sp[8])*20)
			c3 = float(sp[9])
			c4 = float(sp[10])
			
			if len(t) > points:
				del t[0]
				del ax[0]
				del ay[0]		
				del az[0]
				del gx[0]
				del gy[0]		
				del gz[0]
				del c2[0]
		except ValueError:
			print "Not a float"
			#print data
	# PID Loops
	p_y.setPoint(c2[-1])
	#print c2[-1]
	y_rate_cmd = p_y.update(gy[-1])
	print y_rate_cmd
	m2 = c1 - y_rate_cmd
	m4 = c1 + y_rate_cmd
	
	#m2 = c1 - c2[-1]/100
	#m4 = c1 + c2[-1]/100
	
	m2 = min(m2,1000)
	m2 = max(m2,0)
	m4 = min(m4,1000)
	m4 = max(m4,0)
	#print m2
	#print m4
	#print ("b"+str(m2)+"b")
	#print ("d"+str(m4)+"d")
	# Send Motor Commands
	ser.write("a"+str(int(m1))+"a")
	ser.write("b"+str(int(m2))+"b")
	ser.write("c"+str(int(m3))+"c")
	ser.write("d"+str(int(m4))+"d")
	#print time.time()*1000



def RealtimePloter(arg):
  global values
  #CurrentXaxis=pylab.arange(len(ax)-points,len(ax),1)
  #print CurrentXaxis
  line1.set_data(t,ax)
  line2.set_data(t,ay)
  line3.set_data(t,az)
  line4.set_data(t,gx)
  line5.set_data(t,gy)
  line6.set_data(t,gz)
  line7.set_data(t,c2)
  a1.axis([min(t),max(t),-30000,30000])
  a2.axis([min(t),max(t),-23000,23000])
  canvas.show()
  #manager.canvas.draw()
  #print time.time()
  #manager.show()

timer = fig.canvas.new_timer(interval=30)
timer.add_callback(RealtimePloter, ())
timer2 = fig.canvas.new_timer(interval=20)
timer2.add_callback(SinwaveformGenerator, ())
timer.start()
timer2.start()

#pylab.show()
Tk.mainloop()