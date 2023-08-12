//..............Contador Para a interrupção e PWM
count_0 = count_0 + inc;
count_120 = count_120 + inc_120;
count_240 = count_240 + inc_240;
teste = 0;

if(count_0 == PRD) inc = -1;
if(count_0 == 0) inc = 1;

if(count_120 == PRD) inc_120 = -1;
if(count_120 == 0) inc_120 = 1;

if(count_240 == PRD) inc_240 = -1;
if(count_240 == 0) inc_240 = 1;

CMPB = PRD;

//............................................................Interrupções........................................................................................................
if(control_enable == 1)
{
///////////////////////////////////////////////////Interrupção 1
if(count_0 == CMPB)
{
//////////////////////////////////////////////////////////////////////Mudança do modo de operação pelas flags/////////////////////////////////////

//verifica se o modo de Descarga está ativado de início
if(flag.DM == 1)
{
   flag.BVCM = 0;   // Reseta a flag da Tensão de boost
   
   //Rampa de corrente
   IRamp_bt.final = Iref_dis/N_br;
   IRamp_bt.in = (Ibat+Ibat2+Ibat3)/N_br;
   IRamp2_bt.final = Iref_dis/N_br;
   IRamp2_bt.in = (Ibat+Ibat2+Ibat3)/N_br;
   IRamp3_bt.final = Iref_dis/N_br;
   IRamp3_bt.in = (Ibat+Ibat2+Ibat3)/N_br;
}

//verifica se o modo de Carga está ativado de início
if(flag.CM == 1 && flag.BVCM == 0)
{
    Vref = Vboost*Nb_series;   //seta a referência de tensão para a tensão de boost
}
//verifica se o modo de descarga está acionado e se o soc atingiu o soc mínimo (Inicia a carga)
if(flag.DM == 1 && Soc < Soc_min)
{
   flag.DM = 0;               //Desabilita Descarga
   flag.CM = 1;               //Aciona o modo de carga
}

//Verifica se o CM está habilitado e se a tensão do banco de baterias atingiu a tensão de Vboost
if(flag.CM == 1 && Vbat >= Vboost*Nb_series && flag.BVCM == 0)	
{ 
  flag.BVCM = 1;   // Tensão de boost atingida
}

//Verifica se a tensão de boost foi ativada (BVCM = 1) e se a corente da bateria atingiu nos 10% ca corrente de carga inicial. Se sim, inicia o modo float
if(flag.BVCM == 1 && (Ibat+Ibat2+Ibat3) >= -0.1*Iref_ch)	
{ 
  Vref = Vfloat*Nb_series;   //seta a referência de tensão para a tensão de float
}


//Reseta para Descarga (1) e Carga (2)
if(reset == 1)
{
   flag.DM = 1;               //Desabilita Descarga
   flag.CM = 0;               //Aciona o modo de carga
}

if(reset == 2)
{
   flag.DM = 0;               //Desabilita Descarga
   flag.CM = 1;               //Aciona o modo de carga
   Vref = Vboost*Nb_series;   //seta a referência de tensão para a tensão de boost
}

//Rampa da referência de I
Ramp(&IRamp_bt);

////////////////////////////////////////////////////////////////Inicia Descarga(INT1)///////////////////////////////////////////////////////////////
if(flag.DM == 1)
{
//Rampa de corrente
PIbt.Xref = IRamp_bt.atual;

PIbt.Xm = Ibat;                                               //Corrente medida para o modo boost (Descarga)
PIbt.erro = PIbt.Xref  - PIbt.Xm;

PIbt.erropi = PIbt.erro - (1/Kpbt)*PIbt.dif;

PIbt.inte = PIbt.inte_ant + Ts/2 * (PIbt.erropi  + PIbt.erropi_ant);
PIbt.inte_ant = PIbt.inte;
PIbt.erropi_ant = PIbt.erropi;

PIbt.piout = (Kpbt*PIbt.erro + Kibt*PIbt.inte); 

PIbt.piout_sat = PIbt.piout;
if(PIbt.piout>sat_up) PIbt.piout_sat = sat_up;
if(PIbt.piout<sat_down) PIbt.piout_sat= sat_down;

PIbt.dif = PIbt.piout - PIbt.piout_sat;

PIbt.duty = PIbt.piout_sat*PRD;                    // Saída do controlador -> duty
} //fecha DCM

else
{
  IRamp_bt.final = 0;
}
////////////////////////////////////////////////////////////////Inicia Carga (INT1)///////////////////////////////////////////////////////////////
//Carga
if(flag.CM == 1)
{        

//////////////////////////////////////////////////////Malha externa de controle da tensão
///Controle
PIbuv.Xref = Vref;
PIbuv.Xm   = Vbat;                                           //Tensão medida para o modo buck (carga)           

PIbuv.erro = PIbuv.Xref  - PIbuv.Xm;

PIbuv.erropi = PIbuv.erro - (1/Kpvbu)*PIbuv.dif;

PIbuv.inte = PIbuv.inte_ant + Ts/2 * (PIbuv.erropi  + PIbuv.erropi_ant);
PIbuv.inte_ant = PIbuv.inte;
PIbuv.erropi_ant = PIbuv.erropi;

PIbuv.piout = (Kpvbu*PIbuv.erro + Kivbu*PIbuv.inte); 

PIbuv.piout_sat = PIbuv.piout;   //Coloquei o  Iref_ch/N_br como condição inicial para reduzir variações durante o chaveamento do controle

if(PIbuv.piout_sat < -90) PIbuv.piout_sat = -90;
if(PIbuv.piout_sat > Iref_ch/3) PIbuv.piout_sat  = Iref_ch/3;

PIbuv.dif = PIbuv.piout - PIbuv.piout_sat;
	
PIbu.Xref = PIbuv.piout_sat;

PIbu.Xm = -Ibat;                                               //Corrente medida para o modo boost (Descarga)
PIbu.erro = PIbu.Xref  - PIbu.Xm;

PIbu.inte = PIbu.inte_ant + Ts/2 * (PIbu.erro  + PIbu.erro_ant);
PIbu.inte_ant = PIbu.inte;
PIbu.erro_ant = PIbu.erro;

PIbu.piout = (Kpibu*PIbu.erro + Kiibu*PIbu.inte);            //Normalizado pelo Vdc

PIbu.piout_sat = PIbu.piout;
if(PIbu.piout>sat_up) PIbu.piout_sat = sat_up;
if(PIbu.piout<sat_down) PIbu.piout_sat= sat_down;

PIbu.duty = PIbu.piout_sat*PRD;                    // Saída do controlador -> duty

}// fecha CCM

}//Fecha interrupção1

///////////////////////////////////////////////////Interrupção 2
if(count_120 == CMPB)
{
//Rampa da referência de I
Ramp(&IRamp2_bt);
       
////////////////////////////////////////////////////////////////Inicia Descarga(INT1)///////////////////////////////////////////////////////////////
if(flag.DM == 1)
{
PIbt2.Xref = IRamp2_bt.atual;

PIbt2.Xm = Ibat2;                                               //Corrente medida para o modo boost (Descarga)
PIbt2.erro = PIbt2.Xref  - PIbt2.Xm;

PIbt2.erropi = PIbt2.erro - (1/Kpbt)*PIbt2.dif;

PIbt2.inte = PIbt2.inte_ant + Ts/2 * (PIbt2.erropi  + PIbt2.erropi_ant);
PIbt2.inte_ant = PIbt2.inte;
PIbt2.erropi_ant = PIbt2.erropi;

PIbt2.piout = (Kpbt*PIbt2.erro + Kibt*PIbt2.inte); 

PIbt2.piout_sat = PIbt2.piout;
if(PIbt2.piout>sat_up) PIbt2.piout_sat = sat_up;
if(PIbt2.piout<sat_down) PIbt2.piout_sat= sat_down;

PIbt2.dif = PIbt2.piout - PIbt2.piout_sat;

PIbt2.duty = PIbt2.piout_sat*PRD;                    // Saída do controlador -> duty
} //fecha DCM

else
{
  IRamp2_bt.final = 0;
}
////////////////////////////////////////////////////////////////Inicia Carga (INT2)///////////////////////////////////////////////////////////////
//Carga a corrente constante
if(flag.CM == 1)
{

PIbu2.Xref = PIbu.Xref;

PIbu2.Xm = -Ibat2;                                               //Corrente medida para o modo boost (Descarga)
PIbu2.erro = PIbu2.Xref  - PIbu2.Xm;

PIbu2.inte = PIbu2.inte_ant + Ts/2 * (PIbu2.erro  + PIbu2.erro_ant);
PIbu2.inte_ant = PIbu2.inte;
PIbu2.erro_ant = PIbu2.erro;

PIbu2.piout = (Kpibu*PIbu2.erro + Kiibu*PIbu2.inte);            //Normalizado pelo Vdc;

PIbu2.piout_sat = PIbu2.piout;
if(PIbu2.piout>sat_up) PIbu2.piout_sat = sat_up;
if(PIbu2.piout<sat_down) PIbu2.piout_sat= sat_down;

}// fecha CCM

PIbu2.duty = PIbu2.piout_sat*PRD;                    // Saída do controlador -> duty

}//Fecha interrupção2

///////////////////////////////////////////////////Interrupção 3
if(count_240 == CMPB)
{
//Rampa da referência de I
Ramp(&IRamp3_bt);

////////////////////////////////////////////////////////////////Inicia Descarga(INT1)///////////////////////////////////////////////////////////////
if(flag.DM == 1)
{
PIbt3.Xref = IRamp3_bt.atual;

PIbt3.Xm = Ibat3;                                               //Corrente medida para o modo boost (Descarga)
PIbt3.erro = PIbt3.Xref  - PIbt3.Xm;

PIbt3.erropi = PIbt3.erro - (1/Kpbt)*PIbt3.dif;

PIbt3.inte = PIbt3.inte_ant + Ts/2 * (PIbt3.erropi  + PIbt3.erropi_ant);
PIbt3.inte_ant = PIbt3.inte;
PIbt3.erropi_ant = PIbt3.erropi;

PIbt3.piout = (Kpbt*PIbt3.erro + Kibt*PIbt3.inte); 

PIbt3.piout_sat = PIbt3.piout;
if(PIbt3.piout>sat_up) PIbt3.piout_sat = sat_up;
if(PIbt3.piout<sat_down) PIbt3.piout_sat= sat_down;

PIbt3.dif = PIbt3.piout - PIbt3.piout_sat;

PIbt3.duty = PIbt3.piout_sat*PRD;                    // Saída do controlador -> duty
} //fecha DCM

else
{
  IRamp3_bt.final = 0;
}

////////////////////////////////////////////////////////////////Inicia Carga (INT3)///////////////////////////////////////////////////////////////
//Carga a corrente constante
if(flag.CM == 1)
{
PIbu3.Xref = PIbu.Xref;

PIbu3.Xm   = -Ibat3;                                               //Corrente medida para o modo boost (Descarga)
PIbu3.erro = PIbu3.Xref  - PIbu3.Xm;

PIbu3.inte = PIbu3.inte_ant + Ts/2 * (PIbu3.erro  + PIbu3.erro_ant);
PIbu3.inte_ant = PIbu3.inte;
PIbu3.erro_ant = PIbu3.erro;

PIbu3.piout = (Kpibu*PIbu3.erro + Kiibu*PIbu3.inte);            //Normalizado pelo Vdc 

PIbu3.piout_sat = PIbu3.piout;
if(PIbu3.piout>sat_up) PIbu3.piout_sat = sat_up;
if(PIbu3.piout<sat_down) PIbu3.piout_sat= sat_down;

}// fecha CCM

PIbu3.duty = PIbu3.piout_sat*PRD;                    // Saída do controlador -> duty


}//Fecha interrupção3

//..........PWM do conversor boost (Descarga)

if(flag.CM == 1)
{
	if(PIbu.duty >= count_0)  
	{ 
 		S2 = 0;
 		S1 = 1;
	} 
	else  
	{
 		S2 = 0;
		S1 = 0;
	}

	if(PIbu2.duty >= count_120)  
	{ 
 		S4 = 0;
 		S3 = 1;
	} 
	else  
	{
 		S4 = 0;
 		S3 = 0;
	}

	if(PIbu3.duty >= count_240)  
	{ 
 		S6 = 0;
 		S5 = 1;
	} 
	else  
	{
 		S6 = 0;
 		S5 = 0;
	}

}


if(flag.DM == 1)
{
//..........PWM do conversor boost (Descarga)
if(PIbt.duty >= count_0)  
{ 
 S2 = 1;
 S1 = 0;
} 
else  
{
 S2 = 0;
 S1 = 0;
}

if(PIbt2.duty >= count_120)  
{ 
 S4 = 1;
 S3 = 0;
} 
else  
{
 S4 = 0;
 S3 = 0;
}

if(PIbt3.duty >= count_240)  
{ 
 S6 = 1;
 S5 = 0;
} 
else  
{
 S6 = 0;
 S5 = 0;
}

}

}//fecha control enable

Output(0) = IRamp_bt.atual;
Output(1) = PIbt.Xm;
Output(2) = PIbt2.Xm;
Output(3) = PIbt3.Xm;
Output(4) = S1;
Output(5) = S2;
Output(6) = S3;
Output(7) = S4;
Output(8) = S5;
Output(9) = S6;
Output(10) = PIbuv.Xref;
Output(11) = PIbuv.Xm;
Output(12) = PIbu.Xref;
Output(13) = PIbu.Xm;
Output(14) = PIbt.piout_sat;
Output(15) = PIbt2.piout_sat;
Output(16) = PIbt3.piout_sat;


