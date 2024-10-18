#include "ldpch.h"
#include "WindowsInput.h"

#include "Ludo/Log.h"

namespace Ludo {

    Input* Input::s_Instance = new WindowsInput();

    bool WindowsInput::IsKeyPressedImpl(int keyCode)
    {
        return GetKeyState(keyCode) < 0;
    }

    bool WindowsInput::IsMouseButtonDownImpl(int buttonCode)
    {
        return GetKeyState(buttonCode) < 0;
    }

    std::pair<float, float> WindowsInput::GetMousePositionImpl()
    {
        POINT pos;
        GetCursorPos(&pos);
        
        return std::make_pair<float, float>(pos.x, pos.y);
    }

    float WindowsInput::GetMouseXImpl()
    {
        POINT pos;
        GetCursorPos(&pos);

        return pos.x;
    }

    float WindowsInput::GetMouseYImpl()
    {
        POINT pos;
        GetCursorPos(&pos);

        return pos.y;
    }
}