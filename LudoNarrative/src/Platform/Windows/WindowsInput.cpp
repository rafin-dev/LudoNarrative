#include "ldpch.h"

#include "Ludo/Core/Input.h"

namespace Ludo {

    bool Input::IsKeyPressed(KeyCode keyCode)
    {
        return GetKeyState((int)keyCode) < 0;
    }

    bool Input::IsMouseButtonDown(MouseButtonCode buttonCode)
    {
        return GetKeyState((int)buttonCode) < 0;
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