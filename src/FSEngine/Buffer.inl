#include "pch.h"
#include "Buffer.h"

template<typename T>
inline void Buffer<T>::CreateBuffer(int startCapacity, std::function<void(T*)> onCreateObject)
{
	m_onCreation = onCreateObject;
	IncreaseSize(startCapacity);
}

template<typename T>
inline Buffer<T>::~Buffer()
{
	m_data.clear();
}

template<typename T>
inline T* Buffer<T>::Get(int id)
{
	return &m_data[id];
}

template<typename T>
inline void Buffer<T>::IncreaseSize(int toAdd)
{
	if(toAdd == -1)
		toAdd = m_data.size();
	int oldCapacity = m_data.size();
	m_data.reserve(oldCapacity + toAdd);

	for (int i = 0; i < toAdd; i++)
		m_data.push_back(T());
	if (!m_onCreation)
		return;
	for (int i = oldCapacity; i < m_data.size(); i++)
		m_onCreation(&m_data[i]);
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