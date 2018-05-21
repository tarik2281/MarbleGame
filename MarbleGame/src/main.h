#ifndef _MAIN_H_
#define _MAIN_H_

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int SHADOWMAP_RESOLUTION;
extern int NUM_CASCADES;

class ResourceManager;
class EventManager;
class InputManager;
class ScreenManager;
class PhysicsWrapper;
class MaterialList;

extern ResourceManager* g_pResourceManager;
extern EventManager* g_pEventManager;
extern InputManager* g_pInputManager;
extern ScreenManager* g_pScreenManager;
extern PhysicsWrapper* g_pPhysics;
extern MaterialList* g_pMaterials;

extern float g_fFrameRate;
extern float g_fMaxFPS;

#endif