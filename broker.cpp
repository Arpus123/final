#include "broker.h"


Broker::Broker(){th=thread(&Broker::thread_handler,this);};
//Broker * Broker::p_Broker=nullptr;
Broker * Broker::getInstance(){static Broker B; return &B;};


void Broker::addSubscription (Subscription * S){
	unique_lock <mutex> sub_lck(sub_mutex);
	
	subs_cache.insert(S); //Lo inserta
	RetainedTopic T(S->topic,"",nullptr); //Creo un RetainedTopic con el mismo topico
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
	//Notar que cada client sigue manteniendo su propio RetainedTopic si hay mas de uo que publica con el mismo topic
	topics_cache.insert(T);//Y ahora inserta el nuevo RetainedTopic
	};

void Broker::delRetained(RetainedTopic *T){
	unique_lock <mutex> sub_lck(sub_mutex);
	auto it=topics_cache.find(T); // Busca uno con el mismo topico, es un set asi que hay uno solo con el mismo topic
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

void Broker::thread_handler(){
	Client * cli2del;
	while (1){
		delQueue.get(&cli2del); //Saco el contenido de la lista de clientes a borrar
		if (cli2del==nullptr) {
			break; // Si es nullptr salgo
			}
		else{
			clients.erase(cli2del); //Lo borro de la lista de clientes
			delete cli2del; //Y lo destruyo este es delete de registerclient
			}
	}
	};

BrokerOpsIF * Broker::registerClient (ClientOpsIF * cif){
	Client * C=new Client(cif,4);
	unique_lock <mutex> cli_lck(cli_mutex);
	clients.insert(C);
	return C;
	};

void Broker::disconnect_all_clients(){
	for_each(clients.begin(),clients.end(),[](Client * C){C->sendMsg(DisconnectMsg());});	
	for_each(clients.begin(),clients.end(),[](Client * C){delete C;});	
	//En este caso les manda a todos los client un mensaje de disconnect, espera que lo procesen y luego los borra
	}

Broker::~Broker(){
	for_each(clients.begin(),clients.end(),[](Client * C){delete C;}); //joinea los thread
	delQueue.put(nullptr);
	if (th.joinable()) th.join();
	}
bool Broker::compareUserPass(string u,string p){
	return (u==user && p==pass);
	};
