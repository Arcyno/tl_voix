function [coeffs_carac] = get_carac(son, n_frames, frames_length, n_filtres, ordre_lpc, f_ech)
%GET_CARAC Summary of this function goes here
%   Detailed explanation goes here


%%
% mfcc
coeffs_mfcc = mymfcc(f_ech,son,frames_length,n_filtres);

%lpc
coeffs_lpc = mylpc(f_ech,son,frames_length,ordre_lpc);


%% normalisation
for i = 1:n_frames
    coeffs_mfcc(i,:) = (coeffs_mfcc(i,:) - mean(coeffs_mfcc(i,:)))/(var(coeffs_mfcc(i,:))*n_frames);
    coeffs_lpc(i,:) = (coeffs_lpc(i,:) - mean(coeffs_lpc(i,:)))/(var(coeffs_lpc(i,:))*n_frames);
end

%% concatenation
%Normalisation à faire
coeffs_carac = zeros(n_frames,ordre_lpc + n_filtres);
coeffs_carac(:,1:ordre_lpc + 1) = coeffs_lpc;
coeffs_carac(:,ordre_lpc + 2:ordre_lpc + 1 + n_filtres) = coeffs_mfcc;


end

