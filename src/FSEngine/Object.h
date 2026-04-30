#pragma once

namespace FSE
{
	class Object
	{
	protected:
		int m_ID = -1;
		bool m_IsActive = false;

		Object() = default;

		virtual void Reset() {}; // override this

	public:
		virtual void SetID(int id) { m_ID = id; };
		virtual void SetActive(bool isActive) { m_IsActive = isActive; };

		virtual int GetID() { return m_ID; };
		virtual bool IsActive() { return m_IsActive; };
	};
}

