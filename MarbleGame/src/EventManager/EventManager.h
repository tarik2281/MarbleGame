//
//  EventManager.h
//  MarbleGame
//
//  Created by Tarik Karaca on 29.12.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__EventManager__
#define __MarbleGame__EventManager__

#include <map>
#include <set>
#include <queue>
#include <memory>

#include "../StringHash.h"

typedef Hash EventType;
#define EV_TYPE(identifier) StringHash(identifier)

class IEventData {
public:
    virtual EventType getEventType() const = 0;
};
typedef std::shared_ptr<IEventData> EventDataPtr;

class IEventListener {
public:
    virtual void HandleEvent(const EventDataPtr&) = 0;
};

class IEventManager {
public:
    virtual void AddEventListener(IEventListener*, EventType) = 0;
    virtual void RemoveEventListener(IEventListener*, EventType) = 0;
    
    virtual void QueueEvent(const EventDataPtr&) = 0;
    virtual void AbortEvent(IEventData const &) = 0;
    virtual void PollEvents() = 0;
    
    virtual void TriggerEvent(const EventDataPtr&) = 0;
};

class EventManager : public IEventManager {
public:
    void AddEventListener(IEventListener*, EventType);
    void RemoveEventListener(IEventListener*, EventType);
    
    void QueueEvent(const EventDataPtr&);
    void AbortEvent(IEventData const &);
    void PollEvents();
    
    void TriggerEvent(const EventDataPtr&);
    EventManager() {
		m_activeQueue = 0;
		m_processQueue = 0;
	}
    ~EventManager() { }
private:
    typedef std::set<IEventListener*> EventListenerList;
    typedef std::map<EventType, EventListenerList> ListenerMap;
    typedef std::pair<EventType, EventListenerList> ListenerPair;
    
    typedef std::queue<EventDataPtr> EventQueue;
    
    ListenerMap listeners;
    EventQueue queues[2];
    int m_activeQueue;
    int m_processQueue;
};

#endif /* defined(__MarbleGame__EventManager__) */
