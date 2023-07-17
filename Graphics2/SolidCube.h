#pragma once
#include "SceneNode.h"
#include "DirectXFramework.h"

//Vertex Defenition
struct Vertex
{
	XMFLOAT3 Position;
	XMFLOAT4 Colour;
};

struct CBuffer
{
	XMMATRIX world_view_projectionMatrix;
};

class SolidCube : public SceneNode
{
public:
	SolidCube(wstring ObjectName);
	bool Initialise() { return true; }
	void Update(FXMMATRIX& currentWorldTransformation);
	void Render();
	void Shutdown(){}
private:
	//GPU that holds this object
	DirectXFramework* _parentDXDevice;

	ComPtr <ID3D11InputLayout> vertexInputLayout;
	ComPtr<ID3D10Blob> vertexShaderByteCode;
	ComPtr<ID3D10Blob> pixelShaderByteCode;

	ComPtr <ID3D11Buffer> vertexBuffer;
	ComPtr <ID3D11Buffer> indexBuffer;
	ComPtr <ID3D11Buffer> constantBuffer;


	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;
};

