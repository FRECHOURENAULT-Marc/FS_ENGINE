#pragma once

class ECS;

class Script 
{
protected:
	ECS* m_ecs;
public:
	Script();

	virtual void OnStart() {};
	virtual void OnUpdate() {};
};

//Declare a script derived class
#define DECLARE_SCRIPT(name)    \
class name : public Script      \
{                               \
public :                        \
name() : Script() {};
//Declare a script derived class from another script
#define DECLARE_CHILD_SCRIPT(name, parent)  \
class name : public parent                  \
{                                           \
public :                                    \
name() : parent() {};
//same as };
#define END_SCRIPT };
//Add a script to the ECS (can be use everywhere in the code, after the script declaration)
//Create a lambda (function with no capture (= havent access to an extern variable) and without parameter) that execute itself in the same time
//We use inline to avoid multiple definitions errors, especially in .h files
#define ADD_SCRIPT(name)                    \
    inline bool name##Registered = [](){    \
        ECS::Get().AddScript(new name());   \
        return true;                        \
    }();