#pragma once
#include <map>
#include <string>
#include <vector>

class Message;
class Observer;

typedef std::map<std::string, std::vector<Observer*> > MessageListenerMap;
typedef std::map<std::string, std::vector<Observer*> >::iterator MessageListenerMapIterator;
typedef std::vector<Observer*>::iterator ObserverListIterator;

/// Abstract owner class for the Observer pattern
/// This class should be inherited by objects that want to be observed and pass messages to their Observers
class ObserverSubject
{

	// Message handler (called when message occurs)
	virtual void BroadcastMessage(Message* msg) = 0;

};

