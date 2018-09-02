create('ctpticks',11,'time','symbol','vol','amount','upperLimit','lowerLimit','prc','ask1','askvol1','bid1','bidvol1','tseefffffff','time.day','symbol');
timeformat('%Y%M%d %h:%m:%s');
appendcsv('ctpticks','/localData/market/ctp/ticks.*.csv',0,1,2,3,4,5,6,7,8,9,10);
write('ctpticks');
