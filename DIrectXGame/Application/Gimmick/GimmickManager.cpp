#include "GimmickManager.h"
#include "WallGimmick.h"

GimmickManager::GimmickManager()
{
	// モデル初期化
	floarModel_.reset(Model::CreateFlomObj("cube"));
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
	EasePointInfo test = {};
	float value = 0.1f;
	test.startSquares = { 37.0f,1.0f, 0};
	test.endSquares = { 37.0f,7.0f, 0};
	test = GenerateEaseStartToEnd(Vector3{ 37.0f,1.0f,0 }, Vector3{ 37.0f,7.0f,0 }, value);
	// ローカル変数
	uint32_t generateNum = 3;
	ParentBlock info;

	// 上下
	info.direction_ = kRight;
	info.number_ = generateNum;
	AddWallGimmick(test.startSquares, test.endSquares, info);

	// 左右
	info.number_ = 5;
	test = GenerateEaseStartToEnd(Vector3{ 6.0f,15.0f,0 }, Vector3{ 12.0f,15.0f,0 }, value);
	AddWallGimmick(test.startSquares, test.endSquares, info);

	// 上下
	info.direction_ = kTop;
	info.number_ = 5;
	test = GenerateEaseStartToEnd(Vector3{ 12.0f,1.0f,0 }, Vector3{ 12.0f,7.0f,0 }, value);
	AddWallGimmick(test.startSquares, test.endSquares, info);


	info.direction_ = kRight;
	info.number_ = 3;
	test = GenerateEaseStartToEnd(Vector3{ 29.0f,16.0f,0 }, Vector3{ 29.0f,22.0f,0 }, value);
	AddWallGimmick(test.startSquares, test.endSquares, info);

	info.direction_ = kTop;
	info.number_ = 6;
	test = GenerateEaseStartToEnd(Vector3{ 40.0f,22.0f,0 }, Vector3{ 40.0f,30.0f,0 }, value);
	AddWallGimmick(test.startSquares, test.endSquares, info);

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
		gimmick->Draw(viewProjection);
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
