load('ctpm1')
run('au1112');
sumdiff(N=14){
t:=time;
midLine:=ama(close, 10, 2, 30);
sumdiff:=sum(close-midLine,N);
}
