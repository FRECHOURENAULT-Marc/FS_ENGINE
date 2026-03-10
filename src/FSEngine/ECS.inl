#include "pch.h"
#include "ECS.h"

//////////////////
/// PUBLIC
//////////////////

inline int GetNextComponentTypeId()
{
	static int lastId = 0;
	return lastId++;
}
// Create a unique ID for each component type (one id by template)
// Compiler create unique version of the template function by type used
// It increments ID by one each time new type is used ( * * magic * * )
template<typename T>
int GetComponentTypeId()
{
	static int typeId = GetNextComponentTypeId();
	return typeId;
}

template<typename O>
inline ObjectBuffer<O>* ECS::InitComponentBuffer()
{
	ObjectBuffer<O>* buffer = new ObjectBuffer<O>(ENTITES_MAX);
	m_componentArrays.push_back((ObjectBuffer<Component>*)buffer);
	GetComponentTypeId<O>();
	return buffer;
}


template<typename C>
inline std::vector<C>& ECS::GetAllFromType()
{
	int id = GetComponentTypeId<C>();
	return ((ObjectBuffer<C>*)m_componentArrays[id])->GetAll();
}

template<typename C>
inline C* ECS::GetComponent(int entityID)
{
	int id = GetComponentTypeId<C>();
	return ((ObjectBuffer<C>*)m_componentArrays[id])->Get(entityID);
}

template <typename C>
inline C* ECS::AddComponent(int entityID)
{
	C* c = this->GetComponent<C>(entityID);
	c->Reset();
	c->SetActive(true);
	return c;
}

template<typename C>
inline void ECS::RemoveComponent(int entityID)
{
	C* c = GetComponent<C>(entityID);
	if(c == nullptr)
		return;
	c->SetActive(false);
	c->Reset();
}

template<typename S>
inline S* ECS::CreateScript()
{
	S* script = new S;
	Get().AddScript(script);
	return script;
}

