#pragma once
#include "TextureManager.h"
#include "MyStruct.h"
#include "Mesh.h"
#include "LightGroup.h"
#include "WorldTransform.h"
#include "viewProjection.h"
#include <string>
#include <unordered_map>
#include <vector>

/// <summary>
/// モデルデータ
/// </summary>
class Model {
public: // 列挙子
	enum class RootParameter {
		kWorldTransform,	// ワールド変換行列
		kViewProjection,	// ビュープロジェクション変換行列
		kMaterial,			// マテリアル
		kTexture,			// テクスチャ
		kLight,				// ライト
	};

	enum class BlendMode {
		kNone,
		kNormal,
		kAdd,
		kSubtract,
		kMultiply,
		kScreen,
		kCountBlendMode
	};

private: // 静的メンバ定数
	// リソースデータが入ってるファイルのパス
	static const std::string kBaseDirectory;
	// 基本はこのモデルを読み込む
	static const std::string kDefaultModelName;

private: // 静的メンバ変数
	// デスクリプタサイズ
	static UINT sDescreptorHandleIncrementSize_;
	// コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList_;
	// ルートシグネチャ
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature_;
	// パイプラインステートオブジェクト
	static std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, size_t(BlendMode::kCountBlendMode)> sPipelineStates_;
	// ライト
	static std::unique_ptr<LightGroup> lightGroup_;
public: // 静的メンバ関数
	/// <summary>
	/// 静的初期化
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// グラフィックスパイプラインの初期化
	/// </summary>
	static void InitializeGraphicsPipeline();

	/// <summary>
	/// モデルの生成
	/// </summary>
	/// <returns>モデル</returns>
	static Model* Create();

	/// <summary>
	/// .objからモデルを生成
	/// </summary>
	/// <param name="modelname">.obj, .mtlがあるファイルの名前(オブジェクト名)</param>
	/// <param name="smoothing">現在無効</param>
	/// <returns>モデル</returns>
	static Model* CreateFlomObj(const std::string& modelname);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="commandList">描画コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* commandList, BlendMode blendMode);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

	static void SetIsActive() {
		//lightGroup_->
	}
public: // メンバ関数

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Model();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="modelname">.obj, .mtlがあるファイルの名前(オブジェクト名)</param>
	/// <param name="smoothing">現在無効</param>
	void Initialize(const std::string& modelname);
	
	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="worldTransform">ワールド変換データ</param>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Draw(
		const WorldTransform& worldTransform, const ViewProjection& viewProjection);

	/// <summary>
	/// 描画処理(テクスチャ変更)
	/// </summary>
	/// <param name="worldTransform">ワールド変換データ</param>
	/// <param name="viewProjection">ビュープロジェクション</param>
	/// <param name="textireHandle">テクスチャハンドル</param>
	void Draw(
		const WorldTransform& worldTransform, const ViewProjection& viewProjection,
		uint32_t textireHandle);

	/// <summary>
	/// メッシュのコンテナを取得
	/// </summary>
	/// <returns></returns>
	inline const std::vector<Mesh*>& GetMeshes() { return meshes_; }

private: // メンバ変数
	// 名前
	std::string name_;
	// メッシュコンテナ
	std::vector<Mesh*> meshes_;
	// マテリアルコンテナ
	std::unordered_map<std::string, Material*> materials_;
	// デフォルトマテリアル
	Material* defaultMaterial_ = nullptr;
private: // 非公開メンバ関数
	/// <summary>
	/// モデルデータの読み込み
	/// </summary>
	/// <param name="modelname">.obj, .mtlがあるファイルの名前(オブジェクト名)</param>
	/// <param name="smoothing">現在無効</param>
	void LoadModel(const std::string& modelname);

	/// <summary>
	/// マテリアルデータの読み込み
	/// </summary>
	/// <param name="directoryPath">オブジェクトファイルがあるパス</param>
	/// <param name="filename">.obj, .mtlがあるファイルの名前(オブジェクト名)</param>
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// マテリアルをコンテナに追加
	/// </summary>
	/// <param name="material"></param>
	void AddMaterial(Material* material);

	/// <summary>
	/// テクスチャの読み込み
	/// </summary>
	void LoadTextures();
};

