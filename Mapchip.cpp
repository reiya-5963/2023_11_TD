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

void Mapchip::Draw(const ViewProjection& viewProjection) {
	for (uint32_t y = 0u; y < height_; ++y) {
		for (uint32_t x = 0u; x < width_; ++x) {
			mapDatas_[y][x].Draw(viewProjection);
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

float Mapchip::GetDistanceToGround(const Vector2& localPos) const {
	float distance = 0.f;

	const float decimalOnlyValue = localPos.y - std::floor(localPos.y);

	Vector2 leftDown;
	leftDown.x = localPos.x + -0.5f;
	leftDown.y = localPos.y + 0.0f;

	Vector2 rightDown;
	rightDown.x = localPos.x + 0.5f;
	rightDown.y = localPos.y + 0.0f;


	if (
		(leftDown.x < 0.f || rightDown.x >= width_)		// 左右が画面外
		|| (localPos.y + 0.5f >= height_ || leftDown.y < 0.f)	// 上下が画面外
		) {
		return -1.f;
	}

	while (true) {

		--leftDown.y;
		if (
			leftDown.y < 0.f	// 上下が画面外
			) {
			return -1.f;
		}

		if (GetChipData(leftDown).chipState_ != ChipState::kNone) {
			return distance + decimalOnlyValue;
		}

		--rightDown.y;
		if (GetChipData(rightDown).chipState_ != ChipState::kNone) {
			return  distance + decimalOnlyValue;
		}

		++distance;
	}

	return 0.f;
}

const Mapchip::ChipData& Mapchip::GetChipData(const Vector2& vec) const {
	return mapDatas_[static_cast<int32_t>(vec.y)][static_cast<int32_t>(vec.x)];
}

Vector3 Mapchip::HitMap(const Vector3& beforePos, const Vector3& afterPos, float ) const {
	const Vector3 mapRadius = Vector3{ width_ / 2.f, 0.f,0.f } *scale_;
	//const Vector3 mapRadius = Vector3{ (width_ / 2.0f) * scale_, 0.0f * scale_ , 0.0f };

	Vector3 resultPos;
	resultPos = ((afterPos + mapRadius) * kChipSize) / scale_;

	Vector2 leftTop;
	Vector2 rightTop;

	Vector2 leftDown;
	Vector2 rightDown;

	leftTop.x = (resultPos.x - kChipSize / 2.f) / kChipSize;
	leftTop.y = (resultPos.y + kChipSize / 2.f - 1) / kChipSize;
	rightTop.x = (resultPos.x + kChipSize / 2.f - 1) / kChipSize;
	rightTop.y = (resultPos.y + kChipSize / 2.f - 1) / kChipSize;
	leftDown.x = (resultPos.x - kChipSize / 2.f) / kChipSize;
	leftDown.y = (resultPos.y - kChipSize / 2.f) / kChipSize;
	rightDown.x = (resultPos.x + kChipSize / 2.f - 1) / kChipSize;
	rightDown.y = (resultPos.y - kChipSize / 2.f) / kChipSize;

	if (
		(leftDown.x < 0.f || rightDown.x >= float(width_))		// 左右が画面外
		|| (leftTop.y >= float(height_) || leftDown.y < 0.f)	// 上下が画面外
		) {
		return afterPos;
	}

	const float kExtension = 7.f;

	///
	///	↓ マップチップとの当たり判定
#pragma region MapChipHit
	if (afterPos.x - beforePos.x < 0 && afterPos.y - beforePos.y < 0) { //	SA
		//				Novice::ScreenPrintf(10, 90, "SA");
		if ((mapDatas_[(int)(rightDown.y)][(int)(rightDown.x)] != ChipState::kNone) && (mapDatas_[(int)(leftTop.y)][(int)(leftTop.x)] != ChipState::kNone)) { //	右元と左頭が移動不可の場合
			resultPos.x = (int)(leftTop.x + 1) * kChipSize + 16.f;
			resultPos.y = (int)(rightDown.y + 1) * kChipSize + 16.f;

			// player.box.velocity.x *= 0.8;
			// player.box.velocity.y = 0;
			// player.isGround = true;
		}
		else if (mapDatas_[(int)(rightDown.y)][(int)(rightDown.x)] != ChipState::kNone) { //	右元が移動不可の場合

			if (mapDatas_[(int)(rightDown.y) + 1][(int)(rightDown.x)] == ChipState::kNone) {	//	尚且つ右元の上マスが移動可能の場合
				resultPos.y = (int)(rightDown.y + 1) * kChipSize + 16.f;

				// player.box.velocity.y = 0;
				// player.isGround = true;
			}
		}
		else if (mapDatas_[(int)(leftTop.y)][(int)(leftTop.x)] != ChipState::kNone) { //	左頭が移動不可の場合

			if (mapDatas_[(int)(leftTop.y)][(int)(leftTop.x) + 1] == ChipState::kNone) {  //	尚且つ左頭の右マスが開いている場合。
				resultPos.x = (int)(leftTop.x + 1) * kChipSize + 16.f;

				// player.box.velocity.x *= 0.8;
			}
		}


		else if (mapDatas_[(int)(leftDown.y)][(int)(leftDown.x)] != ChipState::kNone) { //	左元が移動不可の場合


			//		尚且つ右と上が開いている場合
			if (mapDatas_[(int)(leftDown.y)][(int)(leftDown.x) + 1] == ChipState::kNone && mapDatas_[(int)(leftDown.y) + 1][(int)(leftDown.x)] == ChipState::kNone) {

				//if (mapDatas_[(int)(leftTop.y)][(int)(leftTop.x - 1)] == ChipState::air) {
				if ((((int)(resultPos.x + 15.f) % kChipSize) < ((int)(resultPos.y + 15.f) % kChipSize))) {
					resultPos.y = (int)(leftDown.y + 1) * kChipSize + 16.f;

					// player.box.velocity.y = 0;
					// player.isGround = true;
				}
				//	}
				//	else if(mapDatas_[(int)(leftTop.y - 1)][(int)(leftTop.x)] == ChipState::air) {
				else {
					resultPos.x = (int)(leftDown.x + 1) * kChipSize + 16.f;

					// player.box.velocity.x *= 0.8;
				}
				//	}
			}
			//		尚且つ左元の上マスが開いていて、右マスが移動不可の場合。
			else if (mapDatas_[(int)(leftDown.y) + 1][(int)(leftDown.x)] == ChipState::kNone && mapDatas_[(int)(leftDown.y)][(int)(leftDown.x) + 1] != ChipState::kNone) {
				resultPos.y = (int)(leftDown.y + 1) * kChipSize + 16.f;

				// player.box.velocity.y = 0;
				// player.isGround = true;
			}
			//		尚且つ左元の右マスが開いていて、上マスが移動不可の場合。
			else if (mapDatas_[(int)(leftDown.y)][(int)(leftDown.x) + 1] == ChipState::kNone && mapDatas_[(int)(leftDown.y) + 1][(int)(leftDown.x)] != ChipState::kNone) {
				resultPos.x = (int)(leftDown.x + 1) * kChipSize + 16.f;

				// player.box.velocity.x *= 0.8;
			}
			//		尚且つ左元の右マスと上マスが移動不可の場合。
			else if (mapDatas_[(int)(leftDown.y)][(int)(leftDown.x) + 1] != ChipState::kNone && mapDatas_[(int)(leftDown.y) + 1][(int)(leftDown.x)] != ChipState::kNone) {
				resultPos.x = (int)(leftDown.x + 1) * kChipSize + 16.f;
				resultPos.y = (int)(leftDown.y + 1) * kChipSize + 16.f;

				// player.box.velocity.x *= 0.8;
				// player.box.velocity.y = 0;
				// player.isGround = true;
			}
		}
	}


	else if (afterPos.x - beforePos.x < 0 && afterPos.y - beforePos.y > 0) { //	WA
		//				Novice::ScreenPrintf(10, 90, "WA");
		if ((mapDatas_[(int)(rightTop.y)][(int)(rightTop.x)] != ChipState::kNone) && (mapDatas_[(int)(leftDown.y)][(int)(leftDown.x)] != ChipState::kNone)) { //	右頭と左元が移動不可の場合
			resultPos.x = (int)(leftDown.x + 1) * kChipSize + 16.f;
			resultPos.y = (int)(rightTop.y - 1) * kChipSize + 16.f;

			// player.box.velocity.x *= 0.8;
			// player.box.velocity.y = 0;
		}
		else if (mapDatas_[(int)(rightTop.y)][(int)(rightTop.x)] != ChipState::kNone) { //	右頭が移動不可の場合

			if (mapDatas_[(int)(rightTop.y)][(int)(rightTop.x - (kExtension / kChipSize))] != ChipState::kNone) { //	右頭の3ピクセルが移動不可の場合
				if (mapDatas_[(int)(rightTop.y) - 1][(int)(rightTop.x)] == ChipState::kNone) {	//	尚且つ右頭の下マスが移動可能の場合
					resultPos.y = (int)(rightTop.y - 1) * kChipSize + 16.f;

					// player.box.velocity.y = 0;
				}
			}
			else {
				resultPos.x = (int)(rightTop.x - 1) * kChipSize + 16.f;
			}
		}
		else if (mapDatas_[(int)(leftDown.y)][(int)(leftDown.x)] != ChipState::kNone) { //	左元が移動不可の場合

			if (mapDatas_[(int)(leftDown.y)][(int)(leftDown.x) + 1] == ChipState::kNone) {  //	尚且つ左元の右マスが開いている場合。
				resultPos.x = (int)(leftDown.x + 1) * kChipSize + 16.f;

				// player.box.velocity.x *= 0.8;
			}
		}


		else if (mapDatas_[(int)(leftTop.y)][(int)(leftTop.x)] != ChipState::kNone) { //	左頭が移動不可の場合


			//		尚且つ右と下が開いている場合
			if (mapDatas_[(int)(leftTop.y)][(int)(leftTop.x) + 1] == ChipState::kNone && mapDatas_[(int)(leftTop.y) - 1][(int)(leftTop.x)] == ChipState::kNone) {

				//if (mapDatas_[(int)(leftTop.y)][(int)(leftTop.x - 1)] == ChipState::air) {
				if (((int)(resultPos.x + 15.f) % kChipSize) <= ((kChipSize)-((int)(resultPos.y + 17.f) % kChipSize)) % kChipSize) {
					resultPos.y = (int)(leftTop.y - 1) * kChipSize + 16.f;

					// player.box.velocity.y = 0;
				}
				//	}
				//	else if(mapDatas_[(int)(leftTop.y - 1)][(int)(leftTop.x)] == ChipState::air) {
				else {
					resultPos.x = (int)(leftTop.x + 1) * kChipSize + 16.f;

					// player.box.velocity.x *= 0.8;
				}
				//	}
			}
			//		尚且つ左頭の下マスが開いていて、右マスが移動不可の場合。
			else if (mapDatas_[(int)(leftTop.y) - 1][(int)(leftTop.x)] == ChipState::kNone && mapDatas_[(int)(leftTop.y)][(int)(leftTop.x) + 1] != ChipState::kNone) {
				resultPos.y = (int)(leftTop.y - 1) * kChipSize + 16.f;

				// player.box.velocity.y = 0;
			}
			//		尚且つ左頭の右マスが開いていて、下マスが移動不可の場合。
			else if (mapDatas_[(int)(leftTop.y)][(int)(leftTop.x) + 1] == ChipState::kNone && mapDatas_[(int)(leftTop.y) - 1][(int)(leftTop.x)] != ChipState::kNone) {
				resultPos.x = (int)(leftTop.x + 1) * kChipSize + 16.f;

				// player.box.velocity.x *= 0.8;
			}
			//		尚且つ左頭の右マスと下マスが移動不可の場合。
			else if (mapDatas_[(int)(leftTop.y)][(int)(leftTop.x) + 1] != ChipState::kNone && mapDatas_[(int)(leftTop.y) - 1][(int)(leftTop.x)] != ChipState::kNone) {
				resultPos.x = (int)(leftTop.x + 1) * kChipSize + 16.f;
				resultPos.y = (int)(leftTop.y - 1) * kChipSize + 16.f;

				// player.box.velocity.x *= 0.8;
				// player.box.velocity.y = 0;
			}
		}
	}

	else if (afterPos.x - beforePos.x > 0 && afterPos.y - beforePos.y < 0) { //	SD
		//				Novice::ScreenPrintf(10, 90, "SD");
		if ((mapDatas_[(int)(leftDown.y)][(int)(leftDown.x)] != ChipState::kNone) && (mapDatas_[(int)(rightTop.y)][(int)(rightTop.x)] != ChipState::kNone)) { //	左元と右頭が移動不可の場合
			resultPos.x = (int)(rightTop.x - 1) * kChipSize + 16.f;
			resultPos.y = (int)(leftDown.y + 1) * kChipSize + 16.f;

			// player.box.velocity.x *= 0.8;
			// player.box.velocity.y = 0;
			// player.isGround = true;
		}
		else if (mapDatas_[(int)(leftDown.y)][(int)(leftDown.x)] != ChipState::kNone) { //	左元が移動不可の場合

			if (mapDatas_[(int)(leftDown.y) + 1][(int)(leftDown.x)] == ChipState::kNone) {	//	尚且つ左元の上マスが移動可能の場合
				resultPos.y = (int)(leftDown.y + 1) * kChipSize + 16.f;

				// player.box.velocity.y = 0;
				// player.isGround = true;
			}
		}
		else if (mapDatas_[(int)(rightTop.y)][(int)(rightTop.x)] != ChipState::kNone) { //	右頭が移動不可の場合

			if (mapDatas_[(int)(rightTop.y)][(int)(rightTop.x) - 1] == ChipState::kNone) {  //	尚且つ右頭の左マスが開いている場合。
				resultPos.x = (int)(rightTop.x - 1) * kChipSize + 16.f;

				// player.box.velocity.x *= 0.8;
			}
		}


		else if (mapDatas_[(int)(rightDown.y)][(int)(rightDown.x)] != ChipState::kNone) { //	右元が移動不可の場合


			//		尚且つ左と上が開いている場合
			if (mapDatas_[(int)(rightDown.y)][(int)(rightDown.x) - 1] == ChipState::kNone && mapDatas_[(int)(rightDown.y) + 1][(int)(rightDown.x)] == ChipState::kNone) {

				//if (mapDatas_[(int)(rightTop.y)][(int)(rightTop.x - 1)] == ChipState::air) {
				if (((kChipSize)-((int)(resultPos.x + 17.f) % kChipSize)) % kChipSize < ((int)(resultPos.y + 15.f) % kChipSize)) {
					resultPos.y = (int)(rightDown.y + 1) * kChipSize + 16.f;

					// player.box.velocity.y = 0;
					// player.isGround = true;
				}
				//	}
				//	else if(mapDatas_[(int)(rightTop.y - 1)][(int)(rightTop.x)] == ChipState::air) {
				else {
					resultPos.x = (int)(rightDown.x - 1) * kChipSize + 16.f;
				}
				//	}
			}
			//		尚且つ右元の上マスが開いていて、左マスが移動不可の場合。
			else if (mapDatas_[(int)(rightDown.y) + 1][(int)(rightDown.x)] == ChipState::kNone && mapDatas_[(int)(rightDown.y)][(int)(rightDown.x) - 1] != ChipState::kNone) {
				resultPos.y = (int)(rightDown.y + 1) * kChipSize + 16.f;

				// player.box.velocity.y = 0;
				// player.isGround = true;
			}
			//		尚且つ右元の左マスが開いていて、上マスが移動不可の場合。
			else if (mapDatas_[(int)(rightDown.y)][(int)(rightDown.x) - 1] == ChipState::kNone && mapDatas_[(int)(rightDown.y) + 1][(int)(rightDown.x)] != ChipState::kNone) {
				resultPos.x = (int)(rightDown.x - 1) * kChipSize + 16.f;

				// player.box.velocity.x *= 0.8;
			}
			//		尚且つ右元の左マスと上マスが移動不可の場合。
			else if (mapDatas_[(int)(rightDown.y)][(int)(rightDown.x) - 1] != ChipState::kNone && mapDatas_[(int)(rightDown.y) + 1][(int)(rightDown.x)] != ChipState::kNone) {
				resultPos.x = (int)(rightDown.x - 1) * kChipSize + 16.f;
				resultPos.y = (int)(rightDown.y + 1) * kChipSize + 16.f;

				// player.box.velocity.x *= 0.8;
				// player.box.velocity.y = 0;
				// player.isGround = true;
			}
		}
	}


	else if (afterPos.x - beforePos.x > 0 && afterPos.y - beforePos.y > 0) { //	WD
		//				Novice::ScreenPrintf(10, 90, "WD");
		if ((mapDatas_[(int)(leftTop.y)][(int)(leftTop.x)] != ChipState::kNone) && (mapDatas_[(int)(rightDown.y)][(int)(rightDown.x)] != ChipState::kNone)) { //	左頭と右元が移動不可の場合
			resultPos.x = (int)(rightDown.x - 1) * kChipSize + 16.f;
			resultPos.y = (int)(leftTop.y - 1) * kChipSize + 16.f;

			// player.box.velocity.x *= 0.8;
			// player.box.velocity.y = 0;
		}
		else if (mapDatas_[(int)(leftTop.y)][(int)(leftTop.x)] != ChipState::kNone) { //	左頭が移動不可の場合

			if (mapDatas_[(int)(leftTop.y)][(int)(leftTop.x + (kExtension / kChipSize))] != ChipState::kNone) { //	左頭の3ピクセルが移動不可の場合
				if (mapDatas_[(int)(leftTop.y) - 1][(int)(leftTop.x)] == ChipState::kNone) {	//	尚且つ左頭の下マスが移動可能の場合
					resultPos.y = (int)(leftTop.y - 1) * kChipSize + 16.f;

					// player.box.velocity.y = 0;
				}
			}
			else {
				resultPos.x = (int)(leftTop.x + 1) * kChipSize + 16.f;
			}
		}
		else if (mapDatas_[(int)(rightDown.y)][(int)(rightDown.x)] != ChipState::kNone) { //	右元が移動不可の場合

			if (mapDatas_[(int)(rightDown.y)][(int)(rightDown.x) - 1] == ChipState::kNone) {  //	尚且つ右元の左マスが開いている場合。
				resultPos.x = (int)(rightDown.x - 1) * kChipSize + 16.f;

				// player.box.velocity.x *= 0.8;
			}
		}


		else if (mapDatas_[(int)(rightTop.y)][(int)(rightTop.x)] != ChipState::kNone) { //	右頭が移動不可の場合


			//		尚且つ左と下が開いている場合
			if (mapDatas_[(int)(rightTop.y)][(int)(rightTop.x) - 1] == ChipState::kNone && mapDatas_[(int)(rightTop.y) - 1][(int)(rightTop.x)] == ChipState::kNone) {

				//if (mapDatas_[(int)(rightTop.y)][(int)(rightTop.x - 1)] == ChipState::air) {
				if (((kChipSize)-((int)(resultPos.x + 17.f) % kChipSize)) % kChipSize <= ((kChipSize)-((int)(resultPos.y + 17.f) % kChipSize)) % kChipSize) {
					resultPos.y = (int)(rightTop.y - 1) * kChipSize + 16.f;

					// player.box.velocity.y = 0;
				}
				//	}
				//	else if(mapDatas_[(int)(rightTop.y - 1)][(int)(rightTop.x)] == ChipState::air) {
				else {
					resultPos.x = (int)(rightTop.x - 1) * kChipSize + 16.f;

					// player.box.velocity.x *= 0.8;
				}
				//	}
			}
			//		尚且つ右頭の下マスが開いていて、左マスが移動不可の場合。
			else if (mapDatas_[(int)(rightTop.y) - 1][(int)(rightTop.x)] == ChipState::kNone && mapDatas_[(int)(rightTop.y)][(int)(rightTop.x) - 1] != ChipState::kNone) {
				resultPos.y = (int)(rightTop.y - 1) * kChipSize + 16.f;

				// player.box.velocity.y = 0;
			}
			//		尚且つ右頭の左マスが開いていて、下マスが移動不可の場合。
			else if (mapDatas_[(int)(rightTop.y)][(int)(rightTop.x) - 1] == ChipState::kNone && mapDatas_[(int)(rightTop.y) - 1][(int)(rightTop.x)] != ChipState::kNone) {
				resultPos.x = (int)(rightTop.x - 1) * kChipSize + 16.f;

				// player.box.velocity.x *= 0.8;
			}
			//		尚且つ右頭の左マスと下マスが移動不可の場合。
			else if (mapDatas_[(int)(rightTop.y)][(int)(rightTop.x) - 1] != ChipState::kNone && mapDatas_[(int)(rightTop.y) - 1][(int)(rightTop.x)] != ChipState::kNone) {
				resultPos.x = (int)(rightTop.x - 1) * kChipSize + 16.f;
				resultPos.y = (int)(rightTop.y - 1) * kChipSize + 16.f;

				// player.box.velocity.x *= 0.8;
				// player.box.velocity.y = 0;
			}
		}
	}

	else if (afterPos.x - beforePos.x > 0) {
		if ((mapDatas_[(int)(rightTop.y)][(int)(rightTop.x)] != ChipState::kNone) || (mapDatas_[(int)(rightDown.y)][(int)(rightDown.x)] != ChipState::kNone)) {
			resultPos.x = (int)(rightTop.x - 1) * kChipSize + 16.f;
		}
	}

	else if (afterPos.x - beforePos.x < 0) {
		if ((mapDatas_[(int)(leftTop.y)][(int)(leftTop.x)] != ChipState::kNone) || (mapDatas_[(int)(leftDown.y)][(int)(leftDown.x)] != ChipState::kNone)) {
			resultPos.x = (int)(leftTop.x + 1) * kChipSize + 16.f;
		}
	}

	else if (afterPos.y - beforePos.y > 0) {
		if ((mapDatas_[(int)(leftTop.y)][(int)(leftTop.x)] != ChipState::kNone) && mapDatas_[(int)(leftTop.y)][(int)(leftTop.x + (kExtension / kChipSize))] == ChipState::kNone) { //	左頭の3ピクセルが移動可の場合
			resultPos.x = (int)(leftTop.x + 1) * kChipSize + 16.f;
		}
		else if ((mapDatas_[(int)(rightTop.y)][(int)(rightTop.x)] != ChipState::kNone) && mapDatas_[(int)(rightTop.y)][(int)(rightTop.x - (kExtension / kChipSize))] == ChipState::kNone) { //	左頭の3ピクセルが移動可の場合
			resultPos.x = (int)(rightTop.x - 1) * kChipSize + 16.f;
		}

		else if ((mapDatas_[(int)(leftTop.y)][(int)(leftTop.x)] != ChipState::kNone) || (mapDatas_[(int)(rightTop.y)][(int)(rightTop.x)] != ChipState::kNone)) {
			resultPos.y = (int)(rightTop.y - 1) * kChipSize + 16.f;

			// player.box.velocity.y = 0;
		}
	}

	else if (afterPos.y - beforePos.y < 0) {
		if ((mapDatas_[(int)(leftDown.y)][(int)(leftDown.x)] != ChipState::kNone) || (mapDatas_[(int)(rightDown.y)][(int)(rightDown.x)] != ChipState::kNone)) {
			resultPos.y = (int)(leftDown.y + 1) * kChipSize + 16.f;

			// player.box.velocity.y = 0;

			// player.isGround = true;

		}
	}
	else {
		return afterPos;
	}

#pragma endregion
	///	↑ マップチップとの当たり判定
	/// 

	return (resultPos * scale_) / kChipSize - mapRadius;
}

void Mapchip::ChipData::Init() {
	std::string modelName;

	switch (chipState_) {
	case Mapchip::ChipState::kNone:
		modelName = "";
		break;
	case Mapchip::ChipState::kBlock:
		modelName = "cube";
		break;
	default:
		modelName = "";
		break;
	}
	if (modelName == "") {
		model_ = nullptr;
	}
	else {
		if (!transform_) {
			transform_ = std::make_unique<WorldTransform>();
			transform_->Initialize();
		}
		model_.reset(Model::CreateFlomObj(modelName));
		//sprite_->SetSize({ mapchipSize_, mapchipSize_ });
	}

}

void Mapchip::ChipData::Exit() {
	this->model_.release();
	this->chipState_ = ChipState::kNone;
	//this->transform_.reset();

}

void Mapchip::ChipData::Create(uint32_t x, uint32_t y) {
	Init();

	const float xCenter = width_ / 2.0f - 0.5f;
	if (transform_) {
		transform_->translation_ = { (x - xCenter) * 2.0f, static_cast<float>(y) * 2.0f + 1.f , 0.0f };
		transform_->UpdateMatrix();
	}
}

void Mapchip::ChipData::Draw(const ViewProjection &viewProjection) const {
	if (model_ == nullptr) {
		return;
	}
	if (transform_) {
		model_->Draw(*transform_.get(), viewProjection);
	}

}

Mapchip::ChipData& Mapchip::ChipData::operator=(ChipState state) {
	chipState_ = state;
	Init();
	return *this;
}
