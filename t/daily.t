create('daily',8,'date','symbol','open','close','high','low','volume','amount','tseeeeee',0,'symbol');
timeformat('%Y.%M.%d');
appendcsv('daily','sh000001.csv',0,1,2,3,4,5,6,7);
write('daily');
