#ifndef INTERFACE
#define INTERFACE
#include "interface.h"
#endif

class ClientOpsIF{
	public:
		virtual void recvMsg (const Message &)=0;
	};

class BrokerOpsIF{
	public:
		virtual void sendMsg (const Message &)=0;
	};
