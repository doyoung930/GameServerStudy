#pragma once
#include"header.h"

class PlayerComponent
{
public:
	int x, y;
	int id;
	int myid;
	RECT m_size;
	TCHAR str[100];
public:
	PlayerComponent() { cout << "잘못된 객체가 들어갔습니다!!" << endl; };
	PlayerComponent(int id, int myid):x(0),y(0),id(id),myid(myid)
	{
		m_size = { 100,100,150,150 };
		ZeroMemory(str, sizeof(str));
	}
	~PlayerComponent() {};
};




class CPlayer
{
private:
	CPlayer();
	~CPlayer();
private:
	static CPlayer* m_pInst;

public:
	static CPlayer* GetInst()
	{
		if (!m_pInst)
			m_pInst = new CPlayer;
		return m_pInst;
	}

	static void DestroyInst()
	{
		if (m_pInst)
		{
			delete m_pInst;
			m_pInst = NULL;
		}
	}
private:
	unordered_map<int, PlayerComponent*> m_players;
	
	
public:
	int GetX(int key)
	{
		return m_players[key]->x;
	}
	int GetY(int key)
	{
		return m_players[key]->y;
	}

	void SetX(int key,int val)
	{
		m_players[key]->x = val;
	}
	void SetY(int key,int val)
	{
		m_players[key]->y=val;
	}
	RECT GetSize(int key)
	{
		return m_players[key]->m_size;
	}

	PlayerComponent* GetPlayer(int key)
	{
		return m_players[key];
	}

	void SetSize(int key,RECT val)
	{
		m_players[key]->m_size = val;
	}
	
	bool AddPlayer(int id)
	{
		if (m_players.find(id) == m_players.end()) {
		if (m_players.empty())
		{
			cout << "나 추가됨" << endl;
			PlayerComponent* p = new PlayerComponent(id, id);
			m_players.emplace(id, p);
			//처음 일때 전체 넘겨달라고 요청
			return true;
		}
		else
		{
			
				PlayerComponent* p = new PlayerComponent(id, 0);
				m_players.emplace(id, p);
			
			return false;
		}
		}
		return false;
	}
	void DestroyAllPlayers()
	{
		for (auto& a : m_players)
		{
			if(a.second)
			delete a.second;
		}
		m_players.clear();
	}
	void DeletePlayer(int key)
	{
		if(m_players.find(key)!=m_players.end())
		delete m_players[key];
		m_players.erase(key);
	}
	//void Update(float deltaTime);
	void Render(HDC hdc, float fDeltaTime);

	PlayerComponent* GetAvatar()
	{
		for (auto& a : m_players)
		{
			if (a.second->myid != 0)
				return a.second;
		}

		return NULL;
	}
};

