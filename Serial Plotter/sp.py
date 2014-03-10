import serial
import numpy as np
import threading
from matplotlib import pyplot as plt
import time

ser = serial.Serial('COM16', 115200)
 
plt.ion() # set plot to animated
 
ydata1 = [0] * 100
ydata2 = [0] * 100
ydata3 = [0] * 100
ydata4 = [0] * 100
ydata5 = [0] * 100
ydata6 = [0] * 100
tdata  = []
i = 0
for i in range(0,100):
	tdata.append(i)
ax1=plt.axes()  
 
# make plot
plt.subplot(211)
line1, line2, line3, = plt.plot(tdata,ydata1,tdata,ydata2,tdata,ydata3)
plt.legend(('m1','m2','m3','m4'), loc='upper left')
plt.ylim([-100000,100000])
plt.xlim([0,49])
plt.subplot(212)
line4, line5, line6, = plt.plot(tdata,ydata4,tdata,ydata5,tdata,ydata6)
plt.legend(('a_pitch','g_pitch','pitch'), loc='upper left')
plt.ylim([-100000,100000])
plt.xlim([0,49])
 
# start data collection
max_sample = 10
sample = 1

# start time
start_time = int(round(time.time()*1000))

try:
    while True:  
		data = ser.readline().rstrip() # read data from serial 
									   # port and strip line endings
		if len(data.split(",")) == 7:
			sample = sample + 1
			if sample > max_sample:
				sample = 0
			if sample == max_sample:
				#ymin = float(min(ydata))-10
				#ymax = float(max(ydata))+10
				#plt.ylim([ymin,ymax])
				line = data.split(",")
				ydata1.append(line[1])
				ydata2.append(line[2])
				ydata3.append(line[3])
				ydata4.append(line[4])
				ydata5.append(line[5])
				ydata6.append(line[6])
				del ydata1[0]
				del ydata2[0]
				del ydata3[0]
				del ydata4[0]
				del ydata5[0]
				del ydata6[0]
				#line.set_xdata(np.arange(len(ydata1)))
				line1.set_ydata(ydata1)  # update the data
				line2.set_ydata(ydata2)  # update the data
				line3.set_ydata(ydata3)  # update the data
				line4.set_ydata(ydata4)  # update the data
				line5.set_ydata(ydata5)  # update the data
				line6.set_ydata(ydata6)  # update the data
				if int(round(time.time()*1000))-start_time > 100:
					plt.draw() # update the plot
					start_time = start_time + 100;
			

except KeyboardInterrupt:
	plt.close()