f_ech = 16000;


%chargement du fichier
son= wavread('test3/adroite.wav');

%création des trames
Nsamps = length(son);
frames_length = 320 ;
Nframes = (floor(Nsamps / frames_length)) * 2 -1;
ordre = 150;
frames = zeros(Nframes,frames_length);
fen = hamming(frames_length);

for frame = 1: Nframes
    frames(frame,:) = fen .* son(frames_length*(frame - 1)/2 + 1: frames_length*(frame + 1)/2);
end


for frame = 1: Nframes           %###########################################################%
    %%levinson-durbin
    son_fen = frames(frame,:);
    acf = zeros(1,frames_length);
    
    for m = 1 : frames_length
        xdecale = zeros(1,frames_length);
        xdecale(1:m) = son_fen(frames_length - m + 1:frames_length);
        acf(m) = son_fen*(xdecale');
    end
    
    
    %acf = corr(son_fen); %% autocorr\'elation
    acf(1:ordre+1) = [];                %% on enl\`eve la partie n\'egative
    acf(1) = real(acf(1));           %% Levinson-Durbin requiert c(1)==conj(c(1))
    
    ref = zeros(ordre,1);
    tmp = -acf(2)/acf(1);
    lpc = [ tmp ];
    sigma2 = real( ( 1 - tmp*conj(tmp)) * acf(1) ); %% real : enl\`eve une \'eventuelle
    %%   partie imaginaire r\'esiduelle
    ref(1) = tmp;
    for tt = 2 : ordre
        tmp = -(acf(tt+1) + lpc * acf(tt:-1:2)') / sigma2; %e3
        lpc = [ lpc + tmp*conj(lpc(tt-1:-1:1)), tmp ]; %E4
        sigma2 = sigma2 * ( 1 - real(tmp*conj(tmp)) );
        ref(tt) = tmp;
    end;
    figure
    lpc = [1, lpc];
    plot(lpc);
end                           %###########################################################%
