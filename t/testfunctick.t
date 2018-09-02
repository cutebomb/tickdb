create('qhm1',8,'time','symbol','open','high','low','close','vol','amount','tsffffff',0,'symbol');
load('ift');
nextstarttime=0m;
module('sim','e',bid1,ask1,1000000);
run('IF1101');

amarange4(K=0.5,length=8,nATR=1.6){
vol=cumVol-ref(cumVol,1);
amount=cumAmount - ref(cumAmount,1);
if(nextstarttime<time,
append('qhm1',time,symbol,prc,prc,prc,prc,vol,amount)
nextstarttime=time-time%1m+1m;
,
qhm1.time=time;
if(prc>qhm1.high,qhm1.high=prc);
if(prc<qhm1.low,qhm1.low=prc);
qhm1.close=prc;
qhm1.vol=qhm1.vol+vol;
qhm1.amount=qhm1.amount+amount;
);
t:=time;
sma:=`testfunc().sma#qhm1`;
ema:=`testfunc().ema#qhm1`;
ama:=`testfunc().ama#qhm1`;
}
