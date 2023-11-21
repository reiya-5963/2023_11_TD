#include "GimmickManager.h"
#include "WallGimmick.h"

GimmickManager::GimmickManager()
{
	// モデル初期化
	floarModel_.reset(Model::CreateFlomObj("cube"));
}

void GimmickManager::Initialize()
{

	Vector3 tmpPosition = { -10.0f,10.0f,0.0f };
	AddWallGimmick(tmpPosition, nullptr);
	//tmpPosition = { 1.0f,0.0f,0.0f };
	//AddWallGimmick(tmpPosition);

	tmpPosition = { 1.0f,20.0f,0.0f };
	AddWallGimmick(tmpPosition, nullptr);

}

void GimmickManager::Update()
{

	for (IGimmick* gimmick : gimmicks_) {
		gimmick->Update();
	}

	//gimmicks_.

}

void GimmickManager::Draw(const ViewProjection& viewProjection)
{

	for (IGimmick* gimmick : gimmicks_) {
		gimmick->Draw(viewProjection);
	}

}

void GimmickManager::AddWallGimmick(const Vector3& position, WorldTransform* parent)
{
	WallGimmick* newObject = new WallGimmick();
	newObject->Initialize(floarModel_.get());
	newObject->SetPosition(position);
	Vector3 easeBegin = position;
	Vector3 easeEnd = R_Math::Subtract(easeBegin, Vector3{ 10.0f,0,0 });
	newObject->SetEasePoint(easeBegin, easeEnd);
	gimmicks_.push_back(newObject);
	parent;
	WallGimmick* child = new WallGimmick();
	child->Initialize(floarModel_.get());
	child->SetPosition(Vector3{ 4.0f,0,0 });
	easeBegin = Vector3{ 4.0f,0,0 };
	easeEnd = R_Math::Subtract(easeBegin, Vector3{ 10.0f,0,0 });
	child->SetEasePoint(easeBegin, easeEnd);
	child->SetParentChildren(newObject->GetWorldTransform());
	gimmicks_.push_back(child);

}
