#ifndef _EVENTS_H_
#define _EVENTS_H_

#include "EventManager.h"

class ContextResizeEvent : public IEventData {
public:
	static const EventType sk_eventType;
	EventType getEventType() const { return sk_eventType; }

	const int m_screenWidth;
	const int m_screenHeight;

	ContextResizeEvent(int width, int height) : m_screenWidth(width), m_screenHeight(height) { }
};

class GameStartEvent : public IEventData {
public:
	static const EventType sk_eventType;
	EventType getEventType() const { return sk_eventType; }
};

class GamePausedEvent : public IEventData {
public:
	static const EventType sk_eventType;
	EventType getEventType() const { return sk_eventType; }

	const bool m_paused;

	GamePausedEvent(bool paused) : m_paused(paused) { }
};

class GameExitEvent : public IEventData {
public:
	static const EventType sk_eventType;
	EventType getEventType() const { return sk_eventType; }
};

class PhysicsBody;
class CollisionEvent : public IEventData {
public:
	static const EventType sk_eventType;
public:
	EventType getEventType() const { return sk_eventType; }
	const bool m_added;
	const PhysicsBody* const m_body1;
	const PhysicsBody* const m_body2;

	CollisionEvent(bool added, const PhysicsBody* body1, const PhysicsBody* body2) :
		m_added(added),
		m_body1(body1),
		m_body2(body2) { }
};
#define IS_COLL_BODY(event, body) (event->m_body1 == body || event->m_body2 == body)

class StarCollectEvent : public IEventData {
public:
	static const EventType sk_eventType;
	EventType getEventType() const { return sk_eventType; }
};

class PowerUpCollectEvent : public IEventData {
public:
	static const EventType sk_eventType;
	EventType getEventType() const { return sk_eventType; }
};

class PlayerLostEvent : public IEventData {
public:
	static const EventType sk_eventType;
	EventType getEventType() const { return sk_eventType; }
};

#endif // _EVENTS_H_