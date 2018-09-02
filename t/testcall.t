create('qhm1',8,'time','symbol','open','high','low','close','vol','amount','tsffffff',0,'symbol');
load('ift');
run('IF0013');

amarange4(){
vol=cumVol-ref(cumVol,1);
amount=cumAmount - ref(cumAmount,1);
if(nextstarttime<time) {
    append('qhm1',time,symbol,prc,prc,prc,prc,vol,amount);
    nextstarttime=time-time%1m+59s;
} else {
    qhm1.time=time;
    if(prc>qhm1.high) {
        qhm1.high=prc;
    }
    if(prc<qhm1.low) {
        qhm1.low=prc;
    }
    qhm1.close=prc;
    qhm1.vol=qhm1.vol+vol;
    qhm1.amount=qhm1.amount+amount;
}
t:=time;
sumdiff:=`sumdiff(4).sumdiff#qhm1`;
ATR:=`ATR(14).ATR#qhm1`;
}
