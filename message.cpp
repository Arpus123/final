
#include "message.h"

Message::Message(Message::Type T):type{T}{}; //Al construir un mensaje hay que si o si especificar el tipo
Message::Type Message::getType() const {return type;}; // devuelve el tipo del mensaje
Message::~Message(){};

ConnectMsg::ConnectMsg(string u,string p):Message{Message::Type::CONNECT},username{u},password{p}{};
string ConnectMsg::getUsername()const {return username;};
string ConnectMsg::getPassword()const {return password;};
ConnectMsg * ConnectMsg::clone() const{return new ConnectMsg(username,password);};
ConnectMsg::~ConnectMsg(){};

ConnAckMsg::ConnAckMsg(Status S):Message{Message::Type::CONNACK},status{S}{}; //Al construir hay que especificar el status
ConnAckMsg::Status ConnAckMsg::getStatus() const {return status;}; 
ConnAckMsg * ConnAckMsg::clone() const{return new ConnAckMsg(status);};
ConnAckMsg::~ConnAckMsg(){};

PublishMsg::PublishMsg(TopicName N,TopicValue V, bool ret):Message{Message::Type::PUBLISH},topic{N},value{V},retain{ret}{};
TopicName PublishMsg::getTopic()const{return topic;};
TopicValue PublishMsg::getValue()const{return value;};
bool PublishMsg::getRetain()const{return retain;};
PublishMsg * PublishMsg::clone() const{return new PublishMsg(topic,value,retain);};
PublishMsg::~PublishMsg(){};

SuscribeMsg::SuscribeMsg(TopicName N):Message{Message::Type::SUSCRIBE},topic{N}{};
TopicName SuscribeMsg::getTopic()const{return topic;};
SuscribeMsg * SuscribeMsg::clone() const{return new SuscribeMsg(topic);};
SuscribeMsg::~SuscribeMsg(){};

UnsuscribeMsg::UnsuscribeMsg(TopicName N):Message{Message::Type::UNSUSCRIBE},topic{N}{};
TopicName UnsuscribeMsg::getTopic()const{return topic;};
UnsuscribeMsg * UnsuscribeMsg::clone() const{return new UnsuscribeMsg();};
UnsuscribeMsg::~UnsuscribeMsg(){};

DisconnectMsg::DisconnectMsg():Message{Message::Type::DISCONNECT}{};
DisconnectMsg * DisconnectMsg::clone() const{return new DisconnectMsg();};
DisconnectMsg::~DisconnectMsg(){};
