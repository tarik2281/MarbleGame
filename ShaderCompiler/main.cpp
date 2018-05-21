
#include <iostream>
#include <vector>
#include <Windows.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

#define VS_ENTRY "VSMain"
#define GS_ENTRY "GSMain"
#define PS_ENTRY "PSMain"

#define VS_TARGET "vs_5_0"
#define GS_TARGET "gs_5_0"
#define PS_TARGET "ps_5_0"

#define VS_SUFFIX "_V.cso"
#define GS_SUFFIX "_G.cso"
#define PS_SUFFIX "_P.cso"

#ifdef _DEBUG
#define COMPILE_MODE D3DCOMPILE_DEBUG
#else
#define COMPILE_MODE 0
#endif

#define PRINT_FORMAT(text, ...) \
	{	\
		char buffer[128]; \
		sprintf(buffer, text, __VA_ARGS__); \
		OutputDebugStringA(buffer); \
	}

enum ShaderType {
	ShaderTypeVertex,
	ShaderTypeGeometry,
	ShaderTypePixel
};

struct ShaderSource {
	void Release() {
		delete[] pSrcData;
	}

	char* pSrcData;
	unsigned int numBytes;
};

void ReadFile(const char* filePath, ShaderSource& source) {
	FILE* pFile = fopen(filePath, "rb");
	fseek(pFile, 0, SEEK_END);
	source.numBytes = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	source.pSrcData = new char[source.numBytes];
	ZeroMemory(source.pSrcData, source.numBytes);
	fread(source.pSrcData, 1, source.numBytes, pFile);
	fclose(pFile);
}

void WriteFile(const char* filePath, const ShaderSource& source) {
	FILE* pFile = fopen(filePath, "wb");
	fwrite(source.pSrcData, source.numBytes, 1, pFile);
	fclose(pFile);
}


class D3DInclude : public ID3DInclude {
public:
	HRESULT __stdcall Open(D3D_INCLUDE_TYPE, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes);
	HRESULT __stdcall Close(LPCVOID pData);

	std::string m_localDir;
};

HRESULT D3DInclude::Open(D3D_INCLUDE_TYPE incType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) {
	std::string filePath = m_localDir + pFileName;
	ShaderSource headerSource;

	ReadFile(filePath.c_str(), headerSource);
	*ppData = headerSource.pSrcData;
	*pBytes = headerSource.numBytes;
	return S_OK;
}

HRESULT D3DInclude::Close(LPCVOID pData) {
	delete[] pData;
	return S_OK;
}


typedef std::vector<std::string> FileList;

void SearchFilesInDirectory(const char* directory, FileList& list) {
	HANDLE fHandle;
	WIN32_FIND_DATAA fileData;
	fHandle = FindFirstFileA(directory, &fileData);

	do {
		if (!(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			list.push_back(fileData.cFileName);

	} while (FindNextFileA(fHandle, &fileData));
	FindClose(fHandle);
}

ID3DBlob* CompileShader(const ShaderSource& srcCode, ID3DInclude* inc, const char* filePath, ShaderType shaderType) {
	ID3DBlob* pCode = nullptr;
	ID3DBlob* pErrors = nullptr;
	
	const char* pEntry = nullptr;
	const char* pTarget = nullptr;

	switch (shaderType) {
	case ShaderTypeVertex:
		pEntry = VS_ENTRY;
		pTarget = VS_TARGET;
		break;
	case ShaderTypeGeometry:
		pEntry = GS_ENTRY;
		pTarget = GS_TARGET;
		break;
	case ShaderTypePixel:
		pEntry = PS_ENTRY;
		pTarget = PS_TARGET;
		break;
	default:
		break;
	}

	HRESULT res;
	if (pEntry && pTarget) {
		res = D3DCompile(srcCode.pSrcData, srcCode.numBytes, filePath, nullptr, inc, pEntry, pTarget, COMPILE_MODE, 0, &pCode, &pErrors);
		
		if (pErrors) {
			const char* errorMsg = reinterpret_cast<const char*>(pErrors->GetBufferPointer());
			OutputDebugStringA(errorMsg);
			pErrors->Release();
			pErrors = nullptr;
		}

		if (pCode) {
			return pCode;
		}
	}

	return nullptr;
}

std::string ReplaceSuffix(const std::string& fileName, const char* newSuffix) {
	size_t dotPos = fileName.find_last_of('.');
	std::string newFileName = fileName.substr(0, dotPos) + newSuffix;
	return newFileName;
}

int main(int argc, char* argv[]) {
	std::string directory = "D:\\Visual Studio\\Projects\\MarbleGame\\MarbleGame\\Shaders\\";
	//std::string outDirectory = directory + "bin\\";
	std::string outDirectory = "D:\\Visual Studio\\Projects\\MarbleGame\\Debug\\Resources\\Shaders\\";
	CreateDirectoryA(outDirectory.c_str(), NULL);
	FileList list;
	SearchFilesInDirectory((directory + "*.fx").c_str(), list);
	
	D3DInclude include;
	include.m_localDir = directory;

	HRESULT res;
	FileList::iterator it;
	for (it = list.begin(); it != list.end(); ++it) {
		char buffer[128];
		sprintf(buffer, "Processing file %s\n", it->c_str());
		OutputDebugStringA(buffer);

		std::string filePath = directory + *it;
		std::string outFilePath = outDirectory + *it;
		ShaderSource fileData;
		ReadFile(filePath.c_str(), fileData);

		ID3DBlob* pCode = nullptr;
		pCode = CompileShader(fileData, &include, filePath.c_str(), ShaderTypeVertex);

		if (pCode) {
			PRINT_FORMAT("\tCompiled vertex shader for %s\n", it->c_str());
			ShaderSource source;
			source.pSrcData = (char*)pCode->GetBufferPointer();
			source.numBytes = pCode->GetBufferSize();
			std::string outFileName = ReplaceSuffix(outFilePath, VS_SUFFIX);
			WriteFile(outFileName.c_str(), source);
			pCode->Release();
			pCode = nullptr;
		}

		pCode = CompileShader(fileData, &include, filePath.c_str(), ShaderTypePixel);
		if (pCode) {
			PRINT_FORMAT("\tCompiled pixel shader for %s\n", it->c_str());
			ShaderSource source;
			source.pSrcData = (char*)pCode->GetBufferPointer();
			source.numBytes = pCode->GetBufferSize();
			std::string outFileName = ReplaceSuffix(outFilePath, PS_SUFFIX);
			WriteFile(outFileName.c_str(), source);
			pCode->Release();
			pCode = nullptr;
		}

		fileData.Release();
	}

	

	Sleep(2000);
	return EXIT_SUCCESS;
}