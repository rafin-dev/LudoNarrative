#include "ldpch.h"

#include "Ludo/Core/Application.h"
#include "Ludo/Core/Input.h"

#include "Platform/Windows/WindowsWindow.h"

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
        
        return std::make_pair<float, float>((float)pos.x, (float)pos.y);
    }

    float Input::GetMouseX()
    {
        POINT pos;
        GetCursorPos(&pos);

        return (float)pos.x;
    }

    float Input::GetMouseY()
    {
        POINT pos;
        GetCursorPos(&pos);

        return (float)pos.y;
    }

    void Input::SetMousePos(int x, int y)
    {
        RECT windowRect;
        GetWindowRect(static_cast<WindowsWindow&>(Application::Get().GetWindow()).GetHandle(), &windowRect);

        SetCursorPos(windowRect.left + x, windowRect.top + y);
    }
}