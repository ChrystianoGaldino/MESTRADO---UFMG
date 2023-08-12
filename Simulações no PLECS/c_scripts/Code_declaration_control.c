#include <math.h>

//float P = P_ativa;
//float Q = Q_reativa;

//float S;
//float theta;

//Variáveis recebem o sinal externo

P = P_ativa;
Q = Q_reativa;
Va = Vpcc_a;
Vb = Vpcc_b;
Vc = Vpcc_c;
Ia = Ipcc_a;
Ib = Ipcc_b;
Ic = Ipcc_c;
rst = Reset;
I_b1 = I_banco1;
I_b2 = I_banco2;
Vdc = V_dc;
Pref = P_ref;


//cálculos realizados

S = sqrt(pow(P,2) + pow(Q,2)); // potencia aparente
theta = acos(P/S);  //angulo theta
fp = cos(theta); // fator de potencia 
//P_calc = Va*Ia + Vb*Ib + Vc*Ic; //potência ativa calculada
//Q_calc = ((Va - Vb) * Ia + (Vb - Vc) * Ib + (Vc - Va) * Ic)*(1/sqrt(3)); //potência reativa calculada

if(rst < 1) //banco em cycle
{

iref_car = 59.71/3;   // corrente de referencia para a cycle (dividida para cada um dos 3 braços do interleaved)
iref_des = 0; 

}

if (rst >= 1) //banco em carga ou descarga
{

if(rst >1 ) //carga
{
    //iref_car = ((Pref/Vdc)/3);   // corrente de referencia para a carga (dividida para cada um dos 3 braços do interleaved) 
    //  * o modo que está implementado em blocos considera a corrente de carga dividida por 3, mas não funciona corretamente
    // dividindo a corrente apenas para os dois bancos funciona melhor para as potencias escolhidas para carregar os bancos.
    iref_car = ((Pref/Vdc)/2);   // corrente de referencia para a carga (dividida para cada um dos 2 bancos)  
    iref_des = 0;  
}

else //descarga
{
    iref_des = (Pref/Vdc)/2;   // corrente de referencia para a descarga (dividida por 2 pois são 2 bancos em paralelo)
    iref_car = 0;

}

}




//Variáveis de saída

Output(0) = S;
Output(1) = fp;
Output(2) = P;
Output(3) = Q;
Output(4) = I_b1;
Output(5) = I_b2;
Output(6) = iref_car;
Output(7) = iref_des;