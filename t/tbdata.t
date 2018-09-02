create('tbdata',6,'time','open','high','low','close','vol','tffffi',0,0);
appendcsv('tbdata','/home/weirj/*.csv',0,1,2,3,4,5);
write('tbdata');
