%%% Algorithme de Levinson-Durbin pour la d\'etermination des param\`etres AR
%%%
%%% entr\'ees :
%%%   - xx : signal
%%%   - pp : ordre du mod\`ele AR (choisi de mani\`ere ind\'ependante)
%%%   - fe : fr\'equence d'\'echantillonnage
%%%
%%% sorties :
%%%   - aa     : les param\`etres AR
%%%   - sigma2 : variance du bruit
%%%   - ref    : coefficients de r\'eflexion
%%%   - ff     : fr\'equences auxquelles la dsp a \'et\'e calcul\'ee
%%%   - mydsp  : la dsp elle-m\^eme
%%%
%%% exemple : fe=32000;f0=440;xx=cos(2*pi*f0/fe*[1:1280]+2*pi*rand(1,1));mylevinsondurbin(xx,4,fe);
%%%
%%% S. Rossignol -- 2012

function [aa, sigma2, ref, ff, mydsp] = mylevinsondurbin (xx, pp, fe)

acf = xcorr(xx, pp+1, 'biased'); %% autocorr\'elation
acf(1:pp+1) = [];                %% on enl\`eve la partie n\'egative
acf(1) = real(acf(1));           %% Levinson-Durbin requiert c(1)==conj(c(1))

ref = zeros(pp,1);
gg = -acf(2)/acf(1);
aa = [ gg ];
sigma2 = real( ( 1 - gg*conj(gg)) * acf(1) ); %% real : enl\`eve une \'eventuelle
                                              %%   partie imaginaire r\'esiduelle
ref(1) = gg;
for tt = 2 : pp
  gg = -(acf(tt+1) + aa * acf(tt:-1:2)') / sigma2; %e3
  aa = [ aa + gg*conj(aa(tt-1:-1:1)), gg ]; %E4
  sigma2 = sigma2 * ( 1 - real(gg*conj(gg)) );
  ref(tt) = gg;
end;
aa = [1, aa];


