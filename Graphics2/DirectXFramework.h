#pragma once
#include <vector>
#include "Framework.h"
#include "DirectXCore.h"
#include "SceneGraph.h"
#include "ResourceManager.h"
#include <time.h>
#include <chrono>
#include "Camera.h"

class DirectXFramework : public Framework
{
public:
	DirectXFramework();
	DirectXFramework(unsigned int width, unsigned int height);

	virtual void CreateSceneGraph();
	virtual void UpdateSceneGraph();

	bool Initialise();
	void Update();
	void Render();
	void OnResize(WPARAM wParam);
	void Shutdown();

	double GetDeltaTime();

	static DirectXFramework *			GetDXFramework();

	ComPtr<ID3D11VertexShader> CompileVertexShader(wstring FileName, string entryPoint, string shaderTarget);
	ComPtr<ID3D11PixelShader> CompilePixelShader(wstring FileName, string entryPoint, string shaderTarget);

	inline SceneGraphPointer			GetSceneGraph() { return _sceneGraph; }
	inline ComPtr<ID3D11Device>			GetDevice() { return _device; }
	inline ComPtr<ID3D11DeviceContext>	GetDeviceContext() { return _deviceContext; }

	inline shared_ptr<ResourceManager> GetResourceManager() { return _resourceManager; }

	XMMATRIX							GetViewTransformation();
	XMMATRIX							GetProjectionTransformation();

	void								SetBackgroundColour(XMFLOAT4 backgroundColour);

	inline shared_ptr<Camera> GetCamera() { return _camera; }

private:
	//DeltaTime Variables
	std::chrono::steady_clock::time_point timeNow = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point timeLastFrame;
	double _deltaTime = 0;

	
	ComPtr<ID3D11Device>				_device;
	ComPtr<ID3D11DeviceContext>			_deviceContext;
	ComPtr<IDXGISwapChain>				_swapChain;
	ComPtr<ID3D11Texture2D>				_depthStencilBuffer;
	ComPtr<ID3D11RenderTargetView>		_renderTargetView;
	ComPtr<ID3D11DepthStencilView>		_depthStencilView;

	D3D11_VIEWPORT						_screenViewport;

	// Our vectors and matrices.  Note that we cannot use
	// XMVECTOR and XMMATRIX for class variables since they need
	// to be aligned on 16-byte boundaries and the compiler cannot
	// guarantee this for class variables

	// For now, we are storing our camera vectors and matrix here.
	// We will move it to a separate Camera class later
	//removed cause camera XMFLOAT4							_eyePosition;
	//removed cause camera XMFLOAT4							_focalPointPosition;
	//removed cause camera XMFLOAT4							_upVector;

	//removed cause camera XMFLOAT4X4							_viewTransformation;
	XMFLOAT4X4							_projectionTransformation;

	SceneGraphPointer					_sceneGraph;
	shared_ptr<ResourceManager>		_resourceManager;


	float							    _backgroundColour[4];

	bool GetDeviceAndSwapChain();

	shared_ptr<Camera> _camera;
};

