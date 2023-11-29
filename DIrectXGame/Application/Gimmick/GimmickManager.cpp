#include "GimmickManager.h"
#include "WallGimmick.h"

GimmickManager::GimmickManager()
{
	// モデル初期化
	floarModel_.reset(Model::CreateFlomObj("cube"));
	blockTexture_ = TextureManager::Load("cube/MoveBlock.png");
}

GimmickManager::EasePointInfo GimmickManager::GenerateEaseStartToEnd(const Vector3& start, const Vector3& end, float value)
{
	EasePointInfo result = {};
	const float xCenter = 48 / 2.0f - 0.5f;
	float valueY = value;
	Vector3 startPoint = { ((start.x - 1.0f) - xCenter) * 2.0f, (start.y * 2.0f + 1.0f) - valueY, 0.0f };
	Vector3 endPoint = { ((end.x - 1.0f) - xCenter) * 2.0f,(end.y * 2.0f + 1.0f) + valueY, 0.0f };
	result.startSquares = startPoint;
	result.endSquares = endPoint;
	return result;
}

void GimmickManager::Initialize()
{
	EasePointInfo Start2End = {};
	float value = 0.0f;
	// ローカル変数
	uint32_t generateNum = 3;
	ParentBlock parentInfo;

	this->gimmicks_.clear();

#pragma region ギミック
	// 3_1
	parentInfo.direction_ = kTop;
	parentInfo.number_ = 5;
	Start2End = GenerateEaseStartToEnd(Vector3{ 12.0f,1.0f,0 }, Vector3{ 12.0f,7.0f,0 }, value);
	AddWallGimmick(Start2End.startSquares, Start2End.endSquares, parentInfo);
	// 3_2
	parentInfo.direction_ = kRight;
	parentInfo.number_ = 3;
	Start2End = GenerateEaseStartToEnd(Vector3{ 29.0f,7.0f,0 }, Vector3{ 29.0f,1.0f,0 }, value);
	AddWallGimmick(Start2End.startSquares, Start2End.endSquares, parentInfo);
	// 3_3
	parentInfo.direction_ = kRight;
	parentInfo.number_ = generateNum;
	Start2End = GenerateEaseStartToEnd(Vector3{ 37.0f,1.0f,0 }, Vector3{ 37.0f,7.0f,0 }, value);
	AddWallGimmick(Start2End.startSquares, Start2End.endSquares, parentInfo);
	// 3_4
	parentInfo.direction_ = kRight;
	parentInfo.number_ = 5;
	Start2End = GenerateEaseStartToEnd(Vector3{ 6.0f,15.0f,0 }, Vector3{ 12.0f,15.0f,0 }, value);
	AddWallGimmick(Start2End.startSquares, Start2End.endSquares, parentInfo);
	// 3_5
	parentInfo.direction_ = kRight;
	parentInfo.number_ = 3;
	Start2End = GenerateEaseStartToEnd(Vector3{ 29.0f,16.0f,0 }, Vector3{ 29.0f,22.0f,0 }, value);
	AddWallGimmick(Start2End.startSquares, Start2End.endSquares, parentInfo);
	// 3_6
	parentInfo.direction_ = kTop;
	parentInfo.number_ = 5;
	Start2End = GenerateEaseStartToEnd(Vector3{ 40.0f,23.0f,0 }, Vector3{ 40.0f,30.0f,0 }, value);
	AddWallGimmick(Start2End.startSquares, Start2End.endSquares, parentInfo);
	// 3_7
	parentInfo.direction_ = kLeft;
	parentInfo.number_ = 3;
	Start2End = GenerateEaseStartToEnd(Vector3{ 24.0f,22.0f,0 }, Vector3{ 24.0f,16.0f,0 }, value);
	AddWallGimmick(Start2End.startSquares, Start2End.endSquares, parentInfo);
#pragma endregion

}

void GimmickManager::Update()
{

	for (IGimmick* gimmick : gimmicks_) {
		gimmick->Update();
	}

}

void GimmickManager::Draw(const ViewProjection& viewProjection)
{

	for (IGimmick* gimmick : gimmicks_) {
		gimmick->Draw(viewProjection, blockTexture_);
	}

}

void GimmickManager::Reset()
{
	for (IGimmick* gimmick : gimmicks_) {
		gimmick->Reset();
	}
}

void GimmickManager::AddWallGimmick(const Vector3& position, const Vector3& endPoint, ParentBlock info)
{
	WallGimmick* newObject = new WallGimmick();
	newObject->Initialize(floarModel_.get());
	newObject->SetPosition(position);
	newObject->SetEasePoint(position, endPoint);
	gimmicks_.push_back(newObject);

	// 子の設定
	if (info.number_ <= 0) {
		return;
	}
	float offSet = 2.0f;
	switch (info.direction_)
	{
	case kLeft:
		for (int i = 0; i < static_cast<int>(info.number_); ++i) {
			AddWallChild(Vector3{ (i + 1) * (-offSet),0,0 }, newObject);
		}
		break;
	case kRight:
		for (int i = 0; i < static_cast<int>(info.number_); ++i) {
			AddWallChild(Vector3{ (i + 1) * offSet,0,0 }, newObject);
		}
		break;
	case kTop:
		for (int i = 0; i < static_cast<int>(info.number_); ++i) {
			AddWallChild(Vector3{ 0,(i + 1) * offSet,0 }, newObject);
		}
		break;
	case kBottom:
		for (int i = 0; i < static_cast<int>(info.number_); ++i) {
			AddWallChild(Vector3{ 0,(i + 1) * (-offSet),0 }, newObject);
		}
		break;
	}

}

void GimmickManager::AddWallChild(const Vector3& position, WallGimmick* parent)
{
	WallGimmick* child = new WallGimmick();
	child->Initialize(floarModel_.get());
	child->SetPosition(position);
	child->SetParentChildren(parent->GetWorldTransform());
	gimmicks_.push_back(child);
}
