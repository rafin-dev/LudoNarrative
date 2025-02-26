#pragma once

#include <LudoNarrative.h>

#include <vector>

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

	Ludo::Ref<Ludo::FrameBuffer> m_FrameBuffer;

	DirectX::XMFLOAT2 m_Position = {};
	DirectX::XMFLOAT2 m_Size = { 1.0f, 1.0f };
	float m_Rotation = 0.0f;

	DirectX::XMFLOAT4 m_Color = { 1.0f, 1.0f, 1.0f, 1.0f };

	Ludo::Ref<Ludo::Texture2D> m_Texture;
	Ludo::Ref<Ludo::ImGuiTexture> m_ImGuiTexture;

	Ludo::Ref<Ludo::Texture2D> m_SpriteSheet;
	Ludo::Ref<Ludo::SubTexture2D> m_StairsTexture;
	Ludo::Ref<Ludo::SubTexture2D> m_TreeTexture;

	float m_LastDeltaTime = 0.0f;
};