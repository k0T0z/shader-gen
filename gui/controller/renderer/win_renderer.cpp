/*********************************************************************************/
/*                                                                               */
/*  Copyright (C) 2024 Seif Kandil (k0T0z)                                       */
/*                                                                               */
/*  This file is a part of the ENIGMA Development Environment.                   */
/*                                                                               */
/*                                                                               */
/*  ENIGMA is free software: you can redistribute it and/or modify it under the  */
/*  terms of the GNU General Public License as published by the Free Software    */
/*  Foundation, version 3 of the license or any later version.                   */
/*                                                                               */
/*  This application and its source code is distributed AS-IS, WITHOUT ANY       */
/*  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS    */
/*  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more        */
/*  details.                                                                     */
/*                                                                               */
/*  You should have recieved a copy of the GNU General Public License along      */
/*  with this code. If not, see <http://www.gnu.org/licenses/>                   */
/*                                                                               */
/*  ENIGMA is an environment designed to create games and other programs with a  */
/*  high-level, fully compilable language. Developers of ENIGMA or anything      */
/*  associated with ENIGMA are in no way responsible for its users or            */
/*  applications created by its users, or damages caused by the environment      */
/*  or programs made in the environment.                                         */
/*                                                                               */
/*********************************************************************************/

#include "gui/controller/renderer/win_renderer.hpp"
#include <QResizeEvent>
#include <d3dcompiler.h>
#include "error_macros.hpp"

// Helper macro for error checking
#define CHECK_HR(hr, msg)                                                                                    \
  if (FAILED(hr)) {                                                                                           \
    shadergen_logger::win_renderer_debug_print(__FUNCTION__, __FILE__, __LINE__, hr, msg);                     \
    return;                                                                                                  \
  } else                                                                                                     \
    ((void)0)

WinRendererWidget::WinRendererWidget(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_PaintOnScreen, true);  // Disable Qt's default painting

    renderTimer.setInterval(32);  // ~30 FPS
    connect(&renderTimer, &QTimer::timeout, this, &WinRendererWidget::render);
    connect(&renderTimer, &QTimer::timeout, this, &WinRendererWidget::scene_update_requested);

    compileDebounceTimer.setSingleShot(true);
    compileDebounceTimer.setInterval(300);  // 300ms debounce delay
    connect(&compileDebounceTimer, &QTimer::timeout, this, &WinRendererWidget::update_shader_program);
}

WinRendererWidget::~WinRendererWidget() {
    cleanup();
}

void WinRendererWidget::set_code(const std::string& new_code) {
    if (this->code == new_code) return;
    this->code = new_code;
    if (!compileDebounceTimer.isActive()) compileDebounceTimer.start();
}

void WinRendererWidget::initializeDirectX() {
    HWND hwnd = reinterpret_cast<HWND>(winId());

    // Create swap chain description
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Width = width();
    scd.BufferDesc.Height = height();
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;

    // Create device and swap chain
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
        D3D11_SDK_VERSION, &scd, &swapChain, &device, nullptr, &context
    );
    CHECK_HR(hr, "Failed to create DirectX device and swap chain");

    // Create render target view
    ID3D11Texture2D* backBuffer = nullptr;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
    CHECK_HR(hr, "Failed to get back buffer");
    hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
    backBuffer->Release();
    CHECK_HR(hr, "Failed to create render target view");

    // Compile vertex shader
    const char* vertexShaderSource = R"(
struct VS_INPUT {
    float2 aPos : POSITION;
    float2 aFragCoord : TEXCOORD0;
};
struct VS_OUTPUT {
    float4 Position : SV_POSITION;
    float2 FragCoord : TEXCOORD0;
};
VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
    output.Position = float4(input.aPos, 0.0, 1.0);
    output.FragCoord = input.aFragCoord;
    return output;
}
    )";

    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    hr = D3DCompile(vertexShaderSource, strlen(vertexShaderSource), nullptr, nullptr, nullptr,
                    "main", "vs_5_0", 0, 0, &vsBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            std::cerr << "Vertex shader compilation failed: " << static_cast<char*>(errorBlob->GetBufferPointer()) << std::endl;
            errorBlob->Release();
        }
        return;
    }
    if (errorBlob) errorBlob->Release();

    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    CHECK_HR(hr, "Failed to create vertex shader");

    // Create input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    hr = device->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
    vsBlob->Release();
    CHECK_HR(hr, "Failed to create input layout");

    // Create constant buffer for uTime
    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.ByteWidth = sizeof(float);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = device->CreateBuffer(&cbd, nullptr, &constantBuffer);
    CHECK_HR(hr, "Failed to create constant buffer");

    // Initialize buffers and pixel shader
    initBuffers();
    update_shader_program();
}

