%calculo do filtro LCL

%_________________________________________________________%



%caracaterística do inversor

%%Q = 600e3; %Potencia reativa 600 kVAR
Fp_min = 0.995; % fator de potência


%Vg = 480 * sqrt(3); %tensão da rede de linha (RMS) % lado baixa  do trafo   (O erro estava aqui !!! estava utilizando só 480 V)
Vg = 2401.78;

%e = 2401.78;  % Tensão de linha eficaz da rede %lado de alta do trafo
fn = 60; % frequencia da rede
wn = 2 * pi * fn; %frequencia angular da rede
f_sw = 6000; %frequencia de chaveamento
Vdc = 1015;  %  tensão do barramento CC

%calculo de S e P   do inversor
%%theta = acos(Fp_min);  %angulo theta
%%S = Q / (sin(theta));  %Potencia aparente S [VA]
%S=600e3;
S=2e6;
P = S * Fp_min; % Potencia ativa P [W]

%calculo da impedancia base Zb
%Zb = e^2 / (P);

%Zb = Vg^2 / (P);

Zb = Vg^2 / S;

%indutãncia base Lb
Lb = Zb / (2* pi * fn);

%capacitância base Cb
Cb = 1/(Zb * 2 * pi * fn);

%corrente nominal In
In = S / (sqrt(3) * Vg);

% relação entre as frequencias mf
mf = f_sw / fn;

%  fator m 
t1 = Vg^2 /3;
%lt1 = 5.3408e-05;           % adicionar aqui o valor da indutancia total Lt escolhida
%%lt1 = 6.3560e-05;

%lt1=1.9068e-04;
lt1=0.477408;

t2 = (wn * lt1 * In)^2;

m = (sqrt(3)/Vdc) * (sqrt(t1 + t2));

%modelagem dos componentes do filtro - Método 2

%Passo 1 definição de rf   rf = f_sw / f_res
%rf = 3;
%rf = 3.12  %appropriate ratio should be around rf ≈ 4.5. For grid current control with rf < 6 [26], no active damping method is necessary to achieve stability
rf = 3.12


%Passo 2: definição de rl  Lf/Lg
rl = 1 %minimiza o valor dos indutores

%Passo 3: definição da capacitância (função da potência reativa)

%In order to select the proper rq, the power factor should be less than 0.995 and the grid current THD
%should be below limit 5% and have sufficient margin to
%account for the nonmodeled elements. 

%rq = 1  %rq > 1 para reduzir as indutâncias necessárias  % The minimum rq = 1, with no reactive power consumption
%rq = 7.1  %(melhor resultado obtido para o filtro, o valor de rq que está relacionado com THD e a potência Q)
rq = 1

%Passo 4 : cálculos


x = [1:0.01:10]; %vetor com os valores de rq
t=1;
lt_vet =zeros(1,901);
ig_vet = zeros(1,901);
fp_vet = zeros(1,901);
C_vet = zeros(1,901);

%THD
f_m = (3/2 * m^2) - (3*sqrt(3)/pi) + (9/8) * (3/2 - ((9/8)* (sqrt(3)/pi)*m^4)); %eq 25 (parte dentro da raiz)

for(a=1:1:901)

%indutancia  lt (PU)
%lt = rf * (fn/f_sw) * ((1+rl)/(sqrt(rl * rq)));  %lt (PU)
aux = rf * (fn/f_sw) * ((1+rl)/(sqrt(rl * x(a))));  %lt (PU)   %eq. 23

%Lt = lt * Zb;   %lt (H)
%Lt = aux * Zb;           %estava errado pois estava utilizando Zb e
%deveria ser Lb

%Lt = lt * Lb;   %lt (H)
Lt = aux * Lb;

%vetor com as indutâncias (mH)
lt_vet(a) =  Lt * 1000;

%vetor com os valores de capacitância
%aux1 = Lt * rq;

%C_vet(a) = aux1 * Cb;

