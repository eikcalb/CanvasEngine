#pragma once
#include <map>
#include <string>
#include <vector>

class Observer;

typedef std::map<std::string, std::vector<Observer&> > MessageListenerMap;
typedef std::map<std::string, std::vector<Observer&> >::iterator MessageListenerMapIterator;
typedef std::vector<Observer&>::iterator ObserverListIterator;

/// Owner class for the Observer pattern
/// This class should be inherited by objects that want to be observed and pass messages to their Observers.
/// The concept is that observers are classes that will implement `Observer` and when an update is available,
/// the `onMessage` method of each observer class will be triggered for the event.
/// 
/// Since this pattern exposes only specific methods, it is up to each implementation to filter which messages
/// to send to each observer and it is up to observers to decide what message they want to act upon.
///
/// Event listeners store the listener lambdas in a map, thereby triggering only the events for a specific
/// event.
class ObserverSubject
{
protected:
	/// <summary>
	/// Default property for holding listeners. This should be parsed by implementers to trigger listeners.
	/// </summary>
	MessageListenerMap listeners = {};

	// Message handler (called when message occurs)
	void BroadcastMessage(Message<std::any>* msg) {
		const auto msgListeners = listeners[msg->GetMessageType()];
		for (Observer& listener : msgListeners) {
			listener.OnMessage(msg);
		}
	}
};

