#pragma once

/// <summary>
/// Abstract owner class for the Observer pattern
/// This class should be inherited by objects that want to observe ObserverSubjects
/// They will be notified of any messages.
/// 
/// <seealso cref="ObserverSubject"/>
/// </summary>
class Observer
{
public:
	/// Receive a message (called when notified of a message by the subject)
	virtual void OnMessage(Message<std::any>*) = 0;
};

