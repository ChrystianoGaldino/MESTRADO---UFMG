
%%%%Referências%%%%
% https://www.mathworks.com/matlabcentral/answers/108616-plotting-data-from-a-csv-file
%https://lrodrigo.sgs.lncc.br/wp/programacao/matlab-exemplos-de-codigos-para-geracao-de-graficos/

%__________________________________________________________________________%

%%%%%%%%%Carga %%%%%%%%%

Array=csvread('arquivo_de_dados.csv'); %leitura do arquivo
col1 = Array(:, 1);  %armazena a coluna 1 - tempo(s)
col2 = Array(:, 2);  %armazena a coluna 2 - módulo de VF
col5 = Array(:, 5);  %armazena a coluna 5 - VF
col6 = Array(:, 6);  %armazena a coluna 6 - corrente
col7 = Array(:, 7);  %armazena a coluna 7 - FP
col8 = Array(:, 8);  %armazena a coluna 8 - Potência Ativa
col9 = Array(:, 9);  %armazena a coluna 9 - Potência Aparente



%gráfico do módulo de VF
figure (1);
plot(col1, col2,'r')
title ('Descarga - Módulo de VF');
xlabel ('Tempo (s)');
ylabel ('Módulo de VF');
%ylim([0 0.05])
xlim([0 350])
ax = gca;
ax.FontSize = 16;
legend('Tensão VF')
grid on;

%gráfico de corrente
figure (2);
plot(col1, col6,'b')
title ('Descarga - Corrente');
xlabel ('Tempo (s)');
ylabel ('Corrente');
%ylim([0 0.05])
xlim([0 350])
ax = gca;
ax.FontSize = 16;
legend('Corrente')
grid on;

%gráfico de FP
figure (3);
plot(col1, -col7,'k')
title ('Descarga - FP');
xlabel ('Tempo (s)');
ylabel ('FP');
%ylim([0 0.05])
xlim([0 350])
ax = gca;
ax.FontSize = 16;
legend('FP')
grid on;

%gráfico de Potência Ativa
figure (4);
plot(col1, -col8,'g:','LineWidth',2)
title ('Descarga - Potência Ativa');
xlabel ('Tempo (s)');
ylabel ('P');
%ylim([0 0.05])
xlim([0 350])
ax = gca;
ax.FontSize = 16;
legend('Potência Ativa')
grid on;

%gráfico de Potência Aparente
figure (5);
plot(col1, col9 * sqrt(3),'m')
title ('Descarga - Potência Aparente');
xlabel ('Tempo (s)');
ylabel ('S');
%ylim([0 0.05])
xlim([0 350])
ax = gca;
ax.FontSize = 16;
legend('Potência Aparente')
grid on;










% %%%%%%%  subplot VF %%%%%%
% figure (13);
% 
% subplot(2,1,1);
%  plot(col1, col2,'r',col1,col22,'k')
%  title ('Gráfico de Carga - CB e 10x CB','FontSize', 28);
%  xlabel ('tempo (s)','FontSize', 28);
%  ylabel ('Módulo de VF','FontSize', 28);
%  legend('Tensão VF - CB', 'Tensão VF - 10x CB','FontSize', 20)
% % ylim([130 180])
% ylim([150 180])
% 
% ax = gca;
% ax.FontSize = 16;
% 
%  grid on;
% 
% subplot(2,1,2); 
%  plot(col31, col32,'r',col41,col42,'k')
%  title ('Gráfico de Descarga - CB e 10x CB','FontSize', 28);
%  xlabel ('tempo (s)','FontSize', 28);
%  ylabel ('Módulo de VF','FontSize', 28);
%  legend('Tensão VF - CB', 'Tensão VF - 10x CB','FontSize', 20)
%  % ylim([130 180])
%  ylim([150 180])
% 
% ax = gca;
% ax.FontSize = 16;
% 
%  grid on;
% 
% 
% %%%%%%% subplot FP %%%%%%
% figure (14);
% 
% subplot(2,1,1);
% %plot(col1, col7,'r', col21, col27,'k')
% plot(col1, -1*col7,'r', col21, -1*col27,'k')
% title ('Gráfico de Carga - FP Unitário','FontSize', 28);
% xlabel ('tempo (s)','FontSize', 28);
% ylabel ('Fator de Potência','FontSize', 28);
% legend('Carga - CB', 'Carga - 10x CB','FontSize', 20)
% ylim([0 1])
% 
% ax = gca;
% ax.FontSize = 16;
% 
% grid on;
% 
% 
% subplot(2,1,2); 
% plot( col31, -1*col37,'r', col41, -1*col47,'k')
% %plot( col31, 1*col37,'r', col41, 1*col47,'k')
% title ('Gráfico de Descarga - FP Unitário','FontSize', 28);
% xlabel ('tempo (s)','FontSize', 28);
% ylabel ('Fator de Potência','FontSize', 28);
% legend('Descarga - CB', 'Descarga - 10x CB','FontSize', 20)
% %ylim([-1  -0.8])
% ylim([0 1])
% 
% ax = gca;
% ax.FontSize = 16;
% 
% grid on;


