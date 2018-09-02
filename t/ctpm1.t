create('ctpm1',8,'time','symbol','open','high','low','close','vol','amount','tsffffee',0,'symbol');
load('ctpticks');
run('au1206');
display('off');
lastmin=-1;
lastday=-1;
{
if(time.date!=lastday) {
lastday=time.date;
tickvol:=vol;
tickamount:=amount;
} else {
tickvol:=vol-ref(vol,1);
tickamount:=amount - ref(amount,1);
}
t:=time;
sec=time.second;
if(time.hms<=90000) {trades:=0;}
if(lastmin != time.min) {
    append('ctpm1',time,symbol,prc,prc,prc,prc,tickvol,tickamount);
    lastmin=time.min;
    if(time.hour==8) { lastmin=0;}
} else {
  if(sec != ref(sec,1)) {
    if(prc>ctpm1.high) {
        ctpm1.high=prc;
    }
    if(prc<ctpm1.low) {
        ctpm1.low=prc;
    }
    ctpm1.close=prc;
    ctpm1.vol=ctpm1.vol+tickvol;
    ctpm1.amount=ctpm1.amount+tickamount;
  }
}
}
