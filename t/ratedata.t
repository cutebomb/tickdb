create('ratedata',4,'time','symbol','bid1','ask1','tsee','time.month','symbol');
timeformat('%Y-%M-%d %h:%m:%s');
appendcsv('ratedata','/mnt/hgfs/MyCode/ratedata/*.csv',3,2,4,5);
write('ratedata');
