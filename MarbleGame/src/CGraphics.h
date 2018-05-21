
#ifndef _CGRAPHICS_H_
#define _CGRAPHICS_H_

#include <d3d11.h>

#include "Maths/VectorMath.h"

typedef Vector4 Color;
typedef ID3D11Device GraphicsDevice;
typedef ID3D11DeviceContext GraphicsContext;

//
// Release macro for all D3D objects
//

#define DX_SAFE_RELEASE(object) if (object) { \
									object->Release(); \
									object = nullptr; \
								}

class CGraphics;

enum CNumSamples {
	CNumSamples1 = 1,
	CNumSamples2 = 2,
	CNumSamples4 = 4,
	CNumSamples8 = 8
};

extern CGraphics* g_pGraphics;

#define TEMP_CONTEXT(context) GraphicsContext* context = g_pGraphics->GetContext();
#define TEMP_DEVICE(device) GraphicsDevice* device = g_pGraphics->GetDevice();

class CGraphics {
public:
	void Initialize(int, int, HWND, CNumSamples);
	void Release();

	inline ID3D11Device* GetDevice() { return m_pDevice; }
	inline ID3D11DeviceContext* GetContext() { return m_pContext; }

	void SetBackBuffer();
	void ClearBackBuffer(const Vector4& = Vector4(0.0f), float = 1.0f);
	void SwapBuffers();

	ID3D11RasterizerState* GetCurrentRSState() {
		ID3D11RasterizerState* temp;
		m_pContext->RSGetState(&temp);
		return temp;
	}
	void SetDefaultRSState();
	void SetDefaultDSState();
	void SetViewport(int x, int y, int width, int height);

	void DisableDepth();

	void EnableBlending();
	void DisableBlending();
private:
	// MSAA
	CNumSamples m_numSamples;

	IDXGISwapChain* m_pSwapChain;
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;
	
	// back buffer
	int m_width;
	int m_height;
	ID3D11RenderTargetView* m_pRTView;
	ID3D11DepthStencilView* m_pDSView;

	ID3D11RasterizerState* m_pRSState;
	ID3D11DepthStencilState* m_pDSState;

	bool m_bBlendingEnabled;
	ID3D11BlendState* m_pBlendState;
};

inline void CGraphics::SetDefaultDSState() {
	m_pContext->OMSetDepthStencilState(nullptr, 0);
}

inline void CGraphics::DisableDepth() {
	m_pContext->OMSetDepthStencilState(m_pDSState, 0);
}


//
// Creation macros for buffers
//

#define MakeBuffer(pDevice, pBuffer, isDynamic, bufferType, pData, iDataSize) \
{ \
	D3D11_BUFFER_DESC desc;	\
	desc.ByteWidth = (iDataSize); \
	desc.Usage = ((isDynamic) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT); \
	desc.BindFlags = (bufferType); \
	desc.CPUAccessFlags = ((isDynamic) ? D3D11_CPU_ACCESS_WRITE : 0); \
	desc.MiscFlags = 0; \
	desc.StructureByteStride = 0; \
	\
	D3D11_SUBRESOURCE_DATA subData; \
	subData.pSysMem = (pData); \
	subData.SysMemPitch = 0; \
	subData.SysMemSlicePitch = 0; \
	pDevice->CreateBuffer(&desc, &subData, &(pBuffer)); \
}

#define MakeDynamicBuffer(pDevice, pBuffer, bufferType, iDataSize) \
{ \
	D3D11_BUFFER_DESC desc; \
	desc.ByteWidth = (iDataSize); \
	desc.Usage = D3D11_USAGE_DYNAMIC; \
	desc.BindFlags = (bufferType); \
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; \
	desc.MiscFlags = 0; \
	desc.StructureByteStride = 0; \
	\
	pDevice->CreateBuffer(&desc, nullptr, &pBuffer); \
}

#define MakeDynamicBufferRes(pDevice, pBuffer, bufferType, iDataSize, result) \
{ \
	D3D11_BUFFER_DESC desc; \
	desc.ByteWidth = (iDataSize); \
	desc.Usage = D3D11_USAGE_DYNAMIC; \
	desc.BindFlags = (bufferType); \
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; \
	desc.MiscFlags = 0; \
	desc.StructureByteStride = 0; \
	\
	result = pDevice->CreateBuffer(&desc, nullptr, &pBuffer); \
}

#define UpdateBufferStruct(pContext, pBuffer, data) \
{ \
	D3D11_MAPPED_SUBRESOURCE subRes; \
	pContext->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes); \
	memcpy(subRes.pData, &data, sizeof(data)); \
	pContext->Unmap(pBuffer, 0); \
}

/*template <class T>
class ConstantBuffer {
public:
	void Initialize(GraphicsDevice* pDevice, T* initData);
	void Release();
	void SetData(GraphicsContext* pContext, _DataType* data);

	inline ID3D11Buffer* GetBuffer() { return m_pBuffer; }

	ConstantBuffer<_DataType>() { m_pBuffer = nullptr; }
	~ConstantBuffer<_DataType>() { Release(); }
private:
	typedef T _DataType;

	ID3D11Buffer* m_pBuffer;
};

template <class T>
inline void ConstantBuffer<T>::Initialize(GraphicsDevice* pDevice, _DataType* initData) {
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(_DataType);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	if (initData) {
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = initData;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		pDevice->CreateBuffer(&desc, &data, &m_pBuffer);
	}
	else {
		pDevice->CreateBuffer(&desc, nullptr, &m_pBuffer);
	}
}

template <class T>
inline void ConstantBuffer<T>::Release() {
	DX_SAFE_RELEASE(m_pBuffer);
}

template <class T>
inline void ConstantBuffer<T>::SetData(GraphicsContext* pContext, _DataType* data) {
	D3D11_MAPPED_SUBRESOURCE subData;
	pContext->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subData);
	memcpy(subData.pData, data, sizeof(_DataType));
	pContext->Unmap(m_pBuffer, 0);
}*/

#endif
