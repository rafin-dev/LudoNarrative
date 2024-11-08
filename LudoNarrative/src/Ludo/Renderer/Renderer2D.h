#pragma once

namespace Ludo {

	class Renderer2D
	{
	public:
		virtual bool Init() = 0;
		virtual ~Renderer2D() = default;

		static Renderer2D* Get();
	};

}