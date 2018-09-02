load('m1');
run('000001');
testfunc{
t:=time;
c:=close;
sma:=sma(close, 20);
ema:=ema(close, 20);
ama:=ama(close, 10, 2, 30);
}
