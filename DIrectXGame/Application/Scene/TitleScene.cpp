#include "TitleScene.h"
#include "SceneManager.h"
#include "audio.h"
#include "imgui.h"
#include <cmath>
#include <numbers>

TitleScene::TitleScene() {
}

TitleScene::~TitleScene() {
	//audioManager_->StopAudio(AudioManager::GetSoundList(AudioManager::kAllBGM));
}

void TitleScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	sceneManager_ = SceneManager::GetInstance();
	transition_ = TransitionManager::GetInstance();
	audioManager_ = new AudioManager();
	audioManager_->PlayBGMAudio(AudioManager::GetSoundList(AudioManager::kAllBGM), 0.1f);

	Vector4 color = { 1,1,1,1 };
	Vector2 center = { (float)WinApp::kWindowWidth / 2,(float)WinApp::kWindowHeight / 2 };
	Vector2 anchor = { 0.5f,0.5f };

	ghostPosition_ = center;
	ghostPosition_.x = 622.0f;
	ghostPosition_.y = 490.0f;

	textPosition_ = center;
	textPosition_.y = 190.0f;

	AButtonPosition_ = center;
	AButtonPosition_.y = 588.0f;

	uint32_t texture = TextureManager::Load("TitleSprite/name.png");
	this->textSprite_.reset(Sprite::Create(texture, textPosition_, 0, color, anchor));

	Vector2 size = textSprite_->GetSize();
	float scale = 1.25f;
	textSprite_->SetSize({ size.x / scale,size.y / scale });

	texture = TextureManager::Load("TitleSprite/ghost2.png");
	this->ghostSprite_.reset(Sprite::Create(texture, ghostPosition_, 0, color, anchor));
	texture = TextureManager::Load("TitleSprite/title01.png");
	this->backGroundSprite_.reset(Sprite::Create(texture, center, 0, color, anchor));
	texture = TextureManager::Load("TitleSprite/Abutton.png");
	this->AButtonSprite_.reset(Sprite::Create(texture, AButtonPosition_, 0, color, anchor));

	amplitude_ = 0.3f;

}

void TitleScene::Finalize() {
}

void TitleScene::Update() {
	GhostSpriteAnimation();
#ifdef _DEBUG

	ImGui::Begin("textpos");
	ImGui::DragFloat2("text", &textPosition_.x, 0.1f, 0, 1280);
	ImGui::DragFloat2("ghost", &ghostPosition_.x, 0.1f, 0, 1280);
	ImGui::DragFloat2("Abutton", &AButtonPosition_.x, 0.1f, 0, 1280);
	if (ImGui::TreeNode("animation")) {
		ImGui::DragFloat("frequens", &fre_, 0.1f, -1, 5);
		ImGui::DragFloat("amplitude", &amplitude_, 0.1f, -10, 10);
		ImGui::TreePop();
	}

	ImGui::End();

	textSprite_->SetPosition(textPosition_);
	ghostSprite_->SetPosition(ghostPosition_);
	AButtonSprite_->SetPosition(AButtonPosition_);

#endif // _DEBUG


	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		SceneManager::GetInstance()->ChangeScene("GAME");
	}
	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		//transition_->TransitionSetting();
	}
	if (transition_->GetIsBlackOutHalf()) {
		SceneManager::GetInstance()->ChangeScene("GAME");
	}

	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoyStickState(0, joyState)) {
		bool KeyInput = joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A;
		if (KeyInput) {
			transition_->TransitionSetting();
		}
	}

	transition_->Update();
}

void TitleScene::Draw() 
{
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Sprite::PreDraw(commandList);

	backGroundSprite_->Draw();
	textSprite_->Draw();
	ghostSprite_->Draw();
	AButtonSprite_->Draw();

	Sprite::PostDraw();

	transition_->Draw();
}

void TitleScene::GhostSpriteAnimation()
{
	waveAnimation_t_ += 0.01f;
	if (waveAnimation_t_ >= 1.0f) {
		waveAnimation_t_ -= 2.0f;
	}
	waveVelocity_.y = amplitude_ * std::cosf(1.0f * float(std::numbers::pi) * fre_ * waveAnimation_t_);

	ghostPosition_.y += waveVelocity_.y;


}
