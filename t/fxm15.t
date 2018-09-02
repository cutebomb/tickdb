create('fxm15',5,'time','open','high','low','close','teeee',0,0);
load('fxm1');
run(0);
display('off');
lastmin=100;
{
tt:=time.min%15;
if(lastmin > tt) {
  append('fxm15',time,open,high,low,close);
} else {
  if(high > fxm15.high) {
    fxm15.high = high;
  }
  if(low < fxm15.low) {
    fxm15.low = low;
  }
  fxm15.close = close;
}
lastmin = tt;
}
