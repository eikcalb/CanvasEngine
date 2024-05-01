#if BUILD_DIRECTX
#include "Renderer_DX.h"

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "VBO.h"
#include "Mesh.h"

/******************************************************************************************************************/

Renderer_DX::Renderer_DX(HWND hWnd)
	: _hWnd(hWnd), _depthStencil(nullptr), _depthStencilView(nullptr)
{
}

/******************************************************************************************************************/

Renderer_DX::~Renderer_DX()
{
	delete _swapchain;		// the pointer to the swap chain interface
	delete _device;			// the pointer to our Direct3D device interface
	delete _context;		// the pointer to our Direct3D device context
	delete _backbuffer;		// the pointer to our back buffer
	delete _layout;			// the pointer to the input layout
	delete _vertexShader;	// the pointer to the vertex shader
	delete _pixelShader;	// the pointer to the pixel shader
	delete _uniformBuffer;

	delete _indexBuffer;
	delete _constantBuffer;

	delete _depthStencil;
	delete _depthStencilView;
}

/******************************************************************************************************************/

void Renderer_DX::ClearScreen()
{
	_context->ClearRenderTargetView(_backbuffer, D3DXCOLOR(_clearColour.r(), _clearColour.g(), _clearColour.b(), _clearColour.a()));
	_context->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

/******************************************************************************************************************/

void Renderer_DX::Destroy()
{
	_swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode

	// close and release all existing COM objects
	_layout->Release();
	_vertexShader->Release();
	_pixelShader->Release();
	_swapchain->Release();
	_backbuffer->Release();
	_device->Release();
	_context->Release();

	if (_uniformBuffer)	
	{
		_uniformBuffer->Release();
	}


	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

/******************************************************************************************************************/

void Renderer_DX::Draw(const std::shared_ptr<Mesh> mesh, glm::mat4 MVM, const Colour& colour)
{
	_context->OMSetRenderTargets(1, &_backbuffer, _depthStencilView);
	MVM = glm::transpose(MVM);

	UniformBuffer uniforms;
	memcpy(&uniforms.MVM, &MVM[0][0], sizeof(DirectX::XMFLOAT4X4));
	colour.copyToArray((float*)&uniforms.Colour);

	// Need to update uniform buffer here
	D3D11_MAPPED_SUBRESOURCE ms;
	_context->Map(_uniformBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);		// map the buffer
	memcpy(ms.pData, &uniforms, sizeof(UniformBuffer));								// copy the data
	_context->Unmap(_uniformBuffer, NULL);											// unmap the buffer
	_context->VSSetConstantBuffers(0, 1, &_uniformBuffer);
	_context->PSSetConstantBuffers(0, 1, &_uniformBuffer);
	
	// select which primtive type we are using
	_context->IASetPrimitiveTopology(topology);
	_context->IASetInputLayout(_layout);
	
	mesh->GetVBO()->Draw(this);
}

/******************************************************************************************************************/

void Renderer_DX::Initialise(int width, int height)
{
	// Reference: https://learn.microsoft.com/en-us/windows/win32/direct3dgetstarted/complete-code-sample-for-using-a-corewindow-with-directx
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferDesc.RefreshRate.Numerator = 75;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferCount = 1;                                   // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // use 32-bit color
	scd.BufferDesc.Width = width;                   // set the back buffer width
	scd.BufferDesc.Height = height;                 // set the back buffer height
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;     // how swap chain is to be used
	scd.OutputWindow = _hWnd;                              // the window to be used
	scd.SampleDesc.Count = 4;                              // how many multisamples
	scd.Windowed = TRUE;                                   // windowed/full-screen mode
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    // allow full-screen switching

#if defined(DEBUG) || defined(_DEBUG)
	scd.Flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// create a device, device context and swap chain using the information in the scd struct
	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&_swapchain,
		&_device,
		NULL,
		&_context);

	// get the address of the back buffer
	ID3D11Texture2D *p_backbuffer;
	_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&p_backbuffer);

	// use the back buffer address to create the render target
	_device->CreateRenderTargetView(p_backbuffer, NULL, &_backbuffer);

	//D3D11_TEXTURE2D_DESC bDesc;
	//p_backbuffer->GetDesc(&bDesc);
	//// Create depth Stencil
	//CD3D11_TEXTURE2D_DESC depthStencilDesc(
	//	DXGI_FORMAT_D24_UNORM_S8_UINT,
	//	static_cast<UINT> (bDesc.Width),
	//	static_cast<UINT> (bDesc.Height),
	//	1, // This depth stencil view has only one texture.
	//	1, // Use a single mipmap level.
	//	D3D11_BIND_DEPTH_STENCIL
	//);

	//_device->CreateTexture2D(
	//	&depthStencilDesc,
	//	nullptr,
	//	&_depthStencil
	//);

	//CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

	//_device->CreateDepthStencilView(
	//	_depthStencil,
	//	&depthStencilViewDesc,
	//	&_depthStencilView
	//);

	// set the render target as the back buffer
	_context->OMSetRenderTargets(
		1,
		&_backbuffer,
		_depthStencilView
	);

	p_backbuffer->Release();

	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MaxDepth = 1;
	viewport.Width = width;
	viewport.Height = height;

	_context->RSSetViewports(1, &viewport);


	// Initialise shaders
	InitialiseShaders();
	InitialiseHud();
}

/******************************************************************************************************************/

void Renderer_DX::SwapBuffers()
{
	_swapchain->Present(1, 0); // Enabble vsync
}

/******************************************************************************************************************/

// Load and prepare shaders
void Renderer_DX::InitialiseShaders()
{
	// load and compile the two shaders
	ID3D10Blob *VS, *PS;
	D3DX11CompileFromFile(L"shaders.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
	D3DX11CompileFromFile(L"shaders.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, 0, 0);

	// encapsulate both shaders into shader objects
	_device->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &_vertexShader);
	_device->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &_pixelShader);

	// set the shader objects
	_context->VSSetShader(_vertexShader, 0, 0);
	_context->PSSetShader(_pixelShader, 0, 0);

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	_device->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &_layout);
	_context->IASetInputLayout(_layout);


	// Create uniform buffer
	UniformBuffer uniforms;

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(uniforms);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &uniforms;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Create the buffer.
	_device->CreateBuffer(&cbDesc, &InitData, &_uniformBuffer);
}

void Renderer_DX::InitialiseHud() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::StyleColorsLight();
	ImGui_ImplWin32_Init(_hWnd);
	ImGui_ImplDX11_Init(GetDevice(), GetContext());
}

/******************************************************************************************************************/

#endif