load('qhm1');
run('IF1101');
ATR(N=14){
t:=time;
TR:=max(ref(close,1), high)-min(ref(close,1),low);
ATR:=sma(TR,N);
}
