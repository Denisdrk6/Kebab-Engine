#pragma once

#include "Component.h"

#include "Math/float3.h"
#include "Math/Quat.h"
#include "Math/float4x4.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform(GameObject& compOwner);
	~ComponentTransform();

	void Enable();
	void Disable();

	void Update();

	void SetPosition(const float3& pos);
	void SetRotation(const Quat& rot);
	void SetScale(const float3& scal);

private:

	float3 position;
	float3 scale;
	Quat rotation;

	float4x4 localTransformMat;
	float4x4 worldTransformMat;
};