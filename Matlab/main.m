%addpath('/usr/users/promo2015/villedieudet_ant/travail/tl_voix/');
clear
close all


%% Parametres
f_ech = 16000;
ordre_lpc = 10;
n_filtres = 40;
frames_length = 320 ;


%% Etude des sons de reference
son_droite = struct('name', 'test3/adroite.wav');
son_gauche = struct('name', 'test3/agauche.wav');
son_avant = struct('name', 'test3/enavant.wav');
son_stop = struct('name', 'test3/stop.wav');
ref_son = [son_droite,son_gauche,son_avant,son_stop];

for i = 1:4   
    son_ref = wavread(ref_son(i).name);
    son_ref = remove_blanc(son_ref);
    n_frames_ref = (floor(length(son_ref) / frames_length)) * 2 -1;
    coeffs_carac = get_carac(son_ref, n_frames_ref, frames_length, n_filtres, ordre_lpc, f_ech);
    ref_son(i).carac = coeffs_carac;
    ref_son(i).n_frames = n_frames_ref;
end


%% etude du son test
son= wavread('test3/enavant2.wav');
son = remove_blanc(son);
n_frames = (floor(length(son) / frames_length)) * 2 -1;
coeffs_carac = get_carac(son, n_frames, frames_length, n_filtres, ordre_lpc, f_ech);


%% distances
dist = zeros(1,4);
for i = 1:4
    dist(i) = dist_globale(ref_son(i).n_frames,n_frames,ref_son(i).carac,coeffs_carac);
end
[a,b] = min(dist);
ref_son(b).name


%%
% Ce qu'on a envie de faire : 
% 
% enregistrer une base de donn�es pas trop uniforme
% pour pouvoir faire de l'apprentissage
% 
% passer en C++
%
% un d�pot git


