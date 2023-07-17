#include "SolidCube.h"
#include "DirectXFramework.h"
SolidCube::SolidCube(wstring ObjectName) : SceneNode(ObjectName)
{
	//Compile Shaders and Set Vertex Layout
	_parentDXDevice = DirectXFramework::GetDXFramework();

	DWORD shaderCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	ComPtr<ID3D10Blob> vertexCompileMessages;
	ComPtr<ID3D10Blob> pixelCompileMessages;
	HRESULT hr;
	hr = D3DCompileFromFile(
		L"shader.hlsl",
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VS", "vs_5_0",
		shaderCompileFlags, 0,
		vertexShaderByteCode.GetAddressOf(),
		vertexCompileMessages.GetAddressOf()
	);
	ThrowIfFailed(hr);

	hr = D3DCompileFromFile(
		L"shader.hlsl",
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PS", "ps_5_0",
		shaderCompileFlags, 0,
		pixelShaderByteCode.GetAddressOf(),
		pixelCompileMessages.GetAddressOf()
	);
		

	if (vertexCompileMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)vertexCompileMessages->GetBufferPointer(), 0, 0);
	}
	
	if (pixelCompileMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)pixelCompileMessages->GetBufferPointer(), 0, 0);
	}

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = _parentDXDevice->GetDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc),
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		vertexInputLayout.GetAddressOf()
	);

	ThrowIfFailed(hr);

	//Create Constant Buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ThrowIfFailed(
		_parentDXDevice->GetDevice()->CreateBuffer(
			&bufferDesc, NULL, constantBuffer.GetAddressOf()));

	//Create Vertex and Pixel Shader
	hr = _parentDXDevice->GetDevice()->CreateVertexShader(
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		NULL,
		vertexShader.GetAddressOf()
	);

	ThrowIfFailed(hr);

	hr = _parentDXDevice->GetDevice()->CreatePixelShader(
		pixelShaderByteCode->GetBufferPointer(),
		pixelShaderByteCode->GetBufferSize(),
		NULL,
		pixelShader.GetAddressOf()
	);

	ThrowIfFailed(hr);


	//Builty Geometry

	Vertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::HotPink) },
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) },
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) },
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) },
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) }
	};

	// Setup the structure that specifies how big the vertex 
	// buffer should be
	D3D11_BUFFER_DESC vertexBufferDescriptor;
	vertexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDescriptor.ByteWidth = sizeof(Vertex) * 8;
	vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescriptor.CPUAccessFlags = 0;
	vertexBufferDescriptor.MiscFlags = 0;
	vertexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the vertices from
	D3D11_SUBRESOURCE_DATA vertexInitialisationData;
	vertexInitialisationData.pSysMem = &vertices;

	ThrowIfFailed(
		_parentDXDevice->GetDevice()->CreateBuffer(
			&vertexBufferDescriptor, &vertexInitialisationData,
			vertexBuffer.GetAddressOf()
		)
	);

	// Create the index buffer
	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	// Setup the structure that specifies how big the index 
	// buffer should be
	D3D11_BUFFER_DESC indexBufferDescriptor;
	indexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDescriptor.ByteWidth = sizeof(UINT) * 36;
	indexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDescriptor.CPUAccessFlags = 0;
	indexBufferDescriptor.MiscFlags = 0;
	indexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the indices from
	D3D11_SUBRESOURCE_DATA indexInitialisationData;
	indexInitialisationData.pSysMem = &indices;

	// and create the index buffer
	ThrowIfFailed(
		_parentDXDevice->GetDevice()->CreateBuffer(
			&indexBufferDescriptor, 
			&indexInitialisationData, 
			indexBuffer.GetAddressOf())
	);
}

void SolidCube::Update(FXMMATRIX& currentWorldTransformation)
{
	XMStoreFloat4x4(&_combinedWorldTransformation, XMMatrixIdentity() * currentWorldTransformation );
}

void SolidCube::Render()
{
	ComPtr<ID3D11Device> device = _parentDXDevice->GetDevice();
	ComPtr<ID3D11DeviceContext> dc = _parentDXDevice->GetDeviceContext();

	XMMATRIX projectionMatrix = DirectXFramework::GetDXFramework()->GetProjectionTransformation();
	XMMATRIX viewMatrix = DirectXFramework::GetDXFramework()->GetViewTransformation();

	XMMATRIX completeTransform = XMLoadFloat4x4(& _worldTransformation) * viewMatrix * projectionMatrix;
	CBuffer cBuffer;
	cBuffer.world_view_projectionMatrix = completeTransform;

	dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &cBuffer, 0, 0);
	dc->IASetInputLayout(vertexInputLayout.Get());
	dc->VSSetShader(vertexShader.Get(),0,0);
	dc->PSSetShader(pixelShader.Get(),0, 0);
	
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	dc->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dc->DrawIndexed(36,0,0);
	
}
