#include "TerrainNode.h"
#include <DirectXMath.h>
#include <ios>
#include <fstream>


struct CBUFFER
{
	XMMATRIX    CompleteTransformation;
	XMMATRIX	WorldTransformation;
	XMFLOAT4	CameraPosition;
	XMVECTOR    LightVector;
	XMFLOAT4    LightColor;
	XMFLOAT4    AmbientColor;
	XMFLOAT4    DiffuseCoefficient;
	XMFLOAT4	SpecularCoefficient;
	float		Shininess;
	float		Opacity;
	float       Padding[2];
};

CBUFFER currentCBuffer;

TerrainNode::TerrainNode(wstring ObjectName) : SceneNode(ObjectName)
{
	_parentDXDevice = DirectXFramework::GetDXFramework();
	LoadHeightMap(L"HeightMap.raw");
	SetWorldTransform(XMMatrixIdentity());
	BuildRenderState();

	BuildShaders();
	BuildVertexLayout();
	BuildConstantBuffer();
	CreateMesh();
}

void TerrainNode::Render()
{
	ComPtr<ID3D11Device> Device = _parentDXDevice->GetDevice();
	ComPtr<ID3D11DeviceContext> DeviceContext = _parentDXDevice->GetDeviceContext();

	XMMATRIX projectionTransformation = DirectXFramework::GetDXFramework()->GetProjectionTransformation();
	XMMATRIX viewTransformation = DirectXFramework::GetDXFramework()->GetCamera()->GetViewMatrix();

	XMMATRIX completeTransformation = XMLoadFloat4x4(&_worldTransformation) * viewTransformation * projectionTransformation;
	currentCBuffer.CompleteTransformation = completeTransformation;
	currentCBuffer.WorldTransformation = XMLoadFloat4x4(&_worldTransformation);

	XMFLOAT4 CamPos;
	XMStoreFloat4(&CamPos, DirectXFramework::GetDXFramework()->GetCamera()->GetCameraPosition());

	currentCBuffer.CameraPosition = CamPos;
	currentCBuffer.LightVector = XMVector4Normalize(XMVectorSet(-1.0f, -1.0f, 0.0f, 0.0f));
	currentCBuffer.LightColor = XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f);
	currentCBuffer.AmbientColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	currentCBuffer.DiffuseCoefficient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	currentCBuffer.SpecularCoefficient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	currentCBuffer.Shininess = 1.0f;
	currentCBuffer.Opacity = 1;

	
	DeviceContext->VSSetShader(vertexShader.Get(),0,0);
	DeviceContext->PSSetShader(pixelShader.Get(),0,0);
	DeviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	DeviceContext->IASetInputLayout(vertexInputLayout.Get());
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	DeviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	DeviceContext->PSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	DeviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &currentCBuffer, 0, 0);

	DeviceContext->DrawIndexed(IndeciesCount, 0, 0);
}

void TerrainNode::BuildRenderState()
{
	D3D11_RASTERIZER_DESC rasterizerState;
	rasterizerState.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerState.CullMode = D3D11_CULL_BACK;
	rasterizerState.FrontCounterClockwise = false;
	rasterizerState.DepthBias = 0;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.ScissorEnable = false;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = false;

	ComPtr<ID3D11Device> device = _parentDXDevice->GetDevice();
	ComPtr<ID3D11DeviceContext> dc = _parentDXDevice->GetDeviceContext();
	dc->RSGetState(OriginalRasterState.GetAddressOf());
	ThrowIfFailed( device->CreateRasterizerState(&rasterizerState, RasterState.GetAddressOf()) );
}

