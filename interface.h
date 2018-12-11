#ifndef INTERFACE
#define INTERFACE
#include "message.h"
class ClientOpsIF{
	public:
		virtual void recvMsg (const Message &)=0;
	};

class BrokerOpsIF{
	public:
		virtual void sendMsg (const Message &)=0;
	};
#endif
