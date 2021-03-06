#pragma once

#include "Panel.h"
#include "Buffer.h"

#include "Math/float2.h"
#include "Math/float4.h"

class ViewportPanel : public Panel
{
public:
	ViewportPanel();
	virtual ~ViewportPanel();

	void OnRender(FrameBuffer* frameBuffer);

	inline float2 const& GetViewportSize() const { return viewportSize; }

	float4 GetViewportDimensions();

private:
	float2 viewportSize = { 0,0 };

	float4 viewportDimensions;
};