load('qh');
module('sim','e',bid1,ask1,1000000);
run('CF205');
bstest{
t:=time;
if(position == 0 && prc>ama(prc,10,2,30),buy(1));
if(position > 0 && lastEntryPrice+10<prc,sell(1));
}
perf();
