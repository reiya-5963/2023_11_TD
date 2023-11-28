#pragma once
#include "Player.h"	

class Player;

class InputState
{
protected:
	Player* player_ = nullptr;
public:
	virtual void SetPlayer(Player* player) {
		player_ = player;
	}
	virtual void Update() {};
};

class ActiveState : public InputState
{
public:
	void Update() override;

private:
	void MoveInputProcess();
	void ActionInput();

};

class InactiveState : public InputState
{
public:
	void Update() override;

};
