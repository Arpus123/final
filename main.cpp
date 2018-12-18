#include "simclient.h"
int main (){
	Broker * B=Broker::getInstance();
	/*B->registerClient(nullptr);
	auto p=B->clients.begin();
	(*p)->sendMsg(ConnectMsg());
	(*p)->sendMsg(DisconnectMsg());
	*/
	SimPublisher S(*B);
	S.start();
	SimPublisher S3(*B);
	S3.start();
	/*SimClient *S2=&S ; 
	thread th=thread(&SimClient::runSim,S2);
	th.join();
	*/
	SimSubscriber S2(*B);

	S2.start();
	SimPublisher S4(*B,"Temp");
	S4.start();
	SimSubscriber S5(*B,"Temp");

	S5.start();
	this_thread::sleep_for(chrono::seconds(3)); //espero un tiempo prudencial para que todo corra,
	//otra alternativa es usar una condition variable y las variables finished de cada SimClient
	return 0;
	} 
