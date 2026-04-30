#include "pch.h"
#include "Buffer.h"

namespace FSE
{

	template<typename T>
	inline void Buffer<T>::CreateBuffer(int startCapacity, std::function<void(T*)> onCreateObject)
	{
		m_OnCreation = onCreateObject;
		IncreaseSize(startCapacity);
	}

	template<typename T>
	inline Buffer<T>::~Buffer()
	{
		m_Data.clear();
	}

	template<typename T>
	inline T* Buffer<T>::Get(int id)
	{
		return &m_Data[id];
	}

	template<typename T>
	inline void Buffer<T>::IncreaseSize(int toAdd)
	{
		if (toAdd == -1)
			toAdd = m_Data.size();
		int oldCapacity = m_Data.size();
		m_Data.reserve(oldCapacity + toAdd);

		for (int i = 0; i < toAdd; i++)
			m_Data.push_back(T());
		if (!m_OnCreation)
			return;
		for (int i = oldCapacity; i < m_Data.size(); i++)
			m_OnCreation(&m_Data[i]);
	}

	template<typename O>
	inline void ObjectBuffer<O>::SetNewObjectID(O* newObject)
	{
		newObject->SetID(m_lastObjectCreated);
		m_lastObjectCreated++;
	}

	template<typename O>
	ObjectBuffer<O>::ObjectBuffer(int startCapacity)
	{
		this->CreateBuffer(startCapacity, std::bind(&ObjectBuffer::SetNewObjectID, this, std::placeholders::_1));
	}

}