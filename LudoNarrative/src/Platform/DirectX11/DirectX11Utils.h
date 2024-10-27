#pragma once

#define CHECK_AND_RELEASE(x) if (x != nullptr) { x->Release(); x = nullptr; }
