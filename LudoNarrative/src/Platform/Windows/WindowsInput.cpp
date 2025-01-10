#include "ldpch.h"

#include "Ludo/Core/Input.h"

#include "Ludo/Core/KeyCodes.h"

namespace Ludo {

    bool Input::IsKeyPressed(int keyCode)
    {
        return GetKeyState(keyCode) < 0;
    }

    bool Input::IsMouseButtonDown(int buttonCode)
    {
        return GetKeyState(buttonCode) < 0;
    }

    std::pair<float, float> Input::GetMousePosition()
    {
        POINT pos;
        GetCursorPos(&pos);
        
        return std::make_pair<float, float>(pos.x, pos.y);
    }

    float Input::GetMouseX()
    {
        POINT pos;
        GetCursorPos(&pos);

        return pos.x;
    }

    float Input::GetMouseY()
    {
        POINT pos;
        GetCursorPos(&pos);

        return pos.y;
    }
}