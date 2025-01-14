#if BUILD_DIRECTX
#include "Renderer_DX.h"

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "GameObject.h"
#include "Mesh.h"
#include "VBO.h"
#include "Utils.h"

/******************************************************************************************************************/

Renderer_DX::Renderer_DX(HWND hWnd):
	Renderer(), _hWnd(hWnd), _depthStencil(nullptr), _depthStencilView(nullptr), _constantBuffer(nullptr)
{
}

/******************************************************************************************************************/

Renderer_DX::~Renderer_DX()
{
	Destroy();
	//delete _swapchain;		// the pointer to the swap chain interface
	//delete _device;			// the pointer to our Direct3D device interface
	//delete _context;		// the pointer to our Direct3D device context
	//delete _backbuffer;		// the pointer to our back buffer
	//delete _layout;			// the pointer to the input layout
	//delete _vertexShader;	// the pointer to the vertex shader
	//delete _pixelShader;	// the pointer to the pixel shader
	//delete _constantBuffer;

	//delete _indexBuffer;
	//delete _constantBuffer;
	//delete _rasterizerState;

	//delete _depthStencil;
	//delete _depthStencilView;
}

/******************************************************************************************************************/

void Renderer_DX::ClearScreen()
{
	_context->ClearRenderTargetView(_backbuffer, D3DXCOLOR(_clearColour.r(), _clearColour.g(), _clearColour.b(), _clearColour.a()));
	//_context->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
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
	_rasterizerState->Release();
	_context->Release();

	//if (_indexBuffer) {
	//	_indexBuffer->Release();
	//}

	if (_depthStencil) {
		_depthStencil->Release();
	}

	if (_constantBuffer)
	{
		_constantBuffer->Release();
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

/******************************************************************************************************************/

void Renderer_DX::Draw(const std::shared_ptr<GameObject> go, const Colour& colour)
{
	auto goWorld = _world * //DirectX::XMMatrixIdentity() *
		DirectX::XMMatrixTranslation(go->GetPosition().x(), go->GetPosition().y(), go->GetPosition().z()) *
		DirectX::XMMatrixRotationRollPitchYaw(0, DirectX::XMConvertToRadians(go->GetAngle()), 0) *
		DirectX::XMMatrixScaling(go->GetScale(), go->GetScale(), go->GetScale());

	_context->OMSetRenderTargets(1, &_backbuffer, _depthStencilView);
	_context->IASetInputLayout(_layout);

	ConstantBuffer cb{};
	colour.copyToArray((float*)&cb.Colour);
	cb.World = DirectX::XMMatrixTranspose(goWorld);
	cb.View = DirectX::XMMatrixTranspose(_view);
	cb.Projection = DirectX::XMMatrixTranspose(_proj);
	_context->UpdateSubresource(_constantBuffer, 0, nullptr, &cb, 0, 0);
	_context->VSSetConstantBuffers(0, 1, &_constantBuffer);
	_context->PSSetConstantBuffers(0, 1, &_constantBuffer);

	// select which primtive type we are using
	_context->IASetPrimitiveTopology(topology);

	go->GetMesh()->GetVBO()->Draw(this, go->GetGeneratorData(), go->GetInstanceCount());
}

/******************************************************************************************************************/

void Renderer_DX::Initialise(int width, int height)
{

#pragma region DirectX setup
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
	ID3D11Texture2D* p_backbuffer;
	if (!SUCCEEDED(_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&p_backbuffer))) {
		throw std::runtime_error("Failed to get swapchain buffer!");
	}

	// use the back buffer address to create the render target
	if (!SUCCEEDED(_device->CreateRenderTargetView(p_backbuffer, NULL, &_backbuffer))) {
		throw std::runtime_error("Failed to create swapchain!");
	}

	//D3D11_TEXTURE2D_DESC descDepth = {};
	//descDepth.Width = width;
	//descDepth.Height = height;
	//descDepth.MipLevels = 1;
	//descDepth.ArraySize = 1;
	//descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//descDepth.SampleDesc.Count = 1;
	//descDepth.SampleDesc.Quality = 0;
	//descDepth.Usage = D3D11_USAGE_DEFAULT;
	//descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	//descDepth.CPUAccessFlags = 0;
	//descDepth.MiscFlags = 0;
	//if (!SUCCEEDED(_device->CreateTexture2D(&descDepth, nullptr, &_depthStencil))) {
	//	throw std::runtime_error("Failed to create depth texture!");
	//}

	// Create the depth stencil view
	//D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	//descDSV.Format = descDepth.Format;
	//descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//descDSV.Texture2D.MipSlice = 0;
	//if (!SUCCEEDED(_device->CreateDepthStencilView(_depthStencil, &descDSV, &_depthStencilView))) {
	//	throw std::runtime_error("Failed to create depth stencil!");
	//}

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
#pragma endregion  DirectX setup

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	//rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = TRUE;
	_device->CreateRasterizerState(&rasterizerDesc, &_rasterizerState);
	_context->RSSetState(_rasterizerState);

	_world = DirectX::XMMatrixIdentity();
	// Initialize the view matrix
	// http://www.directxtutorial.com/Lesson.aspx?lessonid=9-4-5
	// https://www.3dgep.com/understanding-the-view-matrix/
	const auto& cam = GetCameraPosition();
	_eye = DirectX::XMVectorSet(cam[0].r, cam[0].g, cam[0].b, 0.0f);
	_at = DirectX::XMVectorSet(cam[1].r, cam[1].g, cam[1].b, 0.0f);
	_up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	_view = DirectX::XMMatrixLookAtLH(_eye, _at, _up);
	_proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, float(width) / (float)height, 0.1f, 1000.0f);
	
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
	ID3D10Blob* PS,* VS;
	ID3DBlob* compilationErrors = nullptr;

	HRESULT hr;
	hr = D3DX11CompileFromFile(L"shader_VS.hlsl", 0, 0, "main", "vs_5_0", D3D10_SHADER_DEBUG, 0, 0, &VS, &compilationErrors, 0);
	if (!SUCCEEDED(hr)) {
		if (compilationErrors)
		{
			// Display compilation errors
			OutputDebugStringA(static_cast<const char*>(compilationErrors->GetBufferPointer()));
			compilationErrors->Release();
		}
		auto errMessage = Utils::GetErrorMessage(hr);
		auto output = L"Failed to read shader file!\r\n" + std::wstring(errMessage.begin(), errMessage.end());
		OutputDebugString(output.c_str());
		throw std::exception("Failed to read shader file!");
	}
	hr = D3DX11CompileFromFile(L"shader_PS.hlsl", 0, 0, "main", "ps_5_0", 0, 0, 0, &PS, &compilationErrors, 0);
	if (!SUCCEEDED(hr)) {
		if (compilationErrors)
		{
			// Display compilation errors
			OutputDebugStringA(static_cast<const char*>(compilationErrors->GetBufferPointer()));
			compilationErrors->Release();
		}
		auto errMessage = Utils::GetErrorMessage(hr);
		auto output = L"Failed to read shader file!\r\n" + std::wstring(errMessage.begin(), errMessage.end());
		OutputDebugString(output.c_str());
		throw std::exception("Failed to read shader file!");
	}

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
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	_device->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &_layout);
	_context->IASetInputLayout(_layout);

	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(D3D11_BUFFER_DESC));
	cbDesc.ByteWidth = sizeof(ConstantBuffer);
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;

	// Create the buffer.
	hr = _device->CreateBuffer(&cbDesc, nullptr, &_constantBuffer);
	if (!SUCCEEDED(hr)) {
		auto errMessage = Utils::GetErrorMessage(hr);
		auto output = L"Failed to create constant buffer!\r\n" + std::wstring(errMessage.begin(), errMessage.end());
		OutputDebugString(output.c_str());
		throw std::runtime_error("Failed to create constant buffer!");
	}
}

void Renderer_DX::InitialiseHud() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(_hWnd);
	ImGui_ImplDX11_Init(GetDevice(), GetContext());
}

/******************************************************************************************************************/

#endif
