#ifndef CLIENT
#define CLIENT
#include "interface.h"
#include <thread>
#include <set>
#include "queue_thread.cpp"

using namespace std;
class Client;

class Subscription {
	public:
	TopicName topic;
	Client * owner;
	Subscription(TopicName T="",Client * C=nullptr);
	};
class SubsPtrCompare{
	public:
	bool operator() (Subscription * const & lhs,Subscription * const & rhs)const;
	}; //Para comparar punteros de Subscriptions

class RetainedTopic{
	public:
	TopicName topic;
	TopicValue value;
	Client *owner;
	RetainedTopic(TopicName T="",TopicValue V="",Client * C=nullptr);
	};
class RetPtrCompare{
	public:
	bool operator() (RetainedTopic * const & lhs,RetainedTopic * const & rhs)const;
	};//Para comparar punteros de RetainedTopic

class Client:public BrokerOpsIF{
	private:
		
		size_t N; //Tamaño de la cola
		thread th;
		mutex m_cif; //este mutex es para proteger el acceso a cif
		ClientOpsIF *cif;
		set <Subscription *,SubsPtrCompare> subscriptions; //Aca van las suscripciones propias
		set <RetainedTopic *,RetPtrCompare> topics; //Aca van los retained topics propios
		//Es para que cada cliente se encargue de alocar y borrar los suyos.
		//A los container estos solo se añaden nuevos elementos y se borran.
		Queue <Message *> recvQueue; // OJO: Esta cola debe ser thread safe 
		void thread_handler();
	public:
		Client(ClientOpsIF * _cif=nullptr,size_t _N=4);
		~Client();
		void sendPublish2Client(const PublishMsg &m); //Esto usa el lock y se asegura de enviar un unico publish a la vez
		void sendMsg(const Message &m);
	};
	
#endif
