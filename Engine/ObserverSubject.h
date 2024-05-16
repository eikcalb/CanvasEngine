#pragma once
#include <map>
#include <string>
#include <vector>
#include <unordered_map>

#include "Message.h"
#include "Observer.h"

typedef std::unordered_map<std::string, std::vector<std::shared_ptr<Observer>>> MessageListenerMap;
typedef std::map<std::string, std::vector<std::shared_ptr<Observer>> >::iterator MessageListenerMapIterator;
typedef std::vector<std::shared_ptr<Observer>>::iterator ObserverListIterator;

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
	void BroadcastMessage(Message* msg) {
		const auto msgListeners = listeners[msg->GetMessageType()];
		for (auto& listener : msgListeners) {
			listener->OnMessage(msg);
		}
	}

	~ObserverSubject() {
	}

public:
	void ClearAllListeners() {/* listeners.clear();*/ }

	bool Observe(std::string type, std::shared_ptr<Observer> observer) {
		// Accessing a map using the index form invokes the default constructor
		// for the type specified if it does not exist.
		auto& msgListeners = listeners[type];
		auto i = std::find(msgListeners.begin(), msgListeners.end(), observer);

		// Make entry and add listener
		if (i == msgListeners.end())
		{
			msgListeners.push_back(observer);
			return true;
		}

		return false;
	}

	bool UnObserve(std::string type, std::shared_ptr<Observer> observer) {
		auto& msgListeners = listeners[type];
		auto i = std::find(msgListeners.begin(), msgListeners.end(), observer);

		// Exists?
		if (i != msgListeners.end())
		{
			msgListeners.erase(i);
			return true;
		}

		return false;
	}
};

