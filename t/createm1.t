create('m1',8,'time','symbol','open','high','low','close','vol','amount','tsffffff',0,'symbol');
load('ticks');
nextstarttime:=time-time%1m;
createm1{
if(nextstarttime<time,
append('m1',time,sym,prc,prc,prc,prc,vol,vol*prc)
nextstarttime:=time-time%1m+1m;
,
m1.time=time;
if(prc>m1.high,m1.high=prc);
if(prc<m1.low,m1.low=prc);
m1.close=prc;
m1.vol=m1.vol+vol;
m1.amount=m1.amount+prc*vol;
)
}
