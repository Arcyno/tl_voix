function sons_dct_frames = mymfcc(f_ech,son,frames_length,n_filtres);

%f_ech = 16000;
%chargement du fichier
%son= wavread('test3/adroite.wav');
%frames_length = 320 ;
%n_filtres = 40;

%création des trames
Nsamps = length(son);
Nframes = (floor(Nsamps / frames_length)) * 2 -1;
frames = zeros(Nframes,frames_length);

fen = hamming(frames_length);

for frame = 1: Nframes
    frames(frame,:) = fen .* son(frames_length*(frame - 1)/2 + 1: frames_length*(frame + 1)/2);
end



sons_dct_frames = zeros(Nframes,n_filtres);
%figure

for frame = 1: Nframes           %###########################################################%
    son_fen = frames(frame,:);
    
    
    %transfo de fourier
    son_fft = abs(fft(son_fen));
    son_fft = son_fft(1:frames_length/2);
    abscisse = f_ech*(0:frames_length/2-1)/(frames_length);
    subplot(2,2,1)
    plot(abscisse,son_fft)
    xlabel('Frequency')
    ylabel('Amplitude')
    title('fft')
    
    
    % transformation vers le domaine MEL
    abscisse_mel = 2595*(log(abscisse/700 + 1))/log(10);
    subplot(2,2,2)
    plot(abscisse_mel,son_fft)
    title('filtrage MEL')
    
    
    % banc de filtres triangulaires
    Fmin = abscisse_mel(1);
    Fmax = abscisse_mel(size(abscisse_mel,2));
    %W est ici la moitié de celui qui est indiqué dans l'énoncé
    W = (Fmax - Fmin)/(n_filtres + 1);
    %création d'un tableau contenant les fmin, fcentre et fmax de chaque filtre du banc
    bornes_filtres = Fmin*ones(3,n_filtres);
    for i = 1:n_filtres
        bornes_filtres(1,i) = 700*(power(10,(bornes_filtres(1,i) + (i-1)*W)/2595) - 1);
        bornes_filtres(2,i) = 700*(power(10,(bornes_filtres(2,i) + (i)*W)/2595) - 1);
        bornes_filtres(3,i) = 700*(power(10,(bornes_filtres(3,i) + (i+1)*W)/2595) - 1);
    end
    %Application des filtres
    spectres = zeros(n_filtres,size(son_fft,2));
    sum_s = zeros(n_filtres);
    coeff = size(son_fft,2)/abscisse(size(son_fft,2));
    for i =1:n_filtres
        for j = floor(bornes_filtres(1,i)*coeff) + 1 ...
                :floor(bornes_filtres(3,i)*coeff)
            spectres(i,j) = son_fft(j) * (j/coeff - bornes_filtres(1,i))/(bornes_filtres(2,i) - bornes_filtres(1,i));
            sum_s(i) = sum_s(i) + spectres(i,j);
        end
    end
    
    
    % log |.|
    son_log = log(abs(sum_s))/log(10);
    subplot(2,2,3)
    plot(son_log)
    title('log|.|')
    
    
    % DCT
    
    N = n_filtres;
    w = 2*ones(1,N);
    w(1) = 1;
    w = w/N;
    w = sqrt(w);
    
    son_dct = zeros(1,N);
    for j= 1:N
        for i = 1:N
            son_dct(j) = son_dct(j) + w(i)*cos((2*j - 3)*(i-1)*pi/(2*N))*son_log(i);
        end
    end
    subplot(2,2,4)
    plot(son_dct)
    title('dct')
    
    sons_dct_frames(frame,:) = son_dct;
    
    
end                      %###########################################################%


end