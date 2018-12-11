#ifndef BROKER
#define BROKER

#include "client.h"
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
	static Broker* p_Broker;
	mutex cli_mutex;
	mutex sub_mutex;
	set<Client *> clients;
	set<Subscription *> subs_cache; //puede haber varias subs "iguales" (mismo topico)
	set<RetainedTopic *> topics_cache;  //pero solo un RetTopic con el mismo topico
	Broker(){}; //Fijarse que onda con los copy move y delete
	void addClient (Client *);
	void delClient (Client *);
	public:
		~Broker();  //REGLA DE 3
		static Broker *getInstance();
		BrokerOpsIF *registerClient (ClientOpsIF *);
		void publish(const PublishMsg &m); //esto llama a cada sendPublish2Client de cada Client.
		void addSubscription (Subscription *); //Anade un Subscription * al cache, usando subs_cache
		void delSubscription (Subscription *);
		void addRetained (RetainedTopic *);
		void delRetained (RetainedTopic *);
	};

#endif
