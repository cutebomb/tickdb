create('ifm1',7,'time','open','high','low','close','vol','amount','tffffff',0,0);
load('ift');
module('sim','e',bid1,ask1,1000000);
run(0);
display('off');
lastmin=-1;
amarange4(K=0.35,length=4,nATR=2.8,Ntrade=5)
{
vol=cumVol-ref(cumVol,1);
amount=cumAmount - ref(cumAmount,1);
t:=time;
if(time.hms<91500) {trades:=0;lastmin=0;print(time);}
if(lastmin != time.min) {
    append('ifm1',time,prc,prc,prc,prc,vol,amount);
    lastmin=time.min;
} else {
    if(prc>ifm1.high) {
        ifm1.high=prc;
    }
    if(prc<ifm1.low) {
        ifm1.low=prc;
    }
    ifm1.close=prc;
    ifm1.vol=ifm1.vol+vol;
    ifm1.amount=ifm1.amount+amount;

}
m:=`sumdiff(length).midLine#ifm1`;
sumdiff:=`sumdiff(length).sumdiff#ifm1`;
ATR:=`ATR(14).ATR#ifm1`;
if(position==0 && time.hms>92000 && time.hms<143000 && sumdiff > nATR*ATR && prc < upperLimit*0.99 && prc*0.99>lowerLimit && trades<Ntrade) {
        buy(1);
	stopPrice:=lastEntryPrice*(1-0.01*K);
	trades:=trades+1;
}
if(position==0 && time.hms>92000 && time.hms<143000 && sumdiff < -nATR*ATR && prc > lowerLimit*1.01 && prc*1.01<upperLimit && trades<Ntrade) {
        short(1);
	stopPrice:=lastEntryPrice*(1+0.01*K);
	trades:=trades+1;
}
if(position>0) {
    if(prc >= upperLimit-0.0001) {
        sell(1,prc);
    }
    if(prc < stopPrice || time.hms>151400) {
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
    if(prc > stopPrice || time.hms>151400) {
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
