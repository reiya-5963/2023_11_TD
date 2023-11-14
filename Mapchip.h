#pragma once
#include <stdint.h>
#include <array>
#include <string>
#include <fstream>
#include <sstream>


#include <memory>
#include "R_Math.h"
#include "Model.h"
#include "Sprite.h"

class Mapchip {
	Mapchip() = default;
	Mapchip(const Mapchip&) = delete;
	Mapchip& operator=(const Mapchip&) = delete;
	~Mapchip() = default;


public:
	enum class ChipState : uint32_t{
		kNone,
		kBlock,
	};

	struct ChipData {
		ChipState chipState_ = ChipState::kNone;
		Vector2 pos_;
		uint32_t tex_;
		std::unique_ptr<Sprite> sprite_;
		//std::unique_ptr<WorldTransform> trnasform_;
		//std::unique_ptr<Model> model_;

		void Init();

		void Exit();

		void Create(uint32_t x, uint32_t y);

		void Draw() const;

		ChipData& operator=(ChipState state);
		bool operator==(ChipState state) const { return chipState_ == state; }

	};

public:
	static Mapchip* const GetInstance() {
		//static DirectResourceLeakChecker leakChacker{};
		static Mapchip instance{};
		return &instance;
	}


	void Initialize();
	void Initialize(const std::string& file_path);

	//void Update();

	void Draw();

	void LoadMapChip(const std::string& file_path);

	void SetDefaultMap();

private:

	static const uint32_t width_ = 24;

	static const uint32_t height_ = 15;
	inline static const float mapchipSize_ = 1280.0f / 26.0f;

	std::array<std::array<ChipData, width_>, height_> mapDatas_;

	std::unique_ptr<Model> model_;

};

