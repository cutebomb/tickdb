load('tbdata');
module('sim','e',close,close,1000000);
run(0);
display('off');
tbtest(K=0.3,length=2,nATR=2,Ntrade=5){
t:=time;
if(time.hms<=90000) {
	trades:=0;
};
TR=max(ref(close,1), high)-min(ref(close,1),low);
ATR=sma(TR,14);
midLine=ama(close,10,2,30);
sumdiff:=sum(close-midLine,length);
a:=nATR*ATR;
if(position==0 && time.hms>91000 && time.hms<143000 && sumdiff>a && trades<Ntrade) {
	buy(1);
	stopPrice:=lastEntryPrice*(1-0.01*K);
	trades:=trades+1;
}
if(position==0 && time.hms>91000 && time.hms<143000 && sumdiff<-a && trades<Ntrade) {
	short(1);
	stopPrice:=lastEntryPrice*(1+0.01*K);
	trades:=trades+1;
}
if(position>0) {
	if(low<stopPrice) {
		exitPrice=min(open,stopPrice);
		sell(1,exitPrice);
		if(time.hms<143000 && sumdiff<-a && trades<Ntrade) {
			short(1);
			stopPrice:=lastEntryPrice*(1+0.01*K);
			trades:=trades+1;
		}
	}
	if(time.hms>145700) {
	    sell(1);
	}
	newStop=high*(1-0.01*K);
	if(newStop > lastEntryPrice && newStop > stopPrice) {
		stopPrice:=newStop;
	}
}
if(position<0) {
	if(high>stopPrice) {
		exitPrice=max(open,stopPrice);
		cover(1,exitPrice);
		if(time.hms<143000 && sumdiff>a && trades<Ntrade) {
			buy(1);
			stopPrice:=lastEntryPrice*(1-0.01*K);
			trades:=trades+1;
		}
	}
	if(time.hms>145700) {
	    cover(1);
	}
	newStop=low*(1+0.01*K);
	if(newStop<lastEntryPrice && newStop<stopPrice) {
		stopPrice:=newStop;
	}
}
}
perf();
