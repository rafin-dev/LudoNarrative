#pragma once

#include "Ludo/Core/Core.h"
#include "Ludo/Assets/Asset.h"

#include <string>

#include <imgui.h>

namespace Ludo {

	class Texture : public Asset
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height);
		static Ref<Texture2D> Create(const std::filesystem::path& path);

		AssetType GetAssetType() const override { return AssetType::Texture2D; }

		virtual ImTextureID GetImTextureID() = 0;

		virtual bool operator==(const Texture2D& other) const = 0;
	};

}