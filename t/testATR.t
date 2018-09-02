load('qhm1');
run('IF1101');
ATR(length=4,nATR=2.8){
TR=max(ref(close,1), high)-min(ref(close,1),low);
ATR=sma(TR,14);
t:=time;
a:=nATR*ATR;
midLine=ama(close, 10, 2, 30);
b:=close-midLine;
sumdiff:=sum(close-midLine, length);
}
