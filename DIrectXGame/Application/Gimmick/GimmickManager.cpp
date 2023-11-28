#include "GimmickManager.h"
#include "WallGimmick.h"

GimmickManager::GimmickManager()
{
	// モデル初期化
	floarModel_.reset(Model::CreateFlomObj("cube"));
}

void GimmickManager::Initialize()
{
	const float xCenter = 72 / 2.0f - 0.5f;

	Vector3 tmpPosition = { (10.0f - xCenter) * 2.0f, 4.0f * 2.0f + 1.0f, 0.0f };
	uint32_t generateNum = 5;

	AddWallGimmick(tmpPosition, generateNum);

	tmpPosition = { (40.0f - xCenter) * 2.0f, 13.0f * 2.0f + 1.0f, 0.0f };
	generateNum = 2;
	AddWallGimmick(tmpPosition, generateNum);

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

void GimmickManager::AddWallGimmick(const Vector3& position, uint32_t number)
{
	WallGimmick* newObject = new WallGimmick();
	newObject->Initialize(floarModel_.get());
	newObject->SetPosition(position);
	float endOffSet = 10.0f;
	Vector3 easeBegin = position;
	Vector3 easeEnd = R_Math::Subtract(easeBegin, Vector3{ endOffSet,0,0 });
	newObject->SetEasePoint(easeBegin, easeEnd);
	gimmicks_.push_back(newObject);

	float offSet = -2.0f;
	for (int i = 0; i < static_cast<int>(number); ++i) {
		AddWallChild(Vector3{ (i + 1) * offSet,0,0 }, newObject);
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
