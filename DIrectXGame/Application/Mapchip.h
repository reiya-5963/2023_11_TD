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
	// シングルトンのお約束
	Mapchip() = default;
	Mapchip(const Mapchip&) = delete;
	Mapchip& operator=(const Mapchip&) = delete;
	~Mapchip() = default;


public:
	// チップの種類
	enum class ChipState : uint32_t {
		kNone,
		kBlock,
		kRedBlock,
		kBlueBlock,

	};

	// 1チップの情報
	struct ChipData {
		ChipState chipState_ = ChipState::kNone;
		std::unique_ptr<WorldTransform> transform_;
		std::unique_ptr<Model> model_;

		bool showVisible_ = false;

		void Init();

		void Update(const ViewProjection& viewProjection);

		void Exit();

		void Create(uint32_t x, uint32_t y);

		void Draw(const ViewProjection& viewProjection) const;

		ChipData& operator=(ChipState state);
		bool operator==(ChipState state) const { return chipState_ == state; }

	};

public:
	// シングルトン
	static Mapchip* const GetInstance() {
		//static DirectResourceLeakChecker leakChacker{};
		static Mapchip instance{};
		return &instance;
	}


	void Initialize();
	void Initialize(const std::string& file_path);

	void Update(const ViewProjection& viewProjection);

	void Draw(const ViewProjection &viewProjection);

	void LoadMapChip(const std::string& file_path);

	void SetDefaultMap();


	float GetDistanceToGround(const Vector2& localPos) const;

	/*const ChipData& GetChipData(const Vector2& vec) const;*/


	const ChipData& GetChipData(const Vector2& vec) const;

	Vector3 HitMap(const Vector3& beforePos, const Vector3& afterPos, float radius) const;

	static Vector2 GlobalToLocal(const Vector3& global) {
		const float xCenter = width_ / 2.0f;
		return Vector2{ (global.x / 2.f) + xCenter, (global.y - 1.f) / 2.f };
	}

private:
	inline static const int kChipSize = 32;
	inline static const float scale_ = 2.0f;

	static const uint32_t width_ = 48;

	static const uint32_t height_ = 30;

	std::array<std::array<ChipData, width_>, height_> mapDatas_;

	std::unique_ptr<Model> model_;
	
	const std::array<std::array<ChipData, width_>, height_>& GetMapArray()const { return mapDatas_; }
};

