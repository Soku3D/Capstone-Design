#pragma once
#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxtk/Keyboard.h>
#include <directxtk/Mouse.h>
#include <directxtk/SimpleMath.h>
#include <dxgi.h>

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <io.h>
#include <wrl.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>
#include <random>

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;

#include "Vertex.h"
