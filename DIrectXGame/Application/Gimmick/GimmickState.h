#pragma once
#include "Gimmick/IGimmick.h"

class IGimmick;

class GimmickState
{
protected:
	IGimmick* gimmick_ = nullptr;

public:
	virtual void SetGimmick(IGimmick* gimmick) { gimmick_ = gimmick; }
	virtual void Update() {};

};

class GimmickActiveState : public GimmickState 
{
public:
	void Update() override;
};

class GimmickInactiveState : public GimmickState
{
public:
	void Update() override;
};

