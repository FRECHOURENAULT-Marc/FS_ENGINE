#include "pch.h"

namespace FSE
{
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
	inline BUFFERC_IN_USE<O>* ECS::InitObjectBuffer()
	{
		BUFFERC_IN_USE<O>* buffer = new BUFFERC_IN_USE<O>(ONCREATION_BUFFERC_OBJECT(O));
		m_ScMgr->GetActiveScene<Scene>()->m_ComponentArrays.push_back((BUFFERC_IN_USE<Component>*)buffer);
		GetComponentTypeId<O>();
		return buffer;
	}


	template<typename C>
	inline C* ECS::GetAllFromType()
	{
		int id = GetComponentTypeId<C>();
		return ((BUFFERC_IN_USE<C>*)m_ScMgr->GetActiveScene<Scene>()->m_ComponentArrays[id])->GetAll();
	}

	template<typename C>
	inline C* ECS::GetComponent(int entityID)
	{
		int id = GetComponentTypeId<C>();

		Scene* sc = m_ScMgr->GetActiveScene<Scene>();
		auto componentArray = (BUFFERC_IN_USE<C>*)sc->m_ComponentArrays[id];
		C* c = &componentArray->Get(entityID);
		return c;
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
		if (c == nullptr)
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
	template<typename S>
	inline S* ECS::CreatePermanentScript()
	{
		S* script = new S;
		Get().AddPermanentScript(script);
		return script;
	}

}