%corrente THD eq. 28
a1 = (pi * Vdc)/(12*Zb);
a2 = sqrt(x(a)) / (rf^3);
a3 = sqrt(rl) / (1 + rl);
a4 = 1/(((1 - 6/mf)^2) - 1/(rf^2));
ig_vet(a) = a1 * a2 * a3 * a4 * f_m;   %eq. 28

%fator de potência FP
b1 = (x(a) - 1)/(sqrt(x(a)));
b2 = (1 + rl)/sqrt(rl);
b3 = (rf *fn)/f_sw;
q = b1* b2 * b3;

%fp_vet(a) = 1 - (0.5 * q^2); %eq. 20 aproximada
fp_vet(a) = 1 / (sqrt(1 + q^2)); %eq. 20

end

Lf = Lt/2;   % Lf = Lg = Lt/2

%fator de potência

%q = ((rq -1)/(sqrt(rq))) * ((1+rl)/sqrt(rl)) * rf * (fn/f_sw); %pot. reativa do filtro (PU)

FP = 1  - (q^2) / 2;

%calculo da frequencia de ressonância  %deve estar entre 600 e 3000
f_res = f_sw / rf;

%calculo da capacitância Cf  - relacionada com a potencia Q (5 %)

%Cf = 0.05 * (P / (2 * pi * fn * e^2))
%fb =fn;
%wb = 2 * pi * fb;

%Cb = 1/(Zb * wb)

%Cf = 0.05 * (P / (2 * pi * fn * Vg^2))

%cálculo correto utilizando a fórmula: 

%Para  rq = 1:

%encontra o indice do valor desejado para rq no vetor
%n=1; % valor desejado de Lt
%k = find(abs(lt_vet-n) < 0.01);
%ind = min(k);  %indice de menor valor contendo a indutância desejada

ind = 1;


Lt = lt_vet(ind) * 10^-3    % Lt [H]      indutância total

Lg = Lt/2    % L = Lg = Lf 

%rq = cf/lT = (Zb^2 * Cf) / Lt
C = (rq * Lt) / Zb^2  % eq. pág 5/12      C [F]


%Resistor de amortecimento passivo (resistor de amortecimento minimo para estabilidade)
%Rd = (f_sw * Lg^2)/(3*(L+Lg))  (L+Lg = Lt)
Rd = (f_sw * Lg^2)/(3*(Lt))  % 


 %plot dos gráficos
 %grafico de rq x L
 figure (1)
 plot(x,lt_vet)
 grid on
 title('Indutância total - Lt')
 xlabel('r_q') 
 ylabel('Lt [mH]') 
 legend('Lt')

 %grafico de Ig  THD
 figure (2)
 plot(x,(ig_vet/In)*100)
 grid on
 legend('ig THD')

 %grafico de FP
 figure (3)
 plot(x,fp_vet)
 grid on
 title('Fator de Potência')
 xlabel('r_q') 
 ylabel('FP') 
 legend('FP')

 %gráficos juntos

 figure(4)

fig = figure;
left_color = [0 0 1];
right_color = [0 0.35 0];
set(fig,'defaultAxesColorOrder',[left_color; right_color]);


 yyaxis left
 plot(x,lt_vet,'Color',left_color)
 xlabel('r_q') 
 ylabel('L_t (mH)','Color',left_color) 
 yyaxis right
 plot(x,(ig_vet/In)*100,'Color',right_color)
 ylabel('i_g THD (%)','Color',right_color) 
 grid on
 legend('Lt','Ig THD')

% hold on 
% plot(x,fp_vet)
%  hold off

 %Gráfico com os três eixos
 %figure(5)
 ylabels{1}='Lt [mH]';
 ylabels{2}='Fator de Potência';
 ylabels{3}='i_g THD (%)';
 [ax,hlines] = plotyyy(x,lt_vet,x,fp_vet,x,(ig_vet/In)*100,ylabels);
 %legend(hlines,'Lt','FP','i_g THD',2)

 

%https://www.mathworks.com/matlabcentral/fileexchange/1017-plotyyy
 %https://www.mathworks.com/help/matlab/ref/yyaxis.html
 % https://www.mathworks.com/help/matlab/creating_plots/graph-with-multiple-x-axes-and-y-axes.html