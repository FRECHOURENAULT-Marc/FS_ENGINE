#pragma once

template<typename T>
class Buffer
{
protected:
	std::vector<T> m_data;
	std::function<void(T*)> m_onCreation = nullptr;

	void CreateBuffer(int startCapacity, std::function<void(T*)> onCreateObject = nullptr);

public:
	Buffer() = default;
	~Buffer();

	T* Get(int id);
	std::vector<T>& GetAll() { return m_data; };
	int Capacity() { return m_data.size(); }
	void IncreaseSize(int toAdd = -1);
};

template<typename O>
class ObjectBuffer : public Buffer<O>
{
	int m_lastObjectCreated = 0;
	void SetNewObjectID(O* newObject);

public:
	ObjectBuffer(int startCapacity);

};

#include "Buffer.inl"
