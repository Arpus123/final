#ifndef SIMCLIENT
#define SIMCLIENT
#include "broker.h"

class SimClient: public ClientOpsIF{
	thread simth;
	virtual void runSim()=0;
	public:
		bool connack_recieved=false,finished=false,connected=false;
		mutex M;
		condition_variable cv;
		ConnAckMsg * incoming_connack;
		Broker &broker;
		SimClient(Broker &);
		~SimClient();
		void start();
		SimClient & operator =(const SimClient &)=delete;
		SimClient (const SimClient &)=delete;
	};
class SimPublisher: public SimClient{
	void runSim();
	TopicName publish_topic;
	public:	
		SimPublisher(Broker &,TopicName T="");
		void recvMsg(const Message &);
	};
class SimSubscriber: public SimClient{
	void runSim();
	TopicName sub_topic;
	public:
		SimSubscriber(Broker &,TopicName T="");
		void recvMsg(const Message &);
	};
#endif
