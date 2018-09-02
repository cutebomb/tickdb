create('auMI',7,'time','vol','prc','ask1','askvol1','bid1','bidvol1','teeeiei','time.day',0);
timeformat('%Y-%M-%d %h:%m:%s');
appendcsv('auMI','/localData/market/commdty/2011/MI/auMI*.csv',0,1,2,3,4,5,6);
write('auMI');
