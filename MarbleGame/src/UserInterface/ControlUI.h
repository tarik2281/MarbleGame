//
//  ControlUI.h
//  MarbleGame
//
//  Created by Tarik Karaca on 30.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__ControlUI__
#define __MarbleGame__ControlUI__

#include "VectorMath.h"

#include <memory>
#include <set>

class KeyInputData;
class MButtonInputData;
class MMotionInputData;
class MScrollInputData;

class UIScreen;

class ControlUI;

typedef std::shared_ptr<ControlUI> ControlPtr;

class ControlUI {
    friend class UIScreen;
protected:
    Vector2 m_origin;
    Vector2 m_halfSize;
    Vector2 m_pixelPositionOffset = Vector2(0, 0);
    Vector2	m_absoluteOrigin;
    UIScreen* m_owner = 0;
    
    void SetAbsolutePosition(int, int);
public:
    virtual bool HandleKeyEvent(const KeyInputData&) { return false; }
    virtual bool HandleMButtonEvent(const MButtonInputData&) { return false; }
    virtual bool HandleMMotionEvent(const MMotionInputData&) { return false; }
    virtual bool HandleMScrollEvent(const MScrollInputData&) { return false; }
    
    virtual void Render(float opacity = 1.0f) = 0;
    
    const Vector2& GetOrigin() const { return m_origin; }
    void SetOrigin(const Vector2& v) { m_origin = v; }
    void SetOrigin(float x, float y) { m_origin = Vector2(x, y); }
    
    const Vector2& GetPixelPositionOffset() const { return m_pixelPositionOffset; }
    void SetPixelPositionOffset(const Vector2& v) { m_pixelPositionOffset = v; }
    void SetPixelPositionOffset(int x, int y) { m_pixelPositionOffset = Vector2(x, y); }
    
    const Vector2& GetHalfSize() const { return m_halfSize; }
    void SetHalfSize(const Vector2& v) { m_halfSize = v; }
    void SetHalfSize(float x, float y) { m_halfSize = Vector2(x, y); }
    
    virtual void SetOwner(UIScreen* ref) { m_owner = ref; }
    
    virtual void OnResize(int width, int height) { SetAbsolutePosition(width, height); }
};


class ControlContainer {
public:
	bool HandleKeyEvent(const KeyInputData&);
	bool HandleMButtonEvent(const MButtonInputData&);
	bool HandleMMotionEvent(const MMotionInputData&);
	bool HandleMScrollEvent(const MScrollInputData&);

	void Render();

	void AddControl(const ControlPtr&);
	void RemoveControl(const ControlPtr&);
private:
	friend class ControlUI;
	
	void AddFontLabel();
	void AddSprite();

	void RemoveFontLabel();
	void RemoveSprite();

	ID3D10Buffer* m_vertexBuffer;
	ID3D10Buffer* m_indexBuffer;
	int m_numIndices;
};

class ContainerUI : public ControlUI {
protected:
    typedef std::set<ControlPtr> ControlList;
    
    ControlList m_controls;
public:
    bool HandleKeyEvent(const KeyInputData&);
    bool HandleMButtonEvent(const MButtonInputData&);
    bool HandleMMotionEvent(const MMotionInputData&);
    bool HandleMScrollEvent(const MScrollInputData&);
    
    void Render(float);
    
    void AddControl(const ControlPtr&);
    void RemoveControl(const ControlPtr&);
    
    void SetOwner(UIScreen*);
};

inline void ContainerUI::AddControl(const ControlPtr &control) {
    if (!control)
        return;
    
    control->SetOwner(m_owner);
    m_controls.insert(control);
}

inline void ContainerUI::RemoveControl(const ControlPtr &control) {
    if (!control)
        return;
    
    control->SetOwner(NULL);
    m_controls.erase(control);
}

inline void ContainerUI::SetOwner(UIScreen *ref) {
    m_owner = ref;
    
    ControlList::iterator it;
    for (it = m_controls.begin(); it != m_controls.end(); ++it) {
        (*it)->SetOwner(ref);
    }
}

#endif /* defined(__MarbleGame__ControlUI__) */
