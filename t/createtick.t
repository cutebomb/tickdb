create('ticks',4,'time','sym','prc','vol','tsff','time.day', 'sym');
appendcsv('ticks', '/localData/market/equity/*.tik',0,1,3,4);
write('ticks');
