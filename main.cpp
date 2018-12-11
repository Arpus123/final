#include "client.h"
#include "interface.h"
int main (){
	multiset <Subscription *,SubsPtrCompare> subs;
	Subscription A("A"),B("B"),C("C"),B2("B"),D("D");
	subs.insert(&B);
	subs.insert(&C);
	subs.insert(&A);
	subs.insert(&B2);
	auto ini=subs.lower_bound(&B);
	auto fin=subs.upper_bound(&B);
	//subs.erase(ini);
	subs.erase(&D);
	for (auto it=subs.begin();it!=subs.end();it++)
		cout<<(*it)<<endl;
		
	return 0;
	}
