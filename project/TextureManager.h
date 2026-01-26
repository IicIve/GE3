#pragma once
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include "externals/DirectXTex/DirectXTex.h"

#include "DirectXCommon.h"

class TextureManager {
public:
	//シングルトンインスタンスの取得
	static TextureManager* GetInstance();
	//初期化
	void Initialize(DirectXCommon* dxCommon);
	//終了
	void Finalize();
	//ゲッター
	DirectXCommon* GetDxCommon() const { return dxCommon_; }

	//テクスチャファイルの読み込み
	void LoadTexture(const std::string& filePath);

	//SRVインデックスの開始番号
	uint32_t GetTextureIndexByFilePath(const std::string& filePath);

	//テクスチャ番号からGPUハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(uint32_t textureIndex);

	//メタデータを取得
	const DirectX::TexMetadata& GetMetaData(uint32_t textureIndex);

private:
	struct TextureData {
		std::string filePath;
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		//Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;

	};

	static TextureManager* instance;
	DirectXCommon* dxCommon_;
	std::vector<TextureData> textureDatas;

	//Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;

	static uint32_t kSRVIndexTop;

};