void TerrainNode::CreateMesh()
{
	std::vector<VERTEX> vVector;
	std::vector<UINT> iVector;

	//Find How many square in the mesh
	int SquareCount = Size * Size ; //Total amount of sqaures. 
	
	float TotalWidth = cellSize * Size;

	for (int square = 0; square < SquareCount; square++)
	{
		//Find relative x and y points
		int z = square / Size; //Rows
		int x = square - (z * Size); //Column on row

		//The offset the square has
		float xOffset = -TotalWidth / 2;
		float zOffset = TotalWidth / 2;


		//Read Height map values
		float readPercent = (float)square / SquareCount;
		int hMapSize = heightMapValue.size();
		if (hMapSize > SquareCount)
		{
			hMapSize = SquareCount;
		}
		int hMapIndex = hMapSize * readPercent;

		float thisHMapValue = heightMapValue[hMapIndex];

		//Create the points
		VERTEX V1;
		V1.Position = DirectX::XMFLOAT3(
			(x * cellSize) + xOffset,
			0 + (thisHMapValue * maxHeight),
			-(z * cellSize) + zOffset
		);
		V1.Normal = DirectX::XMFLOAT3(0, 1, 0);

		VERTEX V2;
		V2.Position = DirectX::XMFLOAT3(
			((x + 1) * cellSize) + xOffset,
			0 + (thisHMapValue * maxHeight),
			-(z * cellSize) + zOffset
		);
		V2.Normal = DirectX::XMFLOAT3(0, 1, 0);

		VERTEX V3;
		V3.Position = DirectX::XMFLOAT3(
			(x * cellSize) + xOffset,
			0 +(thisHMapValue * maxHeight),
			-(z + 1) * cellSize + zOffset
		);
		V3.Normal = DirectX::XMFLOAT3(0, 1, 0);

		VERTEX V4;
		V4.Position = DirectX::XMFLOAT3(
			(x + 1) * cellSize + xOffset,
			0 + (thisHMapValue * maxHeight),
			-(z + 1) * cellSize + zOffset);

		V4.Normal = DirectX::XMFLOAT3(0, 1, 0);


		int prevSquare = square - 1;
		if (prevSquare >= 0 && x > 0)
		{
			V1.Position.y = vVector[(prevSquare * 4) + 1].Position.y;
			V3.Position.y = vVector[(prevSquare * 4) + 3].Position.y;
		}
		int topSquare = square - Size;
		if (topSquare >= 0)
		{
			V1.Position.y = vVector[(topSquare * 4) + 2].Position.y;
			V2.Position.y = vVector[(topSquare * 4) + 3].Position.y;
		}

		XMVECTOR u = XMVectorSet(
			V2.Position.x - V1.Position.x,
			V2.Position.y - V1.Position.y,
			V2.Position.z - V1.Position.z,
			0
		);

		XMVECTOR v = XMVectorSet(
			V3.Position.x - V1.Position.x,
			V3.Position.y - V1.Position.y,
			V3.Position.z - V1.Position.z,
			0
		);

		XMVECTOR squareCrossProduct = XMVector3Cross(u, v);
		squareCrossProduct = XMVector3Normalize(squareCrossProduct);
		XMFLOAT3 squareNormal;

		XMStoreFloat3(&squareNormal, squareCrossProduct);
		V1.Normal = squareNormal;
		V2.Normal = squareNormal;
		V3.Normal = squareNormal;
		V4.Normal = squareNormal;

		//push back the points
		vVector.push_back(V1);
		vVector.push_back(V2);
		vVector.push_back(V3);
		vVector.push_back(V4);

		//Add indecies to the vector
		iVector.push_back((square * 4));
		iVector.push_back((square * 4) + 1);
		iVector.push_back((square * 4) + 2);

		iVector.push_back((square * 4) + 2);
		iVector.push_back((square * 4) + 1);
		iVector.push_back((square * 4) + 3);
	}

	VertexCount = vVector.size();
	IndeciesCount = iVector.size();

	D3D11_BUFFER_DESC vertexBufferDescriptor;
	vertexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDescriptor.ByteWidth = sizeof(VERTEX) * VertexCount;
	vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescriptor.CPUAccessFlags = 0;
	vertexBufferDescriptor.MiscFlags = 0;
	vertexBufferDescriptor.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexInitialisationData;
	vertexInitialisationData.pSysMem = &vVector[0];

	ThrowIfFailed(
		_parentDXDevice->GetDevice()->CreateBuffer(
			&vertexBufferDescriptor, &vertexInitialisationData,
			vertexBuffer.GetAddressOf()
		)
	);

	D3D11_BUFFER_DESC indexBufferDescriptor;
	indexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDescriptor.ByteWidth = sizeof(UINT) * IndeciesCount;
	indexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDescriptor.CPUAccessFlags = 0;
	indexBufferDescriptor.MiscFlags = 0;
	indexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the indices from
	D3D11_SUBRESOURCE_DATA indexInitialisationData;
	indexInitialisationData.pSysMem = &iVector[0];

	ThrowIfFailed(
		_parentDXDevice->GetDevice()->CreateBuffer(
			&indexBufferDescriptor, &indexInitialisationData,
			indexBuffer.GetAddressOf()
		)
	);

}

void TerrainNode::BuildShaders()
{
	//Compile Shaders and Set Vertex Layout
	_parentDXDevice = DirectXFramework::GetDXFramework();

	//Vertex Shader compile
	DWORD shaderCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	ComPtr<ID3D10Blob> vertexCompileMessages;
	ComPtr<ID3D10Blob> pixelCompileMessages;
	HRESULT hr;
	hr = D3DCompileFromFile(
		L"TexturedShaders.hlsl",
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VShader", "vs_5_0",
		shaderCompileFlags, 0,
		vertexShaderByteCode.GetAddressOf(),
		vertexCompileMessages.GetAddressOf()
	);
	ThrowIfFailed(hr);


	//Pixel Shader compile
	hr = D3DCompileFromFile(
		L"TexturedShaders.hlsl",
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PShader", "ps_5_0",
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
}

void TerrainNode::BuildVertexLayout()
{
	//Create Input Layout
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	HRESULT hr = _parentDXDevice->GetDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc),
		vertexShaderByteCode->GetBufferPointer(),
		vertexShaderByteCode->GetBufferSize(),
		vertexInputLayout.GetAddressOf()
	);

	ThrowIfFailed(hr);
}

void TerrainNode::BuildConstantBuffer()
{
	//Create Constant Buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ThrowIfFailed(
		_parentDXDevice->GetDevice()->CreateBuffer(
			&bufferDesc, NULL, constantBuffer.GetAddressOf()));
}

bool TerrainNode::LoadHeightMap(wstring fileName)
{
	//Get Size of file
	int streamSize = 0;
	std::streampos fSize = 0;
	std::ifstream fileStream(fileName, std::ios::binary);
	//Couldn't find or open file
	if (!fileStream)
	{
		return false;
	}

	fSize = fileStream.tellg();
	fileStream.seekg(0, std::ios::end);
	fSize = fileStream.tellg() - fSize;
	streamSize = fSize;

	fileStream.clear();
	fileStream.seekg(0, std::ios::beg);
	
	USHORT* rawValues = new USHORT[streamSize];
	fileStream.read((char*)rawValues, streamSize);
	fileStream.close();


	//normalize all values between 
	for (int i = 0; i < streamSize; i++)
	{
		heightMapValue.push_back((float)rawValues[i] / 65536);
	}
	delete[] rawValues;
	return true;
}
