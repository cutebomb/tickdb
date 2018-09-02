create('ift',26, 'time', 'prc', 'cumAmount', 'cumVol', 'bid1', 'bidvol1',
'bid2', 'bidvol2', 'bid3', 'bidvol3', 'bid4', 'bidvol4', 'bid5', 'bidvol5',
'ask1', 'askvol1', 'ask2', 'askvol2', 'ask3', 'askvol3', 'ask4', 'askvol4',
'ask5', 'askvol5', 'upperLimit','lowerLimit','tfeeffffffffffffffffffffff','time.day', 0);
appendcsv('ift', '2011/*.tik',0,7,12,13,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,10,11);
write('ift');
