#ifndef H_MAIN
#define H_MAIN

//STD library includes
#include <iostream>

//ACE library includes
//#include "ace/OS.h"
#include "ace/Thread.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/Acceptor.h"
#include "ace/Reactor.h"

//Common resource files
#include "resTypedef.h"
#include "resProtocol.h"

//TestRunnable
//#include "Thread.h"

//For all the threading; like the RealmConnector thread
//#include "RealmConnector.h"

//Things the server needs
#include "PacketHandler.h"

#endif