void WinRendererWidget::initBuffers() {
    const float vertices[] = {
        -1.0f,  1.0f,  0.0f,  1.0f,  // Vertex 1: Position (x,y), FragCoord (u,v)
        -1.0f, -1.0f,  0.0f,  0.0f,  // Vertex 2: Position (x,y), FragCoord (u,v)
         1.0f,  1.0f,  1.0f,  1.0f,  // Vertex 3: Position (x,y), FragCoord (u,v)
         1.0f, -1.0f,  1.0f,  0.0f   // Vertex 4: Position (x,y), FragCoord (u,v)
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = vertices;

    HRESULT hr = device->CreateBuffer(&bd, &sd, &vertexBuffer);
    CHECK_HR(hr, "Failed to create vertex buffer");
}

void WinRendererWidget::update_shader_program() {
    std::string pixelShaderSource;
    if (code.empty()) {
        pixelShaderSource = R"(
struct PS_INPUT {
    float4 Position : SV_POSITION;
    float2 FragCoord : TEXCOORD0;
};
cbuffer Constants : register(b0) {
    float uTime;
};
float4 main(PS_INPUT input) : SV_TARGET {
    return float4(0.0, 0.0, 0.0, 1.0);
}
        )";
    } else {
        pixelShaderSource = R"(
struct PS_INPUT {
    float4 Position : SV_POSITION;
    float2 FragCoord : TEXCOORD0;
};
cbuffer Constants : register(b0) {
    float uTime;
};
float4 main(PS_INPUT input) : SV_TARGET {
)" + code + R"(
}
        )";
    }

    ID3DBlob* psBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompile(pixelShaderSource.c_str(), pixelShaderSource.size(), nullptr, nullptr, nullptr,
                            "main", "ps_5_0", 0, 0, &psBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            std::cerr << "Pixel shader compilation failed: " << static_cast<char*>(errorBlob->GetBufferPointer()) << std::endl;
            errorBlob->Release();
        }
        return;  // Keep the old shader if compilation fails
    }
    if (errorBlob) errorBlob->Release();

    // Release the old pixel shader
    if (pixelShader) {
        pixelShader->Release();
        pixelShader = nullptr;
    }

    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
    psBlob->Release();
    CHECK_HR(hr, "Failed to create pixel shader");
}

void WinRendererWidget::render() {
    if (!device) return;

    // Update constant buffer with elapsed time
    float time = elapsedTimer.elapsed() * 0.001f;  // Convert ms to seconds
    D3D11_MAPPED_SUBRESOURCE ms;
    HRESULT hr = context->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
    CHECK_HR(hr, "Failed to map constant buffer");
    memcpy(ms.pData, &time, sizeof(float));
    context->Unmap(constantBuffer, 0);

    // Clear the render target
    float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    context->ClearRenderTargetView(renderTargetView, clearColor);

    // Set the render target
    context->OMSetRenderTargets(1, &renderTargetView, nullptr);

    // Set the viewport
    D3D11_VIEWPORT vp = {0, 0, static_cast<float>(width()), static_cast<float>(height()), 0, 1};
    context->RSSetViewports(1, &vp);

    // Set the input layout and vertex buffer
    context->IASetInputLayout(inputLayout);
    UINT stride = 4 * sizeof(float);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

    // Set primitive topology
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // Set shaders
    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);

    // Set constant buffer
    context->PSSetConstantBuffers(0, 1, &constantBuffer);

    // Draw the quad
    context->Draw(4, 0);

    // Present the frame
    swapChain->Present(0, 0);
}

void WinRendererWidget::showEvent(QShowEvent* event) {
    if (!device) {
        initializeDirectX();
    }
    renderTimer.start();
    elapsedTimer.start();
    QWidget::showEvent(event);
}

void WinRendererWidget::hideEvent(QHideEvent* event) {
    renderTimer.stop();
    elapsedTimer.invalidate();
    QWidget::hideEvent(event);
}

void WinRendererWidget::resizeEvent(QResizeEvent* event) {
    if (device && renderTargetView) {
        resizeSwapChain(event->size().width(), event->size().height());
    }
    QWidget::resizeEvent(event);
}

void WinRendererWidget::resizeSwapChain(int w, int h) {
    if (!swapChain || !context || !renderTargetView) return;

    // Release the old render target view
    renderTargetView->Release();
    renderTargetView = nullptr;

    // Resize the swap chain
    HRESULT hr = swapChain->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, 0);
    CHECK_HR(hr, "Failed to resize swap chain");

    // Recreate the render target view
    ID3D11Texture2D* backBuffer = nullptr;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
    CHECK_HR(hr, "Failed to get back buffer after resize");
    hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
    backBuffer->Release();
    CHECK_HR(hr, "Failed to create render target view after resize");
}

void WinRendererWidget::cleanup() {
    if (vertexBuffer) { vertexBuffer->Release(); vertexBuffer = nullptr; }
    if (inputLayout) { inputLayout->Release(); inputLayout = nullptr; }
    if (vertexShader) { vertexShader->Release(); vertexShader = nullptr; }
    if (pixelShader) { pixelShader->Release(); pixelShader = nullptr; }
    if (constantBuffer) { constantBuffer->Release(); constantBuffer = nullptr; }
    if (renderTargetView) { renderTargetView->Release(); renderTargetView = nullptr; }
    if (swapChain) { swapChain->Release(); swapChain = nullptr; }
    if (context) { context->Release(); context = nullptr; }
    if (device) { device->Release(); device = nullptr; }
}
