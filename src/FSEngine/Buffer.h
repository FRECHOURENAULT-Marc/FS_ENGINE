#pragma once

#define BUFFERC_SMALL_CPACITY 128

#define DECLARE_BUFFERC(Name, Size)											\
template<typename T>														\
class BufferC##Name															\
{																			\
protected:																	\
	T m_Data[Size];															\
public:																		\
	BufferC##Name(std::function<void(T*, int)> onCreation = nullptr) {		\
		for (int i = 0; i < Size; i++)										\
			m_Data[i] = T();												\
		if (!onCreation)													\
			return;															\
		onCreation(m_Data, Size);											\
	}																		\
	~BufferC##Name() = default;												\
	T& Get(int id) { return m_Data[id]; }									\
	T* GetAll() { return m_Data; }											\
	int Capacity() { return Size; }											\
	T& operator[](int id) { return m_Data[id]; }							\
};

#define ONCREATION_BUFFERC_OBJECT(Type)					\
[](Type* data, int size) {								\
	for (int i = 0; i < size; i++)						\
		data[i].SetID(i);								\
	}													\



namespace FSE
{
	DECLARE_BUFFERC(64, 64)
	DECLARE_BUFFERC(128, 128)
	DECLARE_BUFFERC(256, 256)
	DECLARE_BUFFERC(512, 512)
	DECLARE_BUFFERC(1024, 1024)
	DECLARE_BUFFERC(2048, 2048)
	DECLARE_BUFFERC(4096, 4096)
	DECLARE_BUFFERC(8192, 8192)

	template<typename T>
	class Buffer
	{
	protected:
		std::vector<T> m_Data;
		std::function<void(T*)> m_OnCreation = nullptr;

		void CreateBuffer(int startCapacity, std::function<void(T*)> onCreateObject = nullptr);

	public:
		Buffer() = default;
		~Buffer();

		T* Get(int id);
		std::vector<T>& GetAll() { return m_Data; };
		int Capacity() { return m_Data.size(); }
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

}

#include "Buffer.inl"
