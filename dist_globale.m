function [ dist_totale ] = dist_globale( n_frames_ref,n_frames,coeffs_carac_ref,coeffs_carac )
%DIST_GLOBALE Summary of this function goes here
%   Detailed explanation goes here


distance_cumul =  realmax('double') * ones(n_frames_ref + 1,n_frames + 1);
distance_cumul(1:2,1:2) = zeros(2);
for i=1:n_frames_ref
    for j = 1:n_frames
        distance_cumul(i+1,j+1)= min(min(distance_cumul(i,j), distance_cumul(i+1,j)), distance_cumul(i,j+1))...
                                 + dist_local(coeffs_carac_ref(i,:),coeffs_carac(j,:));
    end
end
dist_totale = distance_cumul(n_frames_ref + 1,n_frames + 1);

end

