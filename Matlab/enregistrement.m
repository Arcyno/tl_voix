
fs=16000; %frequence d'echantillonage (Hz)
nbit=16;   % nombre de bits par echantillon
mode=1;   %pour dire st�reo
D=0.85;        % dur�e d'enregistrement (s)
piste=audiorecorder(fs,nbit, mode);
recordblocking(piste, D);
x=getaudiodata(piste);


wavwrite(x,fs,nbit,'enregistrement.wav');