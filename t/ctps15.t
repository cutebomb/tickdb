create('ctps15',8,'time','symbol','open','high','low','close','vol','amount','tsffffee',0,'symbol');
load('ctpticks');
run('au1112');
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
}
