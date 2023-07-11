%% Analysis and simulation of Passive suspension system using MATLAB 7.5
%% (R2207b)using State Spce equations

Mse=90; % Mass of seat
Ms=250;% Mass of sprung
Mu=40;% Mass of Unsprung
bs=2000; %Damping ratio for sprung mass
bse=3000;%Damping ratio for seat mass
Kt=125000;%Stiffness of Unsprung mass
Ks=28000;%Stiffness of sprung mass
Kse=8000;%Stiffness of seat mass
Zr=0.2;% road disturbance

%% State Space Equation

%Coefficient A
A=[0,1,0,0,0,0;-Kse/Mse,-bse/Mse,Kse/Mse,bse/Mse,0,0;0,0,0,1,0,0;Kse/Ms,bse/Ms,-Kse/Ms-Ks/Ms,-bse/Ms-bs/Ms,Ks/Ms,bs/Ms;0,0,0,0,0,1;...
0,0,Ks/Mu,bs/Mu,-Ks/Mu-Kt/Mu,-bs/Mu];

%%Coeiffcient B
B=[0;0;0;0;0;Kt/Mu];

%C coefficient of displacement analysis
C1=[1,0,0,0,0,0];
C2=[0,0,1,0,0,0];
C3=[0,0,0,0,1,0];

%C coefficient for velocity analysis

v1=[0,1,0,0,0,0];
v2=[0,0,0,1,0,0];
v3=[0,0,0,0,0,1];

% coefficeint D

D=[0];

%% Vertical Displacement response of the suspension system 

dse=ss(A,Zr*B,C1,D);%Seat output equation
ds=ss(A,Zr*B,C2,D);%sprung output equation
du=ss(A,Zr*B,C3,D);%Unsprung output equation

%% Vertical Velocity response of the suspension system  

vse=ss(A,Zr*B,v1,D);
vs=ss(A,Zr*B,v2,D);
vu=ss(A,Zr*B,v3,D);

%% Velocity and displacement response of seat

step(dse,vse)
axis([0 10 -1 1.5])
legend('Displacement','velocity');
title('Velocity and displacement response of seat')


%% Velocity and displacement response of Spung Mass
figure;
step(ds,vs)
axis([0 10 -1 2])
legend('Displacement','velocity');
title('Velocity and displacement response of Spung Mass')

%% Velocity and displacement response of Unspung Mass
figure;
step(du,vu)
axis([0 1 -2 7])
legend('Displacement','velocity');
title('Velocity and displacement response of UnSpung Mass')
