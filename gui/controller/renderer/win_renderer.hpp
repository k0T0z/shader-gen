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

#ifndef SHADER_GEN_WIN_RENDERER_HPP
#define SHADER_GEN_WIN_RENDERER_HPP

#define NOMINMAX // Prevent Windows.h from defining min() and max() macros

#include <QWidget>
#include <d3d11.h>
#include <QTimer>
#include <QElapsedTimer>
#include <string>

class QShowEvent;
class QHideEvent;
class QResizeEvent;

class WinRendererWidget : public QWidget {
    Q_OBJECT

public:
    explicit WinRendererWidget(QWidget* parent = nullptr);
    ~WinRendererWidget() override;

    /**
     * @brief Sets the HLSL pixel shader code (body of the main function).
     *        The code should compute a float4 color using input.FragCoord and uTime.
     * @param code The shader code as a string.
     */
    void set_code(const std::string& code);

Q_SIGNALS:
    void scene_update_requested();

private Q_SLOTS:
    void update_shader_program();
    void render();

private:
    // DirectX resources
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11InputLayout* inputLayout = nullptr;
    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11Buffer* constantBuffer = nullptr;

    // Timers and state
    QTimer renderTimer;
    QTimer compileDebounceTimer;
    QElapsedTimer elapsedTimer;
    std::string code;

    void initializeDirectX();
    void cleanup();
    void initBuffers();
    void resizeSwapChain(int w, int h);

    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
};

#endif // SHADER_GEN_WIN_RENDERER_HPP
