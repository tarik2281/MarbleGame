//
//  GameApp.h
//  MarbleGame
//
//  Created by Tarik Karaca on 01.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef __MarbleGame__GameApp__
#define __MarbleGame__GameApp__

class ResourceManager;

class GameApp {
public:
    virtual void Initialize(int width, int height) = 0;
    virtual void Update() = 0;
    virtual void Render() = 0;
    virtual void Release() = 0;
    //virtual void HandleEvent(Event& event) = 0;
protected:
    ResourceManager* resourceManager;
};

#endif /* defined(__MarbleGame__GameApp__) */
