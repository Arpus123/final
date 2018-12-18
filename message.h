#ifndef MESSAGE
#define MESSAGE

#include <string>
#include <iostream>

using namespace std;
using TopicName =string;
using TopicValue =string;

class Message{	
	public:
		enum class Type {CONNECT, CONNACK, PUBLISH, SUSCRIBE, UNSUSCRIBE, DISCONNECT};
		Type getType() const;
		virtual Message *clone() const=0; //Metodo virtual para que cada hijo lo implemente y que se clone correctamente
		Message(Message::Type);
		virtual ~Message();
	private: 
		Type type;
	};
class ConnectMsg: public Message{
	private:
		string username;
		string password;
	public:
		ConnectMsg * clone() const;
		ConnectMsg(string u="", string p="");
		string getUsername() const;
		string getPassword() const;
		~ConnectMsg();
	};

class ConnAckMsg: public Message{
	public:
	enum class Status{CONNECTION_OK,LOGIN_ERROR};
	ConnAckMsg::Status getStatus() const;
	ConnAckMsg(Status S=ConnAckMsg::Status::CONNECTION_OK);
	ConnAckMsg * clone() const;
	~ConnAckMsg();
	private:
	Status status;
	};

class PublishMsg: public Message {
	public:
	PublishMsg(TopicName N="",TopicValue V="", bool ret=false);
	TopicName getTopic() const;
	TopicValue getValue() const;
	bool getRetain() const;
	PublishMsg * clone() const;
	~PublishMsg();
	private:
	TopicName topic;
	TopicValue value;
	bool retain;
};	 

class SuscribeMsg: public Message{
	public:
	SuscribeMsg(TopicName N="");
	SuscribeMsg * clone() const;
	TopicName getTopic() const;
	~SuscribeMsg();
	private:
	TopicName topic;
	};
class UnsuscribeMsg: public Message{
	TopicName topic;
	public:
		UnsuscribeMsg(TopicName N="");
		TopicName getTopic() const;
		UnsuscribeMsg * clone() const;
		~UnsuscribeMsg();
	};
class DisconnectMsg: public Message{
	public:
		DisconnectMsg();
		DisconnectMsg * clone() const;
		~DisconnectMsg();
	};

template <typename Enumeration>
auto as_integer(Enumeration const value)
-> typename std::underlying_type<Enumeration>::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}; //YO NO IMPLEMENTE ESTO, LO COPIE DE https://stackoverflow.com/questions/11421432/how-can-i-output-the-value-of-an-enum-class-in-c11
//LO USÉ PARA IMPRIMIR LOS getType
#endif
