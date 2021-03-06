#include "Camera.h"

Camera::Camera()
{
	fovVertical = 70.0f;
	fovHorizontal = 80.0f;

	planeFar = 200.f;
	planeNear = 0.1f;

	frustum.SetPerspective(fovHorizontal, fovVertical);

	frustum.SetFrame(vec(0, 0, 0), vec(0, 0, 1), vec(0, 1, 0));

	frustum.SetViewPlaneDistances(planeNear, planeFar);

	frustum.SetKind(math::FrustumProjectiveSpace::FrustumSpaceGL, math::FrustumHandedness::FrustumRightHanded);
}

Camera::~Camera()
{
}

void Camera::Look(const vec& lookPoint)
{
	if (lookPoint.IsFinite())
	{
		vec direction = lookPoint - frustum.Pos();

		float3x3 matrix = float3x3::LookAt(frustum.Front(), direction.Normalized(), frustum.Up(), float3(0, 1, 0));

		frustum.SetFrame(frustum.Pos(), matrix.MulDir(frustum.Front()).Normalized(), matrix.MulDir(frustum.Up()).Normalized());
	}
}

// SETTERS =============================================
void Camera::SetVerticalFov(const float& fovVert)
{
	frustum.SetVerticalFovAndAspectRatio(fovVert, 16 / 9);
}

void Camera::SetHorizontalFov(const float& fovHoriz)
{
	frustum.SetHorizontalFovAndAspectRatio(fovHoriz, 16 / 9);
}

void Camera::SetNearPlane(const float& nearPlane)
{
	frustum.SetViewPlaneDistances(nearPlane, planeFar);
	planeNear = nearPlane;
}

void Camera::SetFarPlane(const float& farPlane)
{
	frustum.SetViewPlaneDistances(planeNear, farPlane);
	planeFar = farPlane;
}

void Camera::SetCameraPosition(const vec& position)
{
	frustum.SetPos(position);
}

// GETTERS =============================================
float4x4 Camera::GetProjectionMatrix() const
{
	return frustum.ProjectionMatrix().Transposed();
}

float4x4 Camera::GetViewMatrix() const
{
	float4x4 mat = frustum.ViewMatrix();
	return mat.Transposed();
}

float Camera::GetVerticalFov() const
{
	return fovVertical;
}

float Camera::GetHorizontalFov() const
{
	return fovHorizontal;
}

float Camera::GetNearPlane() const
{
	return planeNear;
}

float Camera::GetFarPlane() const
{
	return planeFar;
}

vec Camera::GetCameraPosition() const
{
	return frustum.Pos();
}