#include "GimmickManager.h"
#include "WallGimmick.h"

GimmickManager::GimmickManager()
{
	// モデル初期化
	floarModel_.reset(Model::CreateFlomObj("cube"));
}

void GimmickManager::Initialize()
{
	// ローカル変数
	Vector3 tmpPosition = { -10.0f,0.0f,0.0f };
	uint32_t generateNum = 5;
	Vector3 end = R_Math::Add(tmpPosition, Vector3{ 0,10.0f,0 });
	ParentBlock info;

	info.direction_ = kBottom;
	info.number_ = generateNum;
	AddWallGimmick(tmpPosition, end, info);

	tmpPosition = { 1.0f,20.0f,0.0f };
	generateNum = 2;
	info.number_ = generateNum;
	end = R_Math::Add(tmpPosition, Vector3{ 0,10.0f,0 });
	AddWallGimmick(tmpPosition, end, info);

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
