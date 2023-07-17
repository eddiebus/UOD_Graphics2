#include "Graphics2.h"
#include "SolidCube.h"
#include "MeshNode.h"
#include "TerrainNode.h"
Graphics2 app;



void Graphics2::CreateSceneGraph()
{
	_camera = GetDXFramework()->GetCamera();
	GetDXFramework()->GetCamera()->SetCameraPosition(0.0f, 50.0f, -500.0f);

	SceneGraphPointer sceneGraph = GetSceneGraph();
	// This is where you add nodes to the scene graph

	cubePointer = make_shared<SolidCube>(L"SomeCube");
	sceneGraph->Add(cubePointer);

	planePointer = make_shared<MeshNode>(L"Plane1", L"Plane_Model\\Bonanza.3DS");
	sceneGraph->Add(planePointer);

	terrainPointer = make_shared<TerrainNode>(L"TEst");
	sceneGraph->Add(terrainPointer);

}

void Graphics2::UpdateSceneGraph()
{
	dxFramework = GetDXFramework();
	double time = DirectXFramework::GetDXFramework()->GetDeltaTime();
	Angle += float(time);
	if (Angle > 360) { Angle = 0; }

	XMMATRIX planeTransform = DirectX::XMMatrixIdentity();
	planeTransform *= DirectX::XMMatrixScaling(1, 1, 1);
	planeTransform *= DirectX::XMMatrixRotationX(90);
	planeTransform *= DirectX::XMMatrixRotationY(Angle);

	XMMATRIX cubeTransform = DirectX::XMMatrixIdentity();
	cubeTransform = DirectX::XMMatrixScaling(2, 2, 2);
	cubeTransform *= DirectX::XMMatrixRotationY(-Angle) * DirectX::XMMatrixRotationZ(Angle);
	
	planePointer->SetWorldTransform(planeTransform);
	cubePointer->SetWorldTransform(cubeTransform);
	terrainPointer->SetWorldTransform(DirectX::XMMatrixIdentity());

	CameraInput();
}

void Graphics2::CameraInput()
{

	if (!dxFramework->GetWindowFocus())
	{
		return;
	}

	//W Key 
	if (GetAsyncKeyState(0x57) < 0)
	{
		_camera->SetForwardBack(_CamMovSpeed * _CamSpeedScale * dxFramework->GetDeltaTime());
	}
	//S Key
	else if (GetAsyncKeyState(0x53) < 0)
	{
		_camera->SetForwardBack(-_CamMovSpeed * _CamSpeedScale * dxFramework->GetDeltaTime());
	}


	//A Key
	if (GetAsyncKeyState(0x41) < 0)
	{
		_camera->SetLeftRight(- _CamMovSpeed * _CamSpeedScale * dxFramework->GetDeltaTime());
	}
	//D Key
	else if (GetAsyncKeyState(0x44) < 0)
	{
		_camera->SetLeftRight(_CamMovSpeed * _CamSpeedScale * dxFramework->GetDeltaTime());
	}

	if (GetAsyncKeyState(VK_UP) < 0)
	{
		_camera->SetPitch(-_CamRotateSpeed * _CamSpeedScale * dxFramework->GetDeltaTime());
	}
	else if (GetAsyncKeyState(VK_DOWN) < 0)
	{
		_camera->SetPitch(_CamRotateSpeed * _CamSpeedScale * dxFramework->GetDeltaTime());
	}

	if (GetAsyncKeyState(VK_LEFT) < 0)
	{
		_camera->SetYaw(-_CamRotateSpeed * _CamSpeedScale * dxFramework->GetDeltaTime());
	}
	else if (GetAsyncKeyState(VK_RIGHT) < 0)
	{
		_camera->SetYaw(_CamRotateSpeed * _CamSpeedScale * dxFramework->GetDeltaTime());
	}

	//Q Key roll
	if (GetAsyncKeyState(0x51) < 0)
	{
		_camera->SetRoll(_CamRollSpeed * _CamSpeedScale * dxFramework->GetDeltaTime());
	}
	//E Key Roll
	else if (GetAsyncKeyState(0x45) < 0)
	{
		_camera->SetRoll(-_CamRollSpeed * _CamSpeedScale * dxFramework->GetDeltaTime());
	}
}
