#include "Mapchip.h"
#include <string>
#include <cassert>

void Mapchip::Initialize() {
	SetDefaultMap();


	for (uint32_t y = 0u; y < height_; ++y) {
		for (uint32_t x = 0u; x < width_; ++x) {

			mapDatas_[y][x].Create(x, y);

		}
	}



}

void Mapchip::Initialize(const std::string& file_path) {

	LoadMapChip(file_path);

#pragma region マップ確保

	for (uint32_t y = 0u; y < height_; ++y) {
		for (uint32_t x = 0u; x < width_; ++x) {

			mapDatas_[y][x].Create(x, y);

		}
	}

#pragma endregion


}

//void Mapchip::Update() {
//}

void Mapchip::Draw() {
	for (uint32_t y = 0u; y < height_; ++y) {
		for (uint32_t x = 0u; x < width_; ++x) {
			mapDatas_[y][x].Draw();
		}
	}

}

void Mapchip::LoadMapChip(const std::string& file_path) {
	
	// ステージを読み込む
	std::ifstream file{ file_path };

	// もしファイルが読み込めなかったらその場で終了
	if (!file.is_open()) {
		assert(0 && "ステージファイルが読み込めません");
		return;
	}

	std::string line;
	int row = height_;

	while (std::getline(file, line) && row > 0u) {
		std::stringstream ss(line);
		int col = 0;
		std::string cell;

		while (std::getline(ss, cell, ',') && col < width_) {

			if (cell == "") {
				mapDatas_[row - 1][col] = ChipState::kNone;
			}
			else {
				mapDatas_[row - 1][col] = static_cast<ChipState>(std::stoul(cell));
			}

			col++;
		}

		row--;
	}

	// ファイルを閉じる
	file.close();

}

void Mapchip::SetDefaultMap() {
	for (uint32_t x = 0u; x < width_; ++x) {
		mapDatas_[0][x].chipState_ = ChipState::kBlock;
		mapDatas_[height_ - 1][x].chipState_ = ChipState::kBlock;

		if (x > width_ / 2 - 5 && x < width_ / 2 + 4) {
			mapDatas_[0][x].chipState_ = ChipState::kBlock;
		}
	}

	for (uint32_t y = 0u; y < height_; ++y) {
		mapDatas_[y][0].chipState_ = ChipState::kBlock;
		mapDatas_[y][width_ - 1u].chipState_ = ChipState::kBlock;
	}
}

void Mapchip::ChipData::Init() {
	switch (chipState_) {
	case Mapchip::ChipState::kNone:
		tex_ = 0u;
		break;
	case Mapchip::ChipState::kBlock:
		tex_ = TextureManager::Load("unbreakableBlock.png");
		break;
	default:
		tex_ = 0u;
		break;
	}
	if (tex_ == 0u) {
		sprite_ = nullptr;
	}
	else {
		sprite_.reset(Sprite::Create(tex_, pos_, 0.0f, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.5f, 0.5f }));
		sprite_->SetSize({ mapchipSize_, mapchipSize_ });
	}

}

void Mapchip::ChipData::Exit() {
	this->sprite_.release();
	this->chipState_ = ChipState::kNone;
	//this->transform_.reset();

}

void Mapchip::ChipData::Create(uint32_t x, uint32_t y) {
	Init();

	const float xCenter = width_ / mapchipSize_ - 0.5f;
	if (sprite_) {
		pos_ = { (x - xCenter) * mapchipSize_ + 40.0f, static_cast<float>(y) * mapchipSize_ + 1.f };
		sprite_->SetPosition(pos_);
	}
}

void Mapchip::ChipData::Draw() const {
	if (sprite_ == nullptr) {
		return;
	}
	sprite_->Draw();

}

Mapchip::ChipData& Mapchip::ChipData::operator=(ChipState state) {
	chipState_ = state;
	Init();
	return *this;
}
