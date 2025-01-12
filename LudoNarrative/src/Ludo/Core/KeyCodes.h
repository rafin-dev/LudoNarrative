#pragma once

// LudoNarrative KeyCodes
// Just Win32 keycodes, ignored a few that wouldn't be relevant for the engine

namespace Ludo {

	typedef enum class KeyCode : uint16_t
	{
		Space               = 0x20,
		Key_0               = 0x30,
		Key_1               = 0x31,
		Key_2               = 0x32,
		Key_3               = 0x33,
		Key_4               = 0x34,
		Key_5               = 0x35,
		Key_6               = 0x36,
		Key_7               = 0x37,
		Key_8               = 0x38,
		Key_9               = 0x39,
		A                   = 0x41,
		B                   = 0x42,
		C                   = 0x43,
		D                   = 0x44,
		E                   = 0x45,
		F                   = 0x46,
		G                   = 0x47,
		H                   = 0x48,
		I                   = 0x49,
		J                   = 0x4A,
		K                   = 0x4B,
		L                   = 0x4C,
		M                   = 0x4D,
		N                   = 0x4E,
		O                   = 0x4F,
		P                   = 0x50,
		Q                   = 0x51,
		R                   = 0x52,
		S                   = 0x53,
		T                   = 0x54,
		U                   = 0x55,
		V                   = 0x56,
		W                   = 0x57,
		X                   = 0x58,
		Y                   = 0x59,
		Z                   = 0x5A,
		NumPad_0            = 0x60,
		NumPad_1            = 0x61,
		NumPad_2            = 0x62,
		NumPad_3            = 0x63,
		NumPad_4            = 0x64,
		NumPad_5            = 0x65,
		NumPad_6            = 0x66,
		NumPad_7            = 0x67,
		NumPad_8            = 0x68,
		NumPad_9            = 0x69,
		MULTIPLY            = 0x6A,
		ADD                 = 0x6B,
		SEPARATOR           = 0x6C,
		SUBTRACT            = 0x6D,
		DECIMAL             = 0x6E,
		DIVIDE              = 0x6F,

		Backspace           = 0x08,
		Tab                 = 0x09,
		Clear               = 0x0C,
		Enter               = 0x0D,
		Shift               = 0x10,
		CTRL                = 0x11,
		ALT                 = 0x12,
		Pause               = 0x13,
		Caps_Lock           = 0x14,
		Escaoe              = 0x1B,
		Page_Up             = 0x21,
		Page_Down           = 0x22,
		End                 = 0x23,
		Home                = 0x24,
		Left_Arrow          = 0x25,
		Up_Arrow            = 0x26,
		Right_Arrow         = 0x27,
		Down_Arrow          = 0x28,
		Select              = 0x29,
		Print               = 0x2A,
		Execute             = 0x2B,
		Print_Screen        = 0x2C,
		Insert              = 0x2D,
		Delete              = 0x2E,
		Help                = 0x2F,
		Left_Windows_Key    = 0x5B,
		Left_Command_Key    = 0x5B,
		Left_Super_Key      = 0x5B,
		Right_Windows_Key   = 0x5C,
		Right_Command_Key   = 0x5C,
		Right_Super_Key     = 0x5C,

		F1					= 0x70,
		F2					= 0x71,
		F3					= 0x72,
		F4					= 0x73,
		F5					= 0x74,
		F6					= 0x75,
		F7					= 0x76,
		F8					= 0x77,
		F9					= 0x78,
		F10					= 0x79,
		F11					= 0x7A,
		F12					= 0x7B,
		F13					= 0x7C,
		F14					= 0x7D,
		F15					= 0x7E,
		F16					= 0x7F,
		F17					= 0x80,
		F18					= 0x81,
		F19					= 0x82,
		F20					= 0x83,
		F21					= 0x84,
		F22					= 0x85,
		F23					= 0x86,
		F24					= 0x87,

		Num_Lock			= 0x90,
		Scroll_Lock			= 0x91,
		Left_SHift			= 0xA0,
		Right_Shift			= 0xA1,
		Left_CTRL			= 0xA2,
		Right_CTRL			= 0xA3,
		Left_ALT			= 0xA4,
		Right_ALT			= 0xA5,
	};

	typedef enum class MouseButtonCode : uint16_t
	{
		Left   =  0x01,
		Right  =  0x02,
		Middle =  0x04,
		X1     =  0x05,
		X2     =  0x06
	};

}

template<>
struct fmt::formatter<Ludo::KeyCode> : fmt::formatter<std::string>
{
	auto format(Ludo::KeyCode& my, format_context& ctx) const -> decltype(ctx.out())
	{
		return fmt::format_to(ctx.out(), "{}", static_cast<int32_t>(my));
	}
};

// Mouse Buttons
#define LD_MOUSE_BUTTON_LEFT        0x01
#define LD_MOUSE_BUTTON_RIGHT       0x02
#define LD_MOUSE_BUTTON_MIDDLE      0x04 
#define LD_MOUSE_BUTTON_X1          0x05
#define LD_MOUSE_BUTTON_X2          0x06

