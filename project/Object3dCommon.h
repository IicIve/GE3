#pragma once
#include "DirectXCommon.h"
#include <wrl.h>

class Object3dCommon {
public:
	void Initialize(DirectXCommon* dxCommon);

	//共通描画設定
	void CreatePrimitiveTopology();
	//void PreDraw();
	
	//ゲッター
	DirectXCommon* GetDxCommon() { return dxCommon_; }

private:
	//ルートシグネチャの作成
	void CreateRootSignature();
	//グラフィックスパイプラインの生成
	void CreateGraphicsPipelineState();

	DirectXCommon* dxCommon_;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr <ID3D12PipelineState> graphicsPipelineState = nullptr;
	Microsoft::WRL::ComPtr <ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;


};

