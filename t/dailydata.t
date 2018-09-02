load('daily');
run('000001');
{
day:=ref(date,1);
if(close>ref(close,1)) {
label:=1;
} else {
label:=-1;
}
lb200:=ref(volume/ema(volume,200),1);
lb100:=ref(volume/ema(volume,100),1);
lb50:=ref(volume/ema(volume,50),1);
lb10:=ref(volume/ema(volume,10),1);
lb5:=ref(volume/ema(volume,5),1);
diff10:=ref(close/ama(close,10,2,30),1);
rma5:=ref(close/ema(close,5),1);
rma10:=ref(close/ema(close,10),1);
rma20:=ref(close/ema(close,20),1);
rma50:=ref(close/ema(close,50),1);
rma100:=ref(close/ema(close,100),1);
rma200:=ref(close/ema(close,200),1);
ma5rma20:=ref(ema(close,5)/ema(close,20),1);
ma10rma50:=ref(ema(close,10)/ema(close,50),1);
v1=volume/((high-low)*2-abs(close-open));
if(close>open){
  buyv=v1*(high-low);
  sellv=v1*((high-close)+(open-low));
} else {
  buyv=v1*((high-open)+(close-low));
  sellv=v1*(high-low);
}
green=sma(buyv,7);
red=sma(sellv,10);
bsr:=ref(green/red,1);
diff=ema(close,12)-ema(close,26);
dea=ema(diff,9);
macd:=ref(2*(diff-dea),1);
k=(close-llv(low,14))/(hhv(high,14)-llv(low,14));
nk:=ref(k,1);
d:=ref(sma(k, 3),1);
j:=ref(sma(d, 3),1);
}
