load('daily');
run('000001');
testfunc{
t:=date;
c:=close;
sma:=sma(close, 20);
ema:=ema(close, 20);
ama:=ama(close, 10, 2, 30);
hhv1:=hhv(close,14);
llv1:=llv(close,14);

}
