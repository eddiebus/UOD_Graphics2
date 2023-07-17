#pragma once
#include "SceneNode.h"
#include "ResourceManager.h"
#include "DirectXFramework.h"
#include <vector>

class TerrainNode :
    public SceneNode
{
public:
    TerrainNode(wstring ObjectName);
    bool Initialise() { return true; }
    void Update(FXMMATRIX& currentWorldTransformation) 
    {
        XMStoreFloat4x4(&_combinedWorldTransformation, XMMatrixIdentity() * currentWorldTransformation);
    };
    void Render();
    void Shutdown() {}

private:
    bool LoadHeightMap(wstring fileName);
    std::vector<float> heightMapValue;


    void CreateMesh();
    void BuildShaders();
    void BuildVertexLayout();
    void BuildConstantBuffer();
    void BuildRenderState();

    XMFLOAT4			_ambientLight;
    XMFLOAT4			_directionalLightVector;
    XMFLOAT4			_directionalLightColour;
    XMFLOAT4			_cameraPosition;


    DirectXFramework* _parentDXDevice;
    //Raster States
    ComPtr<ID3D11RasterizerState> RasterState;
    ComPtr<ID3D11RasterizerState> OriginalRasterState;

    //InputLayout
    ComPtr <ID3D11InputLayout> vertexInputLayout;
    ComPtr<ID3D10Blob> vertexShaderByteCode;
    ComPtr<ID3D10Blob> pixelShaderByteCode;

    //Buffers
    ComPtr <ID3D11Buffer> vertexBuffer;
    ComPtr <ID3D11Buffer> indexBuffer;
    ComPtr <ID3D11Buffer> constantBuffer;

    //Shaders
    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11PixelShader> pixelShader;

    //Terrain Info
    int Size = 1024;
    float cellSize = 5;
    float maxHeight = 1000;
    UINT VertexCount = 0;
    UINT IndeciesCount = 0;
};

