load('daily');
run('000001');
d=0;
{
day:=ref(date,1);
if(close>ref(close,1)) {
label:=1;
} else {
label:=-1;
}
p:=ref(close,1);
diff=ema(close,12)-ema(close,26);
dea=ema(diff,9);
macd:=ref(2*(diff-dea),1);
k=(close-llv(low,14))/(hhv(high,14)-llv(low,14));
nk:=ref(k,1);
roc:=ref(close/ref(close,14),1);
ma:=ref(sma(close,20),1);
mv=ref(sma((close-ma)*(close-ma),14),1);
mvr1=ref(mv/ref(mv,10),1);
mvr:=mvr1*mvr1;
ma5=sma(close,5);
ma10=sma(close,10);
d5:=ref(close/ma5,1);
oscp:=ref((ma5-ma10)/ma5,1);
tp=(high+low+close)/3.0;
ma14=sma(tp,14);
md14=sum(ma14-tp,14)/14;
cci14:=ref((tp-ma14)/(0.015*md14),1);
lc=ref(close,1);
rs=sma(max(close-lc,0),6)/sma(abs(close-lc),6);
rsi:=ref(100-100/(1+rs),1);
d=ref(d,1)+1;
sd=sum(d,14);
lrl:=ref((14*sum(d*close,14)-sum(d,14)*sum(close,14))/(14*sum(d*d,14)-sd*sd),1);
ma10r200:=ref(ema(close,10)/ema(close,200),1);
}