#include "simclient.h"
#include <chrono>


SimClient::SimClient(Broker & B):broker{B}{
	};

void SimClient::start() {simth = thread(&SimClient::runSim, this);}; //Ojo con esto, si llamo al destructor de las clases hijo,
//va a llamar a la funcion virtual pura
SimClient::~SimClient(){
	connected=false;
	if (simth.joinable()) simth.join();
	};
	
SimPublisher::SimPublisher(Broker &B,TopicName T):SimClient{B},publish_topic{T}{
	};
	
void SimPublisher::runSim(){
	BrokerOpsIF *brops =broker.registerClient(this);
	brops->	sendMsg(ConnectMsg("admin","1234"));
	//ESPERAR POR CONNACK
	unique_lock<mutex> lock(M);
	while(!connack_recieved){ //Espera hasta recibir el connack
		cv.wait(lock);
		}
	if (incoming_connack->getStatus()!=ConnAckMsg::Status::CONNECTION_OK) { //Si el connack no dio OK
		delete incoming_connack; //libero la memoria del clone
		lock.unlock(); //Libero el mutex
		} 
	else{
		delete incoming_connack; //libero la memoria del clone
		lock.unlock(); //Libero el mutex
		connected=true;
		for (int i=0;i<20;i++){
			if (!connected) return;
			this_thread::sleep_for(chrono::milliseconds(100));
			PublishMsg m(publish_topic,to_string(i),true);
			brops->sendMsg(m);
			}
		}
	brops->sendMsg(DisconnectMsg());
	};
void SimPublisher::recvMsg(const Message & m){
	Message::Type type=m.getType();
	if (type==Message::Type::PUBLISH){
		PublishMsg * message=static_cast <PublishMsg *>(m.clone());
		//cout <<message->getTopic() <<message->getValue() <<endl;
		delete message;
		}
	if (type==Message::Type::CONNACK){
		unique_lock<mutex> lock(M);
		incoming_connack=static_cast <ConnAckMsg *>(m.clone());
		lock.unlock();
		connack_recieved=true;
		cv.notify_one();
		}
	};
	
SimSubscriber::SimSubscriber(Broker &B, TopicName T):SimClient{B},sub_topic{T}{
	};
void SimSubscriber::runSim(){
	BrokerOpsIF *brops =broker.registerClient(this);
	brops->	sendMsg(ConnectMsg("admin","1234"));
	//ESPERAR POR CONNACK
	unique_lock<mutex> lock(M);
	while(!connack_recieved){ //Espera hasta recibir el connack
		cv.wait(lock);
		}
	if (incoming_connack->getStatus()!=ConnAckMsg::Status::CONNECTION_OK) { //Si el connack no dio OK
		delete incoming_connack; //libero la memoria del clone
		lock.unlock(); //Libero el mutex
		} 
	else{
		delete incoming_connack; //libero la memoria del clone
		lock.unlock(); //Libero el mutex
		connected=true;
		SuscribeMsg m(sub_topic);
		brops->sendMsg(m);
		this_thread::sleep_for(chrono::milliseconds(500)); //recibe publish por este tiempo
		brops->sendMsg (UnsuscribeMsg(sub_topic));
		}
	brops->sendMsg(DisconnectMsg()); //Y me desconecto
	};

void SimSubscriber::recvMsg(const Message & m){
	Message::Type type=m.getType();
	if (type==Message::Type::PUBLISH){
		PublishMsg * message=static_cast <PublishMsg *>(m.clone());
		//cout <<message->getTopic() <<message->getValue() <<endl;
		delete message;
		}
	if (type==Message::Type::CONNACK){
		unique_lock<mutex> lock(M);
		incoming_connack=static_cast <ConnAckMsg *>(m.clone());
		lock.unlock();
		connack_recieved=true;
		cv.notify_one();
		}
	};
