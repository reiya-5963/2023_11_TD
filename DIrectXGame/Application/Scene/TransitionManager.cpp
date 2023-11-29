#include "TransitionManager.h"
#include "Input.h"
#include "imgui.h"

TransitionManager::TransitionManager()
{
	dxCommon_ = DirectXCommon::GetInstance();
	audio_ = Audio::GetInstance();

	uint32_t texture = TextureManager::Load("white1x1.png");
	whiteSprite_.reset(Sprite::Create(texture, { 0,0 }, 0, { 0,0,0,1 }, { 0.5f,0.5f }));
	blackOut_.endFrame_ = 100;
	blackOut_.spriteInfo_.position_ = { (float)WinApp::kWindowWidth / 2,(float)WinApp::kWindowHeight / 2 };
	blackOut_.spriteInfo_.color_ = { 0,0,0,1.0f };
	blackOut_.spriteInfo_.size_ = { (float)WinApp::kWindowWidth,(float)WinApp::kWindowHeight };

	whiteSprite_->SetPosition(blackOut_.spriteInfo_.position_);
	whiteSprite_->SetSize(blackOut_.spriteInfo_.size_);
	whiteSprite_->SetTextureRect({ 0,0 }, { blackOut_.spriteInfo_.size_.x,blackOut_.spriteInfo_.size_.y });
	//whiteSprite_->SetColor(blackOut_.spriteInfo_.color_);
}

TransitionManager* TransitionManager::GetInstance()
{
	static TransitionManager instance;
	return &instance;
}

void TransitionManager::Update()
{
	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		TransitionSetting();
	}
#ifdef _DEBUG
	ImGui::Begin("transi");
	ImGui::DragFloat4("color", &blackOut_.spriteInfo_.color_.x);
	ImGui::End();
#endif // _DEBUG


	if (blackOut_.isNow_) {
		BlackOutProcess();
	}
}

void TransitionManager::Draw()
{
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Sprite::PreDraw(commandList);
	////この間に背景スプライトの描画を入れる

	if (blackOut_.isNow_) {
		whiteSprite_->Draw();
	}

	Sprite::PostDraw();

}

void TransitionManager::TransitionSetting()
{

	blackOut_.ease_t_ = 0;
	blackOut_.isNow_ = true;
	//blackOut_.alphaValue_=

	blackOut_.spriteInfo_.position_ = { (float)WinApp::kWindowWidth / 2,(float)WinApp::kWindowHeight / 2 };
	blackOut_.spriteInfo_.color_.w = 1.0f;
	blackOut_.spriteInfo_.size_ = { (float)WinApp::kWindowWidth,(float)WinApp::kWindowHeight };

	whiteSprite_->SetPosition(blackOut_.spriteInfo_.position_);
	whiteSprite_->SetSize(blackOut_.spriteInfo_.size_);
	whiteSprite_->SetColor(blackOut_.spriteInfo_.color_);
}

void TransitionManager::BlackOutProcess()
{

	blackOut_.ease_t_ += (1.0f / (float)blackOut_.endFrame_);
	if (blackOut_.ease_t_ >= 1.0f) {
		blackOut_.ease_t_ = 1.0f;
		blackOut_.isNow_ = false;
	}
	blackOut_.spriteInfo_.color_.w = R_Math::EaseInQuadF(blackOut_.ease_t_, 1.0f, 0.0f);
	//blackOut_.spriteInfo_.color_.w -= (1.0f / (float)blackOut_.endFrame_);
	whiteSprite_->SetColor(blackOut_.spriteInfo_.color_);
}
