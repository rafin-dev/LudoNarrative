#include "EditorLayer.h"

#include "Style/ImGuiThemeManager.h"

namespace Ludo {

	void EditorLayer::OnAttach()
	{
		LD_PROFILE_FUNCTION();

		ImGuiFontManager::Get()->Init();
		SetImGuiDarkTheme();

		m_ViewportSize.x = (float)Application::Get().GetWindow().GetWidth();
		m_ViewportSize.y = (float)Application::Get().GetWindow().GetHeight();

		FrameBufferSpecification fbSpec;
		fbSpec.Width = (uint32_t)m_ViewportSize.x;
		fbSpec.Height = (uint32_t)m_ViewportSize.y;

		m_FrameBuffer = FrameBuffer::Create(fbSpec);

		m_ActiveScene = CreateRef<Scene>();

#if 0
		m_Texture = Texture2D::Create("assets/textures/Checkerboard.png");

		m_ActiveScene->CreateEntity("Green Square").AddComponent<SpriteRendererComponent>(DirectX::XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f });
		m_ActiveScene->CreateEntity("Red Square").AddComponent<SpriteRendererComponent>().Color = { 1.0f, 0.0f, 0.0f, 1.0f };

		class CameraController : public ScriptableEntity
		{
		public:
			void OnCreate() override
			{
			}

			void OnDestroy() override
			{
			}

			void OnUpdate(TimeStep ts) override
			{
				auto& position = GetComponent<TransformComponent>().Translation;

				position.x += (Input::IsKeyPressed(KeyCode::D) - Input::IsKeyPressed(KeyCode::A)) * ts * 3;
				position.y += (Input::IsKeyPressed(KeyCode::W) - Input::IsKeyPressed(KeyCode::S)) * ts * 3;
			}
		};

		Entity cameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
		cameraEntity.AddComponent<CameraComponent>();
		cameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
#endif

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDetach()
	{
		LD_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(TimeStep ts)
	{
		LD_PROFILE_FUNCTION();

		if (m_ResizeFrameBuffer)
		{
			m_ResizeFrameBuffer = false;

			m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		Renderer2D::ResetStats();

		m_FrameBuffer->Bind();

		m_ActiveScene->OnUpdate(ts);

		m_FrameBuffer->Unbind();
	}

	void EditorLayer::OnEvent(Event& event)
	{
	}

	void EditorLayer::OnImGuiRender()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		float ogMinWinWidth = style.WindowMinSize.x;
		style.WindowMinSize.x = 340.0f;
		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
		style.WindowMinSize.x = ogMinWinWidth;

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Serialize"))
				{
					SceneSerializer serializer(m_ActiveScene);
					serializer.SerializeToYamlFile("assets/scenes/Example.LudoNarrative");
				}

				if (ImGui::MenuItem("Deserialize"))
				{
					SceneSerializer serializer(m_ActiveScene);
					serializer.DeserializeFromYamlFile("assets/scenes/Example.LudoNarrative");
				}

				if (ImGui::MenuItem("Exit")) { Application::Get().Close(); }

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		m_SceneHierarchyPanel.OnImGuiRender();

		ImGui::Begin("Stats");

		auto stats = Ludo::Renderer2D::GetStats();
		ImGui::Text("Renderer 2D Stats:");
		ImGui::Text(" Draw Calls: %d", stats.DrawCalls);
		ImGui::Text(" Quads     : %d", stats.QuadCount);
		ImGui::Text(" Vertices  : %d", stats.GetTotalVertexCount());
		ImGui::Text(" Indices   : %d", stats.GetTotalIndexCount());

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
		ImGui::Begin("Viewport");

		m_ViewportFocused = ImGui::IsWindowFocused();
		Application::Get().ImGuiBlockEvent(!ImGui::IsWindowHovered());

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ResizeFrameBuffer = viewportPanelSize.x != m_ViewportSize.x || viewportPanelSize.y != m_ViewportSize.y;
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		ImGui::Image(m_FrameBuffer->GetImTextureID(), viewportPanelSize);

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::SetImGuiDarkTheme()
	{
		ImGuiTheme theme;

		// Windows
		theme.Colors.WindowBackground = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		theme.Colors.Header = ImVec4{ 0.2f, 0.205f, 0.21, 1.0f };
		theme.Colors.HeaderHovered = ImVec4{ 0.3f, 0.305f, 0.32f, 1.0f };
		theme.Colors.HeaderActive = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		theme.Colors.Button = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		theme.Colors.ButtonHovered = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		theme.Colors.ButtonActive = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame Background
		theme.Colors.FrameBackground = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		theme.Colors.FrameBackgroundHovered = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		theme.Colors.FrameBackgroundActive = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		theme.Colors.Tab = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		theme.Colors.TabHovered = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		theme.Colors.TabActive = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		theme.Colors.TabUnfocused = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f }; 
		theme.Colors.TabUnfocusedActive = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		theme.Colors.TitleBackground = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		theme.Colors.TitleBackgroundActive = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		theme.Colors.TitleBackgroundCollapsed = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		CustomFontDescription fontDesc;
		fontDesc.SizeInPixels = 15.0f;
		fontDesc.RegularFont = "assets/fonts/roboto/roboto-Regular.ttf";
		fontDesc.BoldFont = "assets/fonts/roboto/Roboto-Bold.ttf";

		theme.Font = ImGuiFontManager::Get()->AddFont("Roboto", fontDesc);
		
		ImGuiThemeManager::AddTheme("Dark Theme", theme);
		ImGuiThemeManager::SetCurrentTheme("Dark Theme");
	}

}