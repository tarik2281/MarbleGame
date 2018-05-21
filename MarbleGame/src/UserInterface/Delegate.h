//
//  Delegate.h
//  MarbleGame
//
//  Created by Tarik Karaca on 11.01.14.
//  Copyright (c) 2014 Tarik Karaca. All rights reserved.
//

#ifndef MarbleGame_Delegate_h
#define MarbleGame_Delegate_h

template<class R, class ... Args>
class Delegate {
    typedef R (*stub_type)(void*, Args ...);
    
    void* object_ptr;
    stub_type stub_ptr;
    
    template <class T, R (T::*TMethod)(Args... args)>
    static R method_stub(void *object_ptr, Args ... args) {
        T* p = static_cast<T*>(object_ptr);
        return (p->*TMethod)(args...);
    }
public:
    Delegate() : object_ptr(0), stub_ptr(0) { }
    
    template <class T, R (T::*TMethod)()>
    static Delegate<R, Args ...> from_member(T* object_ptr) {
        Delegate<R, Args ...> del;
        del.object_ptr = object_ptr;
        del.stub_ptr = &method_stub<T, TMethod>;
        return del;
    }
    
    R operator()(Args ... args) const {
        return (*stub_ptr)(object_ptr, args ...);
    }
};

#define MakeDelegate(class, function, object) from_member<class, &class::function>(object)

#endif
