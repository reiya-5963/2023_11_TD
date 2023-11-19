#pragma once
#include "Player.h"

class PlayerState
{
protected:
	Player* player_;

public:
	void SetPlayer(Player* player) { player_ = player; }
	virtual void Update() = 0;

};

class RootState : public PlayerState
{
public:
	void Update() override;
};