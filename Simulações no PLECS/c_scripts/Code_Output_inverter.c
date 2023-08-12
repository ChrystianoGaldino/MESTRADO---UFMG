//..............Contador Para a interrupção e PWM
count = count + inc;

if(count == PRD) inc = -1;
if(count == 0) inc = 1;

//............................................................Interrupção........................................................................................................
if((count == 0) || (count == PRD))
{

////////////////// Variaveis locais para o controle da tensão do barramento CC //////////////////

float Qref = Q;   //Variaveis que recebem o valor constante que vem fora do C-Script
float psat = P;
//float PotN = Pn;  //potencia de referencia do controle de P ativa (tese pag. 59)

//////////////////////////////////////////////////////////COntrole de tensão do link cc///////////////////////////////////////////////////////////////////////qq
PIvdc.Xref = Vdc_ref*Vdc_ref;
PIvdc.Xm = Vdc*Vdc;                                               //Corrente medida para o modo boost (Descarga)

PIvdc.erro = PIvdc.Xref - PIvdc.Xm;

PIvdc.erropi = PIvdc.erro - (1/Kpouter)*PIvdc.dif;

PIvdc.inte = PIvdc.inte_ant + Ts/2 * (PIvdc.erropi  + PIvdc.erropi_ant);
PIvdc.inte_ant = PIvdc.inte;
PIvdc.erropi_ant = PIvdc.erropi;

PIvdc.piout = (Kpouter*PIvdc.erro + Kiouter*PIvdc.inte); 

PIvdc.piout_sat = PIvdc.piout;
if(PIvdc.piout>psat) PIvdc.piout_sat = psat;
if(PIvdc.piout<-psat) PIvdc.piout_sat= -psat;

PIvdc.dif = PIvdc.piout - PIvdc.piout_sat;

////////////////////////////////////////////////////////Transformada abc para alfa-beta da tensão da rede/////////////////////////////////////
Vabc.a = Vga;
Vabc.b = Vgb;
Vabc.c = Vgc;

Valfabeta.alfa = 0.816496580927726 * (Vabc.a - 0.5*Vabc.b - 0.5*Vabc.c);
Valfabeta.beta = 0.816496580927726 * (0.866025403784439*Vabc.b - 0.866025403784439*Vabc.c);

//Valfabeta.alfa = Alp;   //teste de alpha e beta externos para ver se o erro estava na transformada alp e bet implementada acima
//Valfabeta.beta = Bta;

////////////////////////////////////////////////////////DSOGI-PLL////////////////////////////////////////////////////////////////////////////////
//SOGI1.freq_res = PIpll.piout/(2*pi);  //Adaptativo

SOGI1.freq_res = 60; 

////Sogi para a componente alfa da tensão
SOGI1.Vm = Valfabeta.alfa;

SOGI1.x =  4*Ts*pi * SOGI1.K_damp *  SOGI1.freq_res;
SOGI1.y = (Ts*2*pi * SOGI1.freq_res)*(Ts*2*pi * SOGI1.freq_res);

SOGI1.b1 = SOGI1.x + SOGI1.y + 4;
SOGI1.b2 = SOGI1.x - SOGI1.y - 4;
SOGI1.b3 = 2*(4 - SOGI1.y);

SOGI1.b0 = SOGI1.x * (1/SOGI1.b1);
SOGI1.a1 = SOGI1.b3 * (1/SOGI1.b1);
SOGI1.a2 = SOGI1.b2 * (1/SOGI1.b1);

SOGI1.W = 4*Ts*pi * SOGI1.freq_res;

SOGI1.V_sogi = SOGI1.b0 * SOGI1.Vm - SOGI1.b0 * SOGI1.Vm2 + SOGI1.a1 * SOGI1.V_sogi1 + SOGI1.a2 * SOGI1.V_sogi2;

SOGI1.V_sogi_q = SOGI1.W * SOGI1.b0 * SOGI1.Vm1 + SOGI1.V_sogi_q1 * SOGI1.a1 + SOGI1.V_sogi_q2 * SOGI1.a2;
	
SOGI1.Vm2 = SOGI1.Vm1;
SOGI1.Vm1 = SOGI1.Vm;
	
SOGI1.V_sogi2 = SOGI1.V_sogi1;
SOGI1.V_sogi1 = SOGI1.V_sogi;
	
SOGI1.V_sogi_q2 = SOGI1.V_sogi_q1;
SOGI1.V_sogi_q1 = SOGI1.V_sogi_q;

////Sogi para a componente beta da tensão
SOGI2.Vm = Valfabeta.beta;

SOGI2.x =  4*Ts*pi * SOGI2.K_damp *  SOGI2.freq_res;
SOGI2.y = (Ts*2*pi * SOGI2.freq_res)*(Ts*2*pi * SOGI2.freq_res);

SOGI2.b1 = SOGI2.x + SOGI2.y + 4;
SOGI2.b2 = SOGI2.x - SOGI2.y - 4;
SOGI2.b3 = 2*(4 - SOGI1.y);

SOGI2.b0 = SOGI2.x * (1/SOGI2.b1);
SOGI2.a1 = SOGI2.b3 * (1/SOGI2.b1);
SOGI2.a2 = SOGI2.b2 * (1/SOGI2.b1);

SOGI2.W = 4*Ts*pi * SOGI2.freq_res;

SOGI2.V_sogi = SOGI2.b0 * SOGI2.Vm - SOGI2.b0 * SOGI2.Vm2 + SOGI2.a1 * SOGI2.V_sogi1 + SOGI2.a2 * SOGI2.V_sogi2;

SOGI2.V_sogi_q = SOGI2.W * SOGI2.b0 * SOGI2.Vm1 + SOGI2.V_sogi_q1 * SOGI2.a1 + SOGI2.V_sogi_q2 * SOGI2.a2;
	
SOGI2.Vm2 = SOGI2.Vm1;
SOGI2.Vm1 = SOGI2.Vm;
	
SOGI2.V_sogi2 = SOGI2.V_sogi1;
SOGI2.V_sogi1 = SOGI2.V_sogi;
	
SOGI2.V_sogi_q2 = SOGI2.V_sogi_q1;
SOGI2.V_sogi_q1 = SOGI2.V_sogi_q;

/////////////////////////////////////////////     PLL   /////////////////////////////////////////////
// Cálculo da Seq positiva    (based on DSOGI-QSG)
//PLL.Valfa_in = 0.5*(SOGI1.V_sogi-SOGI2.V_sogi_q);
//PLL.Vbeta_in = 0.5*(SOGI2.V_sogi+SOGI1.V_sogi_q);


PLL.Valfa_in = Alp;  //teste de injeção de alp e beta externos para ver se o erro  está no calculo da seq positiva acima
PLL.Vbeta_in = Bta;

//Calculo de Alpha e Beta conforme o bloco do PLECS (o qual funciona corretamente)
//PLL.Valfa_in = (1/3)*(2*Vga -Vgb -Vgc);
//PLL.Vbeta_in = (1/sqrt(3))*(Vgb -Vgc);

// Transformada alfabeta-dq da tensão
Vdq.d = PLL.Valfa_in*cos(PLL.angle) + PLL.Vbeta_in*sin(PLL.angle);
Vdq.q = PLL.Vbeta_in*cos(PLL.angle) - PLL.Valfa_in*sin(PLL.angle);

// Normalização pelo pico da tensão da rede
PIpll.erropi = Vdq.q/(sqrt(Vdq.q*Vdq.q + Vdq.d*Vdq.d) + 1e-2);

// Controlador PI da PLL
PIpll.inte = PIpll.inte_ant + Ts/2 * (PIpll.erropi  + PIpll.erropi_ant);
PIpll.inte_ant = PIpll.inte;
PIpll.erropi_ant = PIpll.erropi;

PIpll.piout = Kp_pll*PIpll.erropi + Ki_pll*PIpll.inte + PLL.wf; 

// Integrador da PLL para o cálculo do ângulo. Método de integração (Foward)
PLL.angle = PLL.angle_ant + Ts*PLL.pi_out_ant;
PLL.pi_out_ant = PIpll.piout;
if(PLL.angle > 6.283185307179586)  PLL.angle -= 6.283185307179586;
if(PLL.angle < 0.0)  PLL.angle += 6.283185307179586;
PLL.angle_ant = PLL.angle;

/////////////////////////////////////////////////////////////Teoria da potência instantânea//////////////////////////////////
Ialfabeta.alfa = (PLL.Valfa_in*(-PIvdc.piout_sat) + Qref*PLL.Vbeta_in)/(PLL.Valfa_in*PLL.Valfa_in + PLL.Vbeta_in*PLL.Vbeta_in + 1e-2);
Ialfabeta.beta = (PLL.Vbeta_in*(-PIvdc.piout_sat) - Qref*PLL.Valfa_in)/(PLL.Valfa_in*PLL.Valfa_in + PLL.Vbeta_in*PLL.Vbeta_in + 1e-2);

//teste
//Ialfabeta.alfa = (PLL.Valfa_in*(-PotN) + Qref*PLL.Vbeta_in)/(PLL.Valfa_in*PLL.Valfa_in + PLL.Vbeta_in*PLL.Vbeta_in + 1e-2);
//Ialfabeta.beta = (PLL.Vbeta_in*(-PotN) - Qref*PLL.Valfa_in)/(PLL.Valfa_in*PLL.Valfa_in + PLL.Vbeta_in*PLL.Vbeta_in + 1e-2);

 //saturalção da corrente
if(Ialfabeta.alfa>Ir) Ialfabeta.alfa = Ir;
if(Ialfabeta.alfa<-Ir) Ialfabeta.alfa = -Ir;
if(Ialfabeta.beta>Ir) Ialfabeta.beta = Ir;
if(Ialfabeta.beta<-Ir) Ialfabeta.beta = -Ir;

////////////////////////////////////////////////////////Transformada abc para alfa-beta da corrente do inversor/////////////////////////////////////
Isabc.a = Isa;
Isabc.b = Isb;
Isabc.c = Isc;
                         //transformação invariante em potência
Isalfabeta.alfa = 0.816496580927726 * (Isabc.a - 0.5*Isabc.b - 0.5*Isabc.c);
Isalfabeta.beta = 0.816496580927726 * (0.866025403784439*Isabc.b - 0.866025403784439*Isabc.c);


/////////////////////////////////////////////////////////Controladores Proporcional Ressonante////////////////////////////////////////////////////////////

// Componente Alfa
PRf_alfa.Xref = Ialfabeta.alfa;
PRf_alfa.Xm = Isalfabeta.alfa;

PRf_alfa.erro = PRf_alfa.Xref - PRf_alfa.Xm;

PRf_alfa.res = PRf_alfa.c1*PRf_alfa.erro + PRf_alfa.c2*PRf_alfa.erro_ant2 - PRf_alfa.c3*PRf_alfa.res_ant - PRf_alfa.c4*PRf_alfa.res_ant2;
PRf_alfa.res_ant2 = PRf_alfa.res_ant;
PRf_alfa.res_ant = PRf_alfa.res;
PRf_alfa.erro_ant2 = PRf_alfa.erro_ant;
PRf_alfa.erro_ant = PRf_alfa.erro;

if(counti<10) PRf_alfa.pr_out = Kp_res*PRf_alfa.erro + Ki_res*PRf_alfa.res;
if(counti>=10)  PRf_alfa.pr_out = Kp_res*PRf_alfa.erro + Ki_res*PRf_alfa.res + PLL.Valfa_in;

// Componente Beta
PRf_beta.Xref = Ialfabeta.beta;
PRf_beta.Xm = Isalfabeta.beta;

PRf_beta.erro = PRf_beta.Xref - PRf_beta.Xm;

PRf_beta.res = PRf_beta.c1*PRf_beta.erro + PRf_beta.c2*PRf_beta.erro_ant2 - PRf_beta.c3*PRf_beta.res_ant - PRf_beta.c4*PRf_beta.res_ant2;
PRf_beta.res_ant2 = PRf_beta.res_ant;
PRf_beta.res_ant = PRf_beta.res;
PRf_beta.erro_ant2 = PRf_beta.erro_ant;
PRf_beta.erro_ant = PRf_beta.erro;

if(counti<10) 
{ 
counti++;
PRf_beta.pr_out = Kp_res*PRf_beta.erro + Ki_res*PRf_beta.res;
}

if(counti>=10)  PRf_beta.pr_out = Kp_res*PRf_beta.erro + Ki_res*PRf_beta.res + PLL.Vbeta_in;

////////////////////////////////////////////////Transformada abc para alfabeta da tensão de referência////////////////////////////////////
Vpwm_alfabeta.alfa = PRf_alfa.pr_out;
Vpwm_alfabeta.beta = PRf_beta.pr_out;

Vpwm_abc.a = 0.816496580927726*Vpwm_alfabeta.alfa;
Vpwm_abc.b = 0.816496580927726*(-0.5*Vpwm_alfabeta.alfa + 0.866025403784439*Vpwm_alfabeta.beta);
Vpwm_abc.c = 0.816496580927726*(-0.5*Vpwm_alfabeta.alfa - 0.866025403784439*Vpwm_alfabeta.beta);

////////////////////////////////////////////////Normalização pela tensão do barramento cc////////////////////////////////////
Vpwm_norm_a = Vpwm_abc.a*1.732050807568877/Vdc;
Vpwm_norm_b = Vpwm_abc.b*1.732050807568877/Vdc;
Vpwm_norm_c = Vpwm_abc.c*1.732050807568877/Vdc;

if(Vpwm_norm_a > 1) Vpwm_norm_a = 1;
if(Vpwm_norm_a < -1) Vpwm_norm_a = -1;
if(Vpwm_norm_b > 1) Vpwm_norm_b = 1;
if(Vpwm_norm_b < -1) Vpwm_norm_b = -1;
if(Vpwm_norm_c > 1) Vpwm_norm_c = 1;
if(Vpwm_norm_c < -1) Vpwm_norm_c = -1;

//Cálculo da seq zero para o SVPWM
if(Vpwm_norm_a<Vpwm_norm_b && Vpwm_norm_a<Vpwm_norm_c && Vpwm_norm_b>Vpwm_norm_c) 
{
  vmin = Vpwm_norm_a;
  vmax = Vpwm_norm_b;
}  
else if(Vpwm_norm_a<Vpwm_norm_b && Vpwm_norm_a<Vpwm_norm_c && Vpwm_norm_c>Vpwm_norm_b) 
{
  vmin = Vpwm_norm_a;
  vmax = Vpwm_norm_c;
}  
else if(Vpwm_norm_b<Vpwm_norm_a && Vpwm_norm_b<Vpwm_norm_c && Vpwm_norm_a>Vpwm_norm_c) 
{
  vmin = Vpwm_norm_b;
  vmax = Vpwm_norm_a;
}  
else if(Vpwm_norm_b<Vpwm_norm_a && Vpwm_norm_b<Vpwm_norm_c && Vpwm_norm_c>Vpwm_norm_a) 
{
  vmin = Vpwm_norm_b;
  vmax = Vpwm_norm_c;
} 
else if(Vpwm_norm_c<Vpwm_norm_a && Vpwm_norm_c<Vpwm_norm_b && Vpwm_norm_a>Vpwm_norm_b) 
{
  vmin = Vpwm_norm_c;
  vmax = Vpwm_norm_a;
} 
else if(Vpwm_norm_c<Vpwm_norm_a && Vpwm_norm_c<Vpwm_norm_b && Vpwm_norm_b>Vpwm_norm_a) 
{
  vmin = Vpwm_norm_c;
  vmax = Vpwm_norm_b;
} 

vsa_svpwm = -0.5*(vmin+vmax)+Vpwm_norm_a;
vsb_svpwm = -0.5*(vmin+vmax)+Vpwm_norm_b;
vsc_svpwm = -0.5*(vmin+vmax)+Vpwm_norm_c;

dutya = PRD_div2 + 2/sqrt(3)*vsa_svpwm*PRD_div2;
dutyb = PRD_div2 + 2/sqrt(3)*vsb_svpwm*PRD_div2;
dutyc = PRD_div2 + 2/sqrt(3)*vsc_svpwm*PRD_div2;

} // fecha a interrupção

///////////////////////////////PWM//////////////////////////////////////////////
if(dutya>=count)
{
  S1 = 1;
  S2 = 0;
}
else
{
  S1 = 0;
  S2 = 1;
}

if(dutyb>=count)
{
  S3 = 1;
  S4 = 0;
}
else
{
  S3 = 0;
  S4 = 1;
}

if(dutyc>=count)
{
  S5 = 1;
  S6 = 0;
}
else
{
  S5 = 0;
  S6 = 1;
}

Output(0) = S1;
Output(1) = S2;
Output(2) = S3;
Output(3) = S4;
Output(4) = S5;
Output(5) = S6;
Output(6) = PRf_alfa.Xm;
Output(7) = PRf_alfa.Xref;
Output(8) = PRf_beta.Xm;
Output(9) = PRf_beta.Xref;
Output(10) = Vdc_ref;
Output(11) = Vdc;
Output(12) = PIvdc.piout_sat;
Output(13) = PLL.angle;
Output(14) = PLL.Valfa_in;
Output(15) = PLL.Vbeta_in;