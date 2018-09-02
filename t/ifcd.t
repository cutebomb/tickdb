create('ifm1',7,'time','open','high','low','close','vol','amount','tffffff',0,0);
load('ift');
module('sim','e',bid1,ask1,1000000);
run(0);
run_range('2011-01-04 00:00:00','2011-01-05 00:00:00');
display('on');
{
vol=cumVol-ref(cumVol,1);
amount=cumAmount - ref(cumAmount,1);
t:=time;
if(nextstarttime<time) {
    append('ifm1',time,prc,prc,prc,prc,vol,amount);
    nextstarttime=time-time%1m+59s;
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
preH:=`pre().preHigh#ifm1`;
preL:=`pre().preLow#ifm1`;

}
