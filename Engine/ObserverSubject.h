#pragma once
#include <map>
#include <string>
#include <vector>

class Message;
class Observer;

typedef std::map<std::string, std::vector<Observer&> > MessageListenerMap;
typedef std::map<std::string, std::vector<Observer&> >::iterator MessageListenerMapIterator;
typedef std::vector<Observer&>::iterator ObserverListIterator;

/// Abstract owner class for the Observer pattern
/// This class should be inherited by objects that want to be observed and pass messages to their Observers.
/// The concept is that observers are classes that will implement `Observer` and when an update is available,
/// the `onMessage` method of each observer class will be triggered.
/// 
/// Since this pattern exposes only specific methods, it is up to each impmentation to filter which messages
/// to send to each observer and it is up to observers to decide what message they want to act upon.
///
/// An alternative approach would be to use event listeners and store the listenr lambdas in a map, thereby
/// triggering only the events for a specific event. While this method gives more control and is still an
/// implemntation of the observer pattern, I have chosen to implement this simple observer pattern for
/// demonstration.
class ObserverSubject
{
	/// <summary>
	/// Default property for holding listeners. This should be parseb by implementers to trigger listeners.
	/// </summary>
	MessageListenerMap listeners = {};

	// Message handler (called when message occurs)
	virtual void BroadcastMessage(Message* msg) = 0;
};

