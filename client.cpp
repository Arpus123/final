#include "broker.h"
#include <algorithm>

Subscription::Subscription(TopicName T,Client * C):topic{T},owner{C}{};
bool SubsPtrCompare::operator() (Subscription * const & lhs,Subscription * const & rhs)const{
	return (lhs->topic)<(rhs->topic);}; //compara los topicos


RetainedTopic::RetainedTopic(TopicName T,TopicValue V,Client * C):topic{T},value{V},owner{C}{};
bool RetPtrCompare::operator() (RetainedTopic * const & lhs,RetainedTopic * const & rhs)const{
	return (lhs->topic)<(rhs->topic);};



Client::Client(ClientOpsIF * _cif,size_t _N):N{_N},cif{_cif}{ //Es el tamaño de la cola y le puntero al ClientOPSIF
	
		th=thread(&Client::thread_handler,this);
		//th=thread(&Queue <Message *>::get,&recvQueue,message);
	};

void Client::deleteSubsTopics(){
	for_each(subscriptions.begin(),subscriptions.end(),[](Subscription* pPtr){Broker::getInstance()->delSubscription(pPtr);}); 
	//Borro todos de la lista del broker
	for_each(subscriptions.begin(),subscriptions.end(),[](Subscription* pPtr){ delete pPtr; });
	//Libera la memoria de los subscriptions
	for_each(topics.begin(),topics.end(),[](RetainedTopic* pPtr){Broker::getInstance()->delRetained(pPtr);});
	for_each(topics.begin(),topics.end(),[](RetainedTopic* pPtr){ delete pPtr; });
	//Idem con los RetainedTopic
	};

void Client::thread_handler(){
	Message * message; //Doble puntero para poder recibir el puntero
	Message::Type type; //tipo del mensaje
	bool connected=false;
	while (1){ //Escucha todo lo que le manden
		recvQueue.get(&message); //Saca de la cola
		
		type=message->getType();
		if (type == Message::Type::CONNECT){
			ConnectMsg * m=static_cast<ConnectMsg *>(message);
			if (Broker::getInstance()->compareUserPass(m->getUsername(),m->getPassword())){
				connected=true;
				unique_lock <mutex> cif_lock (m_cif); //Adquiere el mutex, 
				cif->recvMsg(ConnAckMsg()); //Mandar el CONNACK
				}
			else{
				unique_lock <mutex> cif_lock (m_cif); //Adquiere el mutex, 
				cif->recvMsg(ConnAckMsg(ConnAckMsg::Status::LOGIN_ERROR)); //Mandar el CONNACK con LOGIN_ERROR
				}
			//cout<<"Connecting"<<endl;
			}
		if (connected && type==Message::Type::SUSCRIBE){

			SuscribeMsg * m=static_cast<SuscribeMsg *>(message);
			Subscription * S=new Subscription (m->getTopic(),this);
			subscriptions.insert(S); 	//Añadir el suscribe a la lista propia
			Broker::getInstance()->addSubscription(S);//Añade el suscribe a la lista del broker
			//y si hay un RetainedTopic lo manda de vuelta (hehcho en el addSubscription)
			
			}
		if (connected && type==Message::Type::PUBLISH){
			PublishMsg * m=static_cast<PublishMsg *>(message);
			if (m->getRetain()){
				RetainedTopic * T=new RetainedTopic(m->getTopic(),m->getValue(),this);
				Broker::getInstance()->addRetained(T); //Lo añado a la lista del Broker y si existe uno previo lo borra
				auto it=topics.find(T); //Busco uno igual (ie: con el mismo topico)
				if (it!=topics.end()) { //si existe
					delete (*it); // borra el puntero a RetainedTopic
					topics.erase(it); //y lo borro de la lista propia
					}
				topics.insert(T); //Y pone el nuevo
			}
			Broker::getInstance()->publish(*m); //el publish se encarga de repartirlo
			}
		if (connected && type==Message::Type::UNSUSCRIBE){
			UnsuscribeMsg * m=static_cast <UnsuscribeMsg *>(message);
			Subscription S(m->getTopic(),this); 
			Broker::getInstance()->delSubscription(&S);//Si la encontro borrarla de la lista del broker
			auto it=subscriptions.find(&S); //lo busca en la lista propia
			if (it!=subscriptions.end()){ //Si lo encontro
				delete (*it); //libera la memoria
				subscriptions.erase(it); // y lo borra de la lista
				}
			}
		if (type==Message::Type::DISCONNECT){
			deleteSubsTopics();
			delete message; //Esto lo hago porque no va a ejecutar el delete de abajo
			break; //Salir del while (dejar de escuchar)
			}
		delete message; //Aca va el delete del new del clone de cada mensaje
		}
	Broker::getInstance()->delQueue.put(this);	//Enviar una orden al broker de destruir este client
	};
	
void Client::sendMsg(const Message &m){
	recvQueue.put(m.clone());
	};
Client::~Client(){
	if (th.joinable()) th.join();
	};
	
void Client::sendPublish2Client (const PublishMsg &m){
	unique_lock <mutex> cif_lock (m_cif); //adquiere el mutex para evitar data race
	cif->recvMsg(m); //manda el mensaje
	};

	

