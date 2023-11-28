#pragma once
#include <memory>
#include <Sprite.h>

class UIManager
{
public:
	void Initialize();
	void Update();
	void Draw();

public:
	std::unique_ptr<Sprite> sprite_;

};

