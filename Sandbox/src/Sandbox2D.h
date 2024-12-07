#pragma once

#include <LudoEngine.h>

class Sandbox2D : public Ludo::Layer
{
public:
	Sandbox2D();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Ludo::TimeStep timeStep) override;
	virtual void OnEvent(Ludo::Event& event) override;
	virtual void OnImGuiRender() override;

private:
	Ludo::OrthographicCameraController m_CameraController;

};