#pragma once
#include "DirectXFramework.h"



float AngleToRadians(float Angle)
{
	return Angle * (XM_PI / 180);
}

class Graphics2 : public DirectXFramework
{
public:
	void CreateSceneGraph();
	void UpdateSceneGraph();
private:
	float _CamSpeedScale = 5;
	float _CamRollSpeed = 40;
	float _CamRotateSpeed = 20;
	float _CamMovSpeed = 100; 
	//Alter the camera depending on key input
	void CameraInput();
	float Angle = 0; 

	DirectXFramework* dxFramework;

	shared_ptr<SceneNode> planePointer;
	shared_ptr<SceneNode> cubePointer;
	shared_ptr<SceneNode> terrainPointer;

	shared_ptr<Camera> _camera;
};

