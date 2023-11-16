#pragma once
#include "Player.h"	

class Player;

class PlayerState
{
protected:
	Player* player_ = nullptr;
public:
	virtual void SetPlayer(Player* player) {
		player_ = player;
	}
	virtual void Update() {};
};


class ActiveState : public PlayerState
{
public:
	void Update() override;

};

class InactiveState : public PlayerState
{
public:
	void Update() override;

};
