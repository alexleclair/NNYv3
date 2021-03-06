#ifndef H_CAPSULE_VERSION
#define H_CAPSULE_VERSION

//Relative path here; we are in a folder nammed "Casules/".
#include "../Capsule.h"
#include "../SessionMgr.h"
//Virtual path here; these are included by the project settings
#include "database.h"
#include "Packet.h"
#include "resProtocol.h"

extern unsigned int g_client_version;
extern database::connection g_db;

class CapsuleVersion : public Capsule {
public:
	virtual void doit(SESSION session, Packet& capsule);
}; //end of class

#endif
