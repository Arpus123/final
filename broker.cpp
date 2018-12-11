#include "broker.h"
#include <algorithm>
#include<mutex>

Broker * Broker::p_Broker=nullptr;

Broker::~Broker(){delete p_Broker;};


Broker * Broker::getInstance(){if (!p_Broker) p_Broker=new Broker; return p_Broker;};
void Broker::addSubscription (Subscription * S){
	unique_lock <mutex> sub_lck(sub_mutex);
	subs_cache.insert(S); //Lo inserta
	RetainedTopic T(S->topic,nullptr); //Creo un RetainedTopic con el mismo topico
	auto it=topics_cache.find(&T); //Busco uno igual (ie: con el mismo topico)
	if (it!=topics_cache.end()) { //Si lo encontro
		PublishMsg P ((*it)->topic,(*it)->value);//Creo un nuevo Publish
		S->owner->sendPublish2Client(P); //y se lo mando al owner
		}
	};

void Broker::delSubscription (Subscription * S){
	unique_lock <mutex> sub_lck(sub_mutex);
	auto end=subs_cache.upper_bound(S);
	auto del=find_if(subs_cache.lower_bound(S),end,sameSubsClient(S)); //del es un iterador al elemento con el mismo topic y client
	if (del!=end) subs_cache.erase(del); //Si lo encontró, borra solamente ese elemento
	};

void Broker::addRetained(RetainedTopic *T){
	unique_lock <mutex> sub_lck(sub_mutex);
	topics_cache.erase(T); //borra el que tiene el mismo topic (si hay tal)
	topics_cache.insert(T);//Y ahora inserta el nuevo RetainedTopic
	};

void Broker::delRetained(RetainedTopic *T){
	unique_lock <mutex> sub_lck(sub_mutex);
	auto it=topics_cache.find(T); // Busca uno con el mismo topico
	if(sameRetClient(T)(*it)) topics_cache.erase(it); //Si tiene el mismo owner, lo borra
	//sino es de otro y no hace nada
	};

void Broker::publish(const PublishMsg &m){
	Subscription S(m.getTopic(),nullptr); //Creo una suscripcion con el mismo topico
	auto begin=subs_cache.lower_bound(&S); //Lo busco en el cache de subs
	auto end=subs_cache.upper_bound(&S);
	for_each(begin,end,[&m](Subscription * el){el->owner->sendPublish2Client(m);});
	//Y para cada uno llamo al sendPub2Cli que se lo manda a cada cliente, usando mutex (thread-safe)
	};

BrokerOpsIF * Broker::registerClient (ClientOpsIF * cif){
	Client * C=new Client(cif,4);
	unique_lock <mutex> cli_lck(cli_mutex);
	clients.insert(C);
	return C;
	};
