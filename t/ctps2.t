create('ctps15',8,'time','symbol','open','high','low','close','vol','amount','tsffffee',0,'symbol');
load('ctpticks');
module('sim','e',bid1,ask1,1000000);
run('cu1112');
run_range('2011-09-01 00:00:00','2011-10-24 00:00:00');
display('off');
lastmin=-1;
lastday=-1;
ctps2(K=0.3,length=4,nATR=1.5,Ntrade=4){
if(time.date!=lastday) {
  lastday=time.date;
  tickvol:=vol;
  tickamount:=amount;
} else {
  tickvol:=vol-ref(vol,1);
  tickamount:=amount-ref(amount,1);
}
t:=time;
if(time.hms<90000) {
time=time+1m;
trades:=0;
lastmin:=-1;
}
if(time.hms>150000) {
time=ref(time,1);
}
if(lastmin != time.second/15) {
  append('ctps15',time,symbol,prc,prc,prc,prc,tickvol,tickamount);
  lastmin=time.second/15;
} else {
  if(prc>ctps15.high) {
    ctps15.high=prc;
  }
  if(prc<ctps15.low) {
    ctps15.low=prc;
  }
  ctps15.close=prc;
  ctps15.vol=ctps15.vol+tickvol;
  ctps15.amount=ctps15.amount+tickamount;
}
m:=`sumdiff(length).midLine#ctps15`;
sumdiff:=`sumdiff(length).sumdiff#ctps15`;
ATR:=`ATR(30).ATR#ctps15`;
if(position==0 && time.hms>90500 && time.hms<144000 && sumdiff > nATR*ATR &&
prc < upperLimit*0.995 && prc*0.995>lowerLimit
    && trades<Ntrade) {
        buy(1);
	stopPrice:=lastEntryPrice*(1-0.01*K);
	trades:=trades+1;
}
if(position==0 && time.hms>90500 && time.hms<144000 && sumdiff < -nATR*ATR && prc > lowerLimit*1.005 && prc*1.005<upperLimit
    && trades<Ntrade) {
        short(1);
	stopPrice:=lastEntryPrice*(1+0.01*K);
	trades:=trades+1;
}
if(position>0) {
    if(prc >= upperLimit-0.0001) {
        sell(1,prc);
    }
    if(prc < stopPrice || time.hms>145730) {
        sell(1);
    } else {
        newStop=prc*(1-0.01*K);
        if(newStop > lastEntryPrice && newStop > stopPrice) {
            stopPrice:=newStop;
        }
    }
}
if(position<0) {
    if(prc<=lowerLimit+0.0001) {
        cover(1,prc);
    }
    if(prc > stopPrice || time.hms>145730) {
        cover(1);
    } else {
        newStop = prc*(1+0.01*K);
        if(newStop < lastEntryPrice && newStop < stopPrice) {
            stopPrice:=newStop;
        }
    }
}
}
perf();