// Function keys
#define LD_KEY_BACKSPACE            ::Ludo::KeyCode::Space
#define LD_KEY_TAB                  ::Ludo::KeyCode::Tab
#define LD_KEY_CLEAR                ::Ludo
#define LD_KEY_ENTER                0x0D
#define LD_KEY_SHIFT                0x10
#define LD_KEY_CTRL                 0x11
#define LD_KEY_ALT                  0x12
#define LD_KEY_PAUSE                0x13
#define LD_KEY_CAPS_LOCK            0x14
#define LD_KEY_ESCAPE               0x1B
#define LD_KEY_PAGE_UP              0x21
#define LD_KEY_PAGE_DOWN            0x22
#define LD_KEY_END                  0x23
#define LD_KEY_HOME                 0x24
#define LD_KEY_LEFT_ARROW           0x25
#define LD_KEY_UP_ARROW             0x26
#define LD_KEY_RIGHT_ARROW          0x27
#define LD_KEY_DOWN_ARROW           0x28
#define LD_KEY_SELECT               0x29
#define LD_KEY_PRINT                0x2A
#define LD_KEY_EXECUTE              0x2B
#define LD_KEY_PRINT_SCREEN         0x2C
#define LD_KEY_INSERT               0x2D
#define LD_KEY_DELETE               0x2E
#define LD_KEY_HELP                 0x2F
#define LD_LEFT_WINDOWS_KEY         0x5B
#define LD_LEFT_COMMAND_KEY         0x5B
#define LD_LEFT_SUPER_KEY           0x5B
#define LD_RIGHT_WINDOWS_KEY        0x5C
#define LD_RIGHT_COMMAND_KEY        0x5C
#define LD_RIGHT_SUPER_KEY          0x5C
#define LD_KEY_F1                   0x70
#define LD_KEY_F2                   0x71
#define LD_KEY_F3                   0x72
#define LD_KEY_F4                   0x73
#define LD_KEY_F5                   0x74
#define LD_KEY_F6                   0x75
#define LD_KEY_F7                   0x76
#define LD_KEY_F8                   0x77
#define LD_KEY_F9                   0x78
#define LD_KEY_F10                  0x79
#define LD_KEY_F11                  0x7A
#define LD_KEY_F12                  0x7B
#define LD_KEY_F13                  0x7C
#define LD_KEY_F14                  0x7D
#define LD_KEY_F15                  0x7E
#define LD_KEY_F16                  0x7F
#define LD_KEY_F17                  0x80
#define LD_KEY_F18                  0x81
#define LD_KEY_F19                  0x82
#define LD_KEY_F20                  0x83
#define LD_KEY_F21                  0x84
#define LD_KEY_F22                  0x85
#define LD_KEY_F23                  0x86
#define LD_KEY_F24                  0x87
#define LD_KEY_NUM_LOCK             0x90
#define LD_KEY_SCROLL_LOCK          0x91
#define LD_KEY_LEFT_SHIFT           0xA0
#define LD_KEY_RIGHT_SHIFT          0xA1
#define LD_KEY_LEFT_CTRL            0xA2
#define LD_KEY_RIGHT_CTRL           0xA3
#define LD_KEY_LEFT_ALT             0xA4
#define LD_KEY_RIGHT_ALT            0xA5

// Printable keys
#define LD_KEY_SPACE                0x20
#define LD_KEY_0                    0x30
#define LD_KEY_1                    0x31
#define LD_KEY_2                    0x32
#define LD_KEY_3                    0x33
#define LD_KEY_4                    0x34
#define LD_KEY_5                    0x35
#define LD_KEY_6                    0x36
#define LD_KEY_7                    0x37
#define LD_KEY_8                    0x38
#define LD_KEY_9                    0x39
#define LD_KEY_A                    0x41
#define LD_KEY_B                    0x42
#define LD_KEY_C                    0x43
#define LD_KEY_D                    0x44
#define LD_KEY_E                    0x45
#define LD_KEY_F                    0x46
#define LD_KEY_G                    0x47
#define LD_KEY_H                    0x48
#define LD_KEY_I                    0x49
#define LD_KEY_J                    0x4A
#define LD_KEY_K                    0x4B
#define LD_KEY_L                    0x4C
#define LD_KEY_M                    0x4D
#define LD_KEY_N                    0x4E
#define LD_KEY_O                    0x4F
#define LD_KEY_P                    0x50
#define LD_KEY_Q                    0x51
#define LD_KEY_R                    0x52
#define LD_KEY_S                    0x53
#define LD_KEY_T                    0x54
#define LD_KEY_U                    0x55
#define LD_KEY_V                    0x56
#define LD_KEY_W                    0x57
#define LD_KEY_X                    0x58
#define LD_KEY_Y                    0x59
#define LD_KEY_Z                    0x5A
#define LD_NUMPAD_0                 0x60
#define LD_NUMPAD_1                 0x61
#define LD_NUMPAD_2                 0x62
#define LD_NUMPAD_3                 0x63
#define LD_NUMPAD_4                 0x64
#define LD_NUMPAD_5                 0x65
#define LD_NUMPAD_6                 0x66
#define LD_NUMPAD_7                 0x67
#define LD_NUMPAD_8                 0x68
#define LD_NUMPAD_9                 0x69
#define LD_KEY_MULTIPLY             0x6A
#define LD_KEY_ADD                  0x6B
#define LD_KEY_SEPARATOR            0x6C
#define LD_KEY_SUBTRACT             0x6D
#define LD_KEY_DECIMAL              0x6E
#define LD_KEY_DIVIDE               0x6F
