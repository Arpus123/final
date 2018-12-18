#ifndef BROKER
#define BROKER
#include "client.h"
#include <algorithm>
class sameSubsClient{
	private:
		Subscription * S;
	public:
		sameSubsClient(Subscription * _S):S{_S}{};
		bool operator () (Subscription *S2){return S2->owner == S->owner;};
	}; // Functor para comparar si los subscriptions tienen el mismo owner

class sameRetClient{
	private:
		RetainedTopic * S;
	public:
		sameRetClient(RetainedTopic * _S):S{_S}{};
		bool operator () (RetainedTopic *S2){return S2->owner == S->owner;};
	};// Functor para comparar si los RetainedTopic tienen el mismo owner

class Broker{
	mutex cli_mutex;
	mutex sub_mutex;
	string user="admin",pass="1234";
	multiset<Subscription *,SubsPtrCompare> subs_cache; //puede haber varias subs "iguales" (mismo topico)
	set<RetainedTopic *,RetPtrCompare> topics_cache;  //pero solo un RetTopic con el mismo topico
	set<Client *> clients;
	Broker(); 
	~Broker();  
	thread th; //Este thread se queda esperando para borrar los clients 
	public:
		Queue <Client *> delQueue; //Cola donde se registran los clientes para borrarse (debe ser thread safe)
		void thread_handler();
		static Broker *getInstance();
		BrokerOpsIF *registerClient (ClientOpsIF *);
		void publish(const PublishMsg &m); //esto llama a cada sendPublish2Client de cada Client.
		void addSubscription (Subscription *); //Anade un Subscription * al cache, usando subs_cache
		void delSubscription (Subscription *);
		void addRetained (RetainedTopic *);
		void delRetained (RetainedTopic *);
		void disconnect_all_clients();
		bool compareUserPass(string u,string p);
		Broker & operator =(const Broker &)=delete;
		Broker (const Broker &)=delete;
	};

#endif
