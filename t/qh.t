timeformat('%Y%M%d %h:%m:%s');
create('qh',8,'time','symbol','prc','cumVol','bid1','bidvol1','ask1','askvol1','tsfefifi','time.day','symbol');
appendcsv('qh', '/localWork/ctp/prod/ticks.20110819',0,1,6,2,9,10,7,8);
write('qh');
