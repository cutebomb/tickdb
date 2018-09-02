load('ctpm1')
run('au1112');
sumdiff2(N=14){
t:=time;
midLine:=ama(vwap, 10, 2, 30);
sumdiff:=sum(close-midLine,N);
}
