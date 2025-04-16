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
#include "Ludo/Core/Core.h"
#include "Ludo/Core/Log.h"
#include "Ludo/Utils/PlatformUtils.h"

#include "Ludo/Events/Event.h"
#include "Ludo/Core/Input.h"
#include "Ludo/Core/KeyCodes.h"
#include "Ludo/Core/TimeStep.h"

#include "Ludo/Scene/Scene.h"
#include "Ludo/Scene/SceneSerializer.h"
#include "Ludo/Scene/Entity.h"
#include "Ludo/Scene/ScriptableEntity.h"
#include "Ludo/Scene/Components.h"

#include "Ludo/Project/Project.h"

#include "Ludo/Assets/Asset.h"
#include "Ludo/Assets/AssetImporter.h"
#include "Ludo/Assets/AssetManager.h"
#include "Ludo/Assets/AssetMetadata.h"

// ========== Renderer ==========
#include "Ludo/Renderer/Renderer.h"
#include "Ludo/Renderer/Renderer2D.h"
#include "Ludo/Renderer/RenderCommand.h"
#include "Ludo/Renderer/FrameBuffer.h"

#include "Ludo/Renderer/Shader.h"
#include "Ludo/Renderer/Buffer.h"
#include "Ludo/Renderer/VertexArray.h"
#include "Ludo/Renderer/Texture.h"
#include "Ludo/Renderer/SubTexture2D.h"

#include "Ludo/Renderer/EditorCamera.h"
// ==============================