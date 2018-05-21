#include "Events.h"

const EventType ContextResizeEvent::sk_eventType = EV_TYPE("context_resize");
const EventType GameStartEvent::sk_eventType = EV_TYPE("game_start");
const EventType GamePausedEvent::sk_eventType = EV_TYPE("game_paused");
const EventType GameExitEvent::sk_eventType = EV_TYPE("game_exit");
const EventType CollisionEvent::sk_eventType = EV_TYPE("collision"); 
const EventType StarCollectEvent::sk_eventType = EV_TYPE("star_collect");
const EventType PowerUpCollectEvent::sk_eventType = EV_TYPE("power_collect");
const EventType PlayerLostEvent::sk_eventType = EV_TYPE("player_lost");