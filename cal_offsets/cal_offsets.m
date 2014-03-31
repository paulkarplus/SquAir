clear all;
close all;
clc;
% data = csvread('2014_03_29 C5.csv',3,0);
% data = csvread('data1.csv',0,0);
data = csvread('C5.csv',420,0);
t = data(:,1);
ax = data(:,2);
ay = data(:,3);
az = data(:,4);
gx = data(:,5);
gy = data(:,6);
gz = data(:,7);

figure(1)
plot(t,ax,t,ay,t,az);
figure(2)
plot(t,gx,t,gy,t,gz);

% gx_mean = mean(gx)
% gy_mean = mean(gy)
% gz_mean = mean(gz)
gx_mean = -225.5359;
gy_mean = 144.0359;
gz_mean = -24.5569;

gx = (gx-gx_mean)*500/655.35; % 1/100thdegrees/sec
gy = (gy-gy_mean)*500/655.35; % 1/100thdegrees/sec
gz = (gz-gz_mean)*500/655.35; % 1/100thdegrees/sec

% gx_cov = cov(gx)
% gy_cov = cov(gy)
% gz_cov = cov(gz)

figure(3)
plot(t,gx,t,gy,t,gz)

figure(4)
plotyy(t,gy,t,data(:,15));
legend('gyro','cmd')
title('Motor Step Command')
xlabel('time (ms)')

theta_y = gy*0;
for i = 2:length(gy)
    theta_y(i) = theta_y(i-1) + (t(i)-t(i-1))*(gy(i))/100/1000;
end
figure(5)
plotyy(t/1000,theta_y/180*3.14,t/1000,gy/100/180*3.14)

theta_dot_dot_y = gy*0;
for i = 2:length(gy)
    theta_dot_dot_y(i) = ((gy(i)-gy(i-1))/100)/((t(i)-t(i-1))/1000)/180*3.14; % deg/sec/sec
end
figure(6)
plot(t/1000,theta_dot_dot_y)

