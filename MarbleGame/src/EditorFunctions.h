#ifndef _EDITOR_H_
#define _EDITOR_H_


#ifdef _WINDLL

#include "src/Maths/VectorMath.h"
#include "src/Maths/GeometryMath.h"


#define _out_

extern "C" __declspec(dllexport) bool _cdecl CreateView(int* hwnd);
extern "C" __declspec(dllexport) void _cdecl RenderView();
extern "C" __declspec(dllexport) void _cdecl ReleaseView();
extern "C" __declspec(dllexport) void _cdecl ResizeView(int, int);
extern "C" __declspec(dllexport) void _cdecl NewLevel();
extern "C" __declspec(dllexport) void _cdecl LoadLevel(const char* path);
extern "C" __declspec(dllexport) void _cdecl SaveLevel(const char* path);
extern "C" __declspec(dllexport) void _cdecl WndProc(int* hwnd, int msg, int wParam, int lParam);
extern "C" __declspec(dllexport) const char* _cdecl PickObject();
extern "C" __declspec(dllexport) void _cdecl PickTempObject();

struct EditorMaterial {
	const char* name;
	const char* diffuseMap;
	const char* normalMap;
};
extern "C" __declspec(dllexport) bool _cdecl MaterialsBegin(_out_ EditorMaterial*);
extern "C" __declspec(dllexport) bool _cdecl MaterialsNext(_out_ EditorMaterial*);

struct ObjectData {
	char flag;
	Vector3 position;
	Vector3 rotation;
	const char* parent;
};
extern "C" __declspec(dllexport) void _cdecl GetObjectDataByName(const char*, _out_ ObjectData*);
extern "C" __declspec(dllexport) void _cdecl SetPositionForObject(const char*, Vector3);
extern "C" __declspec(dllexport) void _cdecl AddRotationForObject(const char*, Quaternion);
extern "C" __declspec(dllexport) void _cdecl AddRotationForPlatform(const char*, int);

extern "C" __declspec(dllexport) void _cdecl GetMouseRay(_out_ Ray*);
extern "C" __declspec(dllexport) void _cdecl GetPositionOnPlane(Ray, float, _out_ Vector3*);
extern "C" __declspec(dllexport) void _cdecl GetPositionOnCamPlane(Ray, Vector3, _out_ Vector3*);
extern "C" __declspec(dllexport) const char* _cdecl GetWorldPosition(Ray, _out_ Vector3*);

extern "C" __declspec(dllexport) const char* _cdecl AddObject(Vector3, char, const char*);

extern "C" __declspec(dllexport) void _cdecl RemoveObjectByName(const char*);

extern "C" __declspec(dllexport) bool _cdecl ObjectsBegin(_out_ const char**);
extern "C" __declspec(dllexport) bool _cdecl ObjectsNext(_out_ const char**);

extern "C" __declspec(dllexport) void _cdecl SetPlatformResizing(bool);
extern "C" __declspec(dllexport) void _cdecl GetPlatformData(const char*, _out_ Vector3*);
extern "C" __declspec(dllexport) void _cdecl SetPlatformMaterial(const char*, const char*);

extern "C" __declspec(dllexport) void _cdecl SetNameForObject(const char*, const char*);

extern "C" __declspec(dllexport) void _cdecl SetTargetForTeleporter(const char*, const char*);
extern "C" __declspec(dllexport) void _cdecl GetTeleporterData(const char*, _out_ const char**);

extern "C" __declspec(dllexport) void _cdecl AddTemporaryObject(char);
extern "C" __declspec(dllexport) void _cdecl RemoveTemp();
extern "C" __declspec(dllexport) void _cdecl SetTempPosition(Vector3);

#endif

#endif // _EDITOR_H_