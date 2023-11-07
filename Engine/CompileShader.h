#pragma once
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <dxcapi.h>

#include "cVertexPos.h"

#pragma comment(lib,"dxcompiler.lib")

class CompileShader {
public:
	static void Initialize();
	/// <summary>
	/// CompileShader�֐�
	/// </summary>
	static IDxcBlob* Compile(
		//Compiler����Shader�t�@�C���ւ̃p�X
		const std::wstring& filePath,
		//Compiler�Ɏg�p����Profile
		const wchar_t* profile
	);
private:
	static Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
	static Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
	static Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;
};

