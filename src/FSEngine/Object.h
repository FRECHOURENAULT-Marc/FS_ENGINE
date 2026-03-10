#pragma once

class Object
{
protected:
	int m_id = -1;
	bool m_isActive = false;

	Object() = default;

	virtual void Reset() {}; // override this

public:
	virtual void SetID(int id) { m_id = id; };
	virtual void SetActive(bool isActive) { m_isActive = isActive; };

	virtual int GetID() { return m_id; };
	virtual bool IsActive() { return m_isActive; };
};