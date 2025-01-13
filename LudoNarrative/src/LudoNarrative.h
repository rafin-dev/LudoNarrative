#pragma once
//
//  /$$                       /$$           /$$   /$$                                          /$$     /$$                    
// | $$                      | $$          | $$$ | $$                                         | $$    |__/                    
// | $$       /$$   /$$  /$$$$$$$  /$$$$$$ | $$$$| $$  /$$$$$$   /$$$$$$   /$$$$$$  /$$$$$$  /$$$$$$   /$$ /$$    /$$ /$$$$$$ 
// | $$      | $$  | $$ /$$__  $$ /$$__  $$| $$ $$ $$ |____  $$ /$$__  $$ /$$__  $$|____  $$|_  $$_/  | $$|  $$  /$$//$$__  $$
// | $$      | $$  | $$| $$  | $$| $$  \ $$| $$  $$$$  /$$$$$$$| $$  \__/| $$  \__/ /$$$$$$$  | $$    | $$ \  $$/$$/| $$$$$$$$
// | $$      | $$  | $$| $$  | $$| $$  | $$| $$\  $$$ /$$__  $$| $$      | $$      /$$__  $$  | $$ /$$| $$  \  $$$/ | $$_____/
// | $$$$$$$$|  $$$$$$/|  $$$$$$$|  $$$$$$/| $$ \  $$|  $$$$$$$| $$      | $$     |  $$$$$$$  |  $$$$/| $$   \  $/  |  $$$$$$$
// |________/ \______/  \_______/ \______/ |__/  \__/ \_______/|__/      |__/      \_______/   \___/  |__/    \_/    \_______/
//

// ========== Core ==========
#include "Ludo/Core/Application.h"
#include "Ludo/Core/Layer.h"
#include "Ludo/Core/Log.h"

#include "Ludo/Core/Input.h"
#include "Ludo/Core/KeyCodes.h"
#include "Ludo/Core/TimeStep.h"

#include "Ludo/Renderer/OrthographicCameraController.h"

#include "Ludo/Scene/Scene.h"
#include "Ludo/Scene/Entity.h"
#include "Ludo/Scene/ScriptableEntity.h"
#include "Ludo/Scene/Components.h"

// ========== Renderer ==========
#include "Ludo/Renderer/Renderer.h"
#include "Ludo/Renderer/Renderer2D.h"
#include "Ludo/Renderer/RenderCommand.h"
#include "Ludo/Renderer/FrameBuffer.h"

#include "Ludo/Renderer/Shader.h"
#include "Ludo/Renderer/Buffer.h"
#include "Ludo/Renderer/VertexArray.h"
#include "Ludo/Renderer/Material.h"
#include "Ludo/Renderer/Texture.h"
#include "Ludo/Renderer/SubTexture2D.h"
#include "Ludo/Renderer/ImGuiTexture.h"

#include "Ludo/Renderer/OrthographicCamera.h"
#include "Ludo/Renderer/Transform.h"
// ==============================