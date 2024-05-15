#pragma once
#if BUILD_DIRECTX

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <DirectXMath.h>

#include "Renderer.h"

class GameObject;

struct ConstantBuffer
{
	DirectX::XMMATRIX World;
	DirectX::XMMATRIX View;
	DirectX::XMMATRIX Projection;
	DirectX::XMFLOAT4 Colour;
};

// DirectX Device & Context
class Renderer_DX :
	public Renderer
{
	// Data
protected:
	IDXGISwapChain* _swapchain;             // the pointer to the swap chain interface
	ID3D11Device* _device;                // the pointer to our Direct3D device interface
	ID3D11DeviceContext* _context;				// the pointer to our Direct3D device context
	ID3D11RenderTargetView* _backbuffer;			// the pointer to our back buffer
	ID3D11InputLayout* _layout;				// the pointer to the input layout
	ID3D11VertexShader* _vertexShader;			// the pointer to the vertex shader
	ID3D11PixelShader* _pixelShader;			// the pointer to the pixel shader

	ID3D11Buffer* _indexBuffer;
	ID3D11Buffer* _constantBuffer;

	ID3D11Texture2D* _depthStencil;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11RasterizerState* _rasterizerState;

	HWND						_hWnd;					// Window handle

	DirectX::XMMATRIX			_world;
	DirectX::XMMATRIX			_view;
	DirectX::XMMATRIX			_proj;

	DirectX::XMVECTOR			_eye;
	DirectX::XMVECTOR			_at;
	DirectX::XMVECTOR			_up;

	// Structors
public:
	Renderer_DX(HWND hWnd);
	virtual ~Renderer_DX();

	// Gets/Sets
public:
	ID3D11Device* GetDevice()					const { return _device; }
	void SetDevice(ID3D11Device* v) { _device = v; }

	ID3D11DeviceContext* GetContext()			const { return _context; }
	void SetContext(ID3D11DeviceContext* v) { _context = v; }

	// Functions
public:
	virtual void UpdateCamera() override {
		_eye = DirectX::XMVectorSet(_cameraPos[0].r, _cameraPos[0].g, _cameraPos[0].b, 0.0f);
		_at = DirectX::XMVectorSet(_cameraPos[1].r, _cameraPos[1].g, _cameraPos[1].b, 0.0f);
		_view = DirectX::XMMatrixLookAtLH(_eye, _at, _up);
	}

	// Get the ray direction in world space
	// https://www.braynzarsoft.net/viewtutorial/q16390-20-picking
	// https://stackoverflow.com/questions/39376687/mouse-picking-with-ray-casting-in-directx
	virtual glm::mat2x4 CalculateMouseRay(const int x, const int y, unsigned int width, unsigned int height) override {
		auto orig = DirectX::XMVector3Unproject(DirectX::XMVectorSet(x, y, 0,1),
			0,
			0,
			width,
			height,
			0,
			1,
			_proj,
			_view,
			_world);

		auto dest = DirectX::XMVector3Unproject(DirectX::XMVectorSet(x, y, 1, 1),
			0,
			0,
			width,
			height,
			0,
			1,
			_proj,
			_view,
			_world);

		glm::vec4 origGlm = glm::vec4(DirectX::XMVectorGetX(orig), DirectX::XMVectorGetY(orig), DirectX::XMVectorGetZ(orig), DirectX::XMVectorGetW(orig));
		glm::vec4 dirGlm = glm::vec4(DirectX::XMVectorGetX(dest), DirectX::XMVectorGetY(dest), DirectX::XMVectorGetZ(dest), DirectX::XMVectorGetW(dest));

		return glm::mat2x4(origGlm, glm::normalize(dirGlm));
		//return glm::mat2x4(origGlm, dirGlm);
	}

	// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection.html
	virtual bool IntersectMouseRay(const glm::vec4& rayOrig, const glm::vec4& rayDir, const Vector4& pos, const float radius) override {
		auto normal = glm::vec3(0.0f, 0.0f, 1.0f);
		auto point = glm::vec3(pos.x(), pos.y(), pos.z());
		auto rayOrigin = glm::vec3(rayOrig);
		auto rayDirection = glm::vec3(rayDir);

		float halfSize = radius / 2.0f;
		glm::vec3 squareMin = point - glm::vec3(halfSize, halfSize, 0.0f);
		glm::vec3 squareMax = point + glm::vec3(halfSize, halfSize, 0.0f);

		// Calculate the distance from the origin to the plane
		float planeDistance = -glm::dot(normal, point);
		// Calculate the intersection point between the ray and the plane
		float t = -(glm::dot(rayOrigin, normal) + planeDistance) / glm::dot(rayDirection, normal);
		glm::vec3 intersectionPoint = rayOrigin + rayDirection * t;

		// Step 3: Check if the intersection point lies within the bounds of the square
		if (intersectionPoint.x >= squareMin.x && intersectionPoint.x <= squareMax.x &&
			intersectionPoint.y >= squareMin.y && intersectionPoint.y <= squareMax.y)
		{
			// The ray intersects with the square
			return true;
		}

		// No intersection
		return false;
	}

	virtual void ClearScreen() override;
	virtual void Destroy() override;
	virtual void Draw(const std::shared_ptr<GameObject> mesh, const Colour& colour) override;
	virtual void Initialise(int width, int height) override;
	virtual void SwapBuffers() override;

	// Initialise the shaders
	void InitialiseShaders();
	void InitialiseHud();
};

#endif