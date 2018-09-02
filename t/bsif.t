load('ift');
module('sim','e',bid1,ask1, 1000000);
run('IF1101');
bsiftest{
t=time;
p=prc;
ap=ama(prc,10,2,30);
if(position==0 && p>ap,buy(1));
if(position==0 && p<ap, short(1));
if(position>0 && lastEntryPrice + 10<p,sell(1));
if(position<0 && lastEntryPrice - 10>p,cover(1));
}
perf();
