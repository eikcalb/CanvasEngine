#if BUILD_DIRECTX
#include "VBO_DX.h"

#include "Renderer_DX.h"
#include "Utils.h"

/******************************************************************************************************************/

VBO_DX::VBO_DX()
	: _vbo(NULL), _idx(NULL)
{
}

/******************************************************************************************************************/

VBO_DX::~VBO_DX()
{
	_idx->Release();
	_vbo->Release();
	if (_ins) {
		_ins->Release();
	}

	if (_srv) {
		_srv->Release();
	}

	delete _idx;
	delete _vbo;
	delete _ins;
	delete _srv;
}

/******************************************************************************************************************/

void VBO_DX::Create(
	Renderer* renderer,
	Vertex vertices[],
	int numVertices,
	unsigned short indices[],
	int numIndices,
	unsigned int stride,
	unsigned long size
)
{
	/// Apparently, using unsigned int for the index instead of unsigned short breaks
	/// the output for DirectX.
	/// Time lost = 7 days! (Documenting this nightmare that gave me nightmares!).

	Renderer_DX* rendererDX = (Renderer_DX*)renderer;

	_numVertices = numVertices;
	_numIndices = numIndices;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;                // write access access by CPU and GPU
	bd.ByteWidth = sizeof(Vertex) * numVertices;   // size is the VERTEX struct * num vertices
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = 0;    // allow CPU to write in buffer

	D3D11_SUBRESOURCE_DATA vbd;
	ZeroMemory(&vbd, sizeof(D3D10_SUBRESOURCE_DATA));
	vbd.pSysMem = vertices;
	rendererDX->GetDevice()->CreateBuffer(&bd, &vbd, &_vbo);        // create the buffer

	D3D11_BUFFER_DESC id;
	ZeroMemory(&id, sizeof(id));

	id.Usage = D3D11_USAGE_DEFAULT;
	id.ByteWidth = sizeof(unsigned int) * numIndices;
	id.BindFlags = D3D11_BIND_INDEX_BUFFER;
	id.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA iData;
	ZeroMemory(&iData, sizeof(D3D11_SUBRESOURCE_DATA));
	iData.pSysMem = indices;
	iData.SysMemPitch = 0;

	rendererDX->GetDevice()->CreateBuffer(&id, &iData, &_idx);

	if (stride <= 0) {
		return;
	}

	D3D11_BUFFER_DESC instanceBufferDesc{};
	ZeroMemory(&instanceBufferDesc, sizeof(D3D11_BUFFER_DESC));
	instanceBufferDesc.ByteWidth = stride * size;
	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	instanceBufferDesc.StructureByteStride = stride;

	// Create the buffer to store instance data.
	auto hr = rendererDX->GetDevice()->CreateBuffer(&instanceBufferDesc, nullptr, &_ins);
	if (!SUCCEEDED(hr)) {
		auto errMessage = Utils::GetErrorMessage(hr);
		auto output = L"Failed to create instance buffer!\r\n" + std::wstring(errMessage.begin(), errMessage.end());
		OutputDebugString(output.c_str());
		throw std::runtime_error("Failed to create instance buffer!");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // Assuming you have the appropriate format for your data
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	//srvDesc.Buffer.ElementWidth = stride * size;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = size;

	hr = rendererDX->GetDevice()->CreateShaderResourceView(_ins, &srvDesc, &_srv);
	if (!SUCCEEDED(hr)) {
		auto errMessage = Utils::GetErrorMessage(hr);
		auto output = L"Failed to create shader resource!\r\n" + std::wstring(errMessage.begin(), errMessage.end());
		OutputDebugString(output.c_str());
		throw std::runtime_error("Failed to create shader resource!");
	}

	rendererDX->GetContext()->VSSetShaderResources(0, 1, &_srv);
}

/******************************************************************************************************************/

void VBO_DX::Draw(Renderer* renderer, const std::shared_ptr<VBOInstanceData> instanceData, unsigned long count)
{
	Renderer_DX* rendererDX = (Renderer_DX*)renderer;

	if (instanceData && _ins) {
		// Here, we will be setting up the instance data that will be used to render multiple
		// instances of the same object on the GPU.
		// 
		// References for GPU instancing concept are below. Although they sugegst utilizing a
		// vertex buffer, that would require updating the input layout and that cannot be done
		// dynamically, hence the need for using a constant buffer and using a pointer for the
		// instance data representation.
		// https://developer.nvidia.com/gpugems/gpugems2/part-i-geometric-complexity/chapter-3-inside-geometry-instancing
		// https://www.braynzarsoft.net/viewtutorial/q16390-33-instancing-with-indexed-primitives
		// https://www.rastertek.com/dx11tut37.html
		//
		// According to the limitation published here: https://www.gamedev.net/forums/topic/624529-structured-buffers-vs-constant-buffers/,
		// a constant buffer cannot be more than 64kb, hence it is limited for the purpose of a
		// generator. Instead, we will utilize a structured buffer.
		if (instanceData->shouldUpdate) {
			rendererDX->GetContext()->UpdateSubresource(_ins, 0, nullptr, instanceData->data, 0, 0);
			// After updating the shader resource, it is automatically set to pause update until
			// the application up[dates it again.
			//instanceData->shouldUpdate = false;
		}
	}

	// select which vertex buffer to display
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	rendererDX->GetContext()->IASetVertexBuffers(0, 1, &_vbo, &stride, &offset);

	if (_numIndices > 0) {
		rendererDX->GetContext()->IASetIndexBuffer(_idx, DXGI_FORMAT_R16_UINT, offset);
		rendererDX->GetContext()->DrawIndexedInstanced(_numIndices, count, 0, 0, 0);
	}
	else {
		rendererDX->GetContext()->DrawInstanced(_numVertices, count, 0, 0);
	}
}

/******************************************************************************************************************/

#endif