//
//  EventManager.cpp
//  MarbleGame
//
//  Created by Tarik Karaca on 29.12.13.
//  Copyright (c) 2013 Tarik Karaca. All rights reserved.
//

#include "EventManager.h"

void EventManager::AddEventListener(IEventListener* listener, EventType type) {
    ListenerMap::iterator it = listeners.find(type);
    
    if (it == listeners.end()) {
        it = listeners.insert(ListenerPair(type, EventListenerList())).first;
    }
    
    it->second.insert(listener);
}

void EventManager::RemoveEventListener(IEventListener* listener, EventType type) {
    ListenerMap::iterator it = listeners.find(type);
    
    if (it == listeners.end()) {
        return;
    }
    
    it->second.erase(listener);
}

void EventManager::QueueEvent(const EventDataPtr& event) {
    queues[m_activeQueue].push(event);
}

void EventManager::AbortEvent(const IEventData &event) {
    
}

void EventManager::PollEvents() {
    m_processQueue = m_activeQueue;
    m_activeQueue = (m_activeQueue + 1) % 2;
    
    EventQueue& q = queues[m_processQueue];
    while (!q.empty()) {
        const EventDataPtr& data = q.front();
        ListenerMap::iterator res = listeners.find(data->getEventType());
        
        if (res != listeners.end()) {
            EventListenerList& l = res->second;
            EventListenerList::iterator it;
            for (it = l.begin(); it != l.end(); it++) {
                (*it)->HandleEvent(data);
            }
        }
        
        q.pop();
    }
}

void EventManager::TriggerEvent(const EventDataPtr& event) {
    EventListenerList& l = listeners.find(event->getEventType())->second;
}