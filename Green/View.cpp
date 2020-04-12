#include "View.h"
#include <boost/bind.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define RGB_ON  0xFFFFFFFFL
#define RGB_OFF 0x00000000L

ID3D11Device* View::g_pd3dDevice = NULL;
ID3D11DeviceContext* View::g_pd3dDeviceContext = NULL;
IDXGISwapChain* View::g_pSwapChain = NULL;
ID3D11RenderTargetView* View::g_mainRenderTargetView = NULL;
unsigned char* View::buffer8888 = (unsigned char*)malloc(4 * 224 * 256);
Machine* View::machine = new Machine();

bool View::LoadTextureFromData(unsigned char* data, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
    // Load from disk into a raw RGBA buffer
    int image_width = 224;
    int image_height = 256;
    unsigned char* image_data = data;// stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create texture
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    ID3D11Texture2D* pTexture = NULL;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

    // Create texture view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
    pTexture->Release();

    *out_width = image_width;
    *out_height = image_height;

    return true;
}

// Simple helper function to load an image into a DX11 texture with common settings
bool View::LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
    // Load from disk into a raw RGBA buffer
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create texture
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    ID3D11Texture2D* pTexture = NULL;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

    // Create texture view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
    pTexture->Release();

    *out_width = image_width;
    *out_height = image_height;
    stbi_image_free(image_data);

    return true;
}

bool View::CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void View::CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void View::CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void View::CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI View::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}


int View::Start(int argc, char** argv)
{
    machine->Init(argc, argv);
    /*Start Imgui*/
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Green emulator"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    /*Start imgui end */


    int my_image_width = 0;
    int my_image_height = 0;
    ID3D11ShaderResourceView* my_texture = NULL;
    bool ret = LoadTextureFromFile("C:\\Users\\Cindy\\Desktop\\MyImage01.jpg", &my_texture, &my_image_width, &my_image_height);
    IM_ASSERT(ret);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("8080 Emulator");
            static float f = 0.0f;
            static int counter = 0;

            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            static float sz = 36.0f;
            const ImVec2 p = ImGui::GetCursorScreenPos();
            const ImU32 col = ImColor(0, 0, 255);
            float x = p.x + 4.0f, y = p.y + 4.0f;
            float spacing = 10.0f;
            draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz + 10, y + sz), col);

            ImGui::Dummy(ImVec2((sz + spacing) * 9.8f, (sz + spacing) * 3));
            ImGui::End();
        }

        {
            ImGui::Begin("DirectX11 Texture Test");
            ImGui::Text("pointer = %p", my_texture);
            ImGui::Text("size = %d x %d", my_image_width, my_image_height);
            ImGui::Image((void*)my_texture, ImVec2(my_image_width, my_image_height));
            ImGui::End();



            /**************************************************************/
            //Translate the 1-bit space invaders frame buffer into
            // my 32bpp RGB bitmap.  We have to rotate and
            // flip the image as we go.
            //


            //TODO CDA :Beginning of change
            int vramPtr, b;

            unsigned char* screenPtr = (unsigned char*)machine->FrameBuffer();

            int x = 0;
            int y = 255;

            for (vramPtr = 0; vramPtr < 0x3FFF - 0x2400; vramPtr++)
            {
                for (b = 0; b < 8; b++)
                {
                    unsigned char pixel_val = ((readByte2(0x2400 + vramPtr) >> b) & 1) ? 0xFF : 0x00;;
                    screenPtr[(x + y * 256) * 4] = pixel_val;
                    screenPtr[(x + y * 256) * 4 + 1] = pixel_val;
                    screenPtr[(x + y * 256) * 4 + 2] = pixel_val;
                    screenPtr[(x + y * 256) * 4 + 3] = pixel_val;
                    y--;

                    if (y == -1)
                    {
                        y = 255;
                        x++;
                    }
                }
            }

            
            /*unsigned char* b = (unsigned char*)buffer8888;
            unsigned char* fb = (unsigned char*)machine->FrameBuffer();*/

            //for (int i = 0; i < 224; ++i)
            //{
            //    for (int j = 0; j < 256; j += 8)
            //    {
            //        int p;
            //        //Read the first 1-bit pixel
            //        // divide by 8 because there are 8 pixels
            //        // in a byte
            //        unsigned char pix = fb[(i * (256 / 8)) + j / 8];

            //        ////That makes 8 output vertical pixels
            //        //// we need to do a vertical flip
            //        //// so j needs to start at the last line
            //        //// and advance backward through the buffer
            //        int offset = (255 - j) * (224 * 4) + (i * 4);
            //        unsigned int* p1 = (unsigned int*)(&b[offset]);
            //        for (p = 0; p < 8; ++p)
            //        {
            //            if (0 != (pix & (1 << p)))
            //                *p1 = RGB_ON;
            //            else
            //                *p1 = RGB_OFF;
            //            p1 -= 224;  //next line
            //        }
            //    }
            //}
            //TODO CDA :End of change

            /*************************Temp**********************************************************/
            for (int h = 0; h < 4 * 224 * 256; h++)
            {
                if (b[h] == '\0')
                    bool tatata = true;
            }
            /**************************************************************************************/
            int my_image_width1 = 0;
            int my_image_height1 = 0;
            ID3D11ShaderResourceView* my_texture1 = NULL;
            bool ret1 = LoadTextureFromData(b, &my_texture1, &my_image_width1, &my_image_height1);
            IM_ASSERT(ret1);

            //224, 256 height and size 
            ImGui::Begin("My texture");
            ImGui::Text("pointer = %p", my_texture1);
            ImGui::Text("size = %d x %d", my_image_width1, my_image_height1);
            ImGui::Image((void*)my_texture1, ImVec2(my_image_width1, my_image_height1));
            ImGui::End();

            /*CGContextRef myContext = [[NSGraphicsContext currentContext]graphicsPort];
            CGImageRef ir = CGBitmapContextCreateImage(bitmapCtx);
            CGContextDrawImage(myContext, self.bounds, ir);
            CGImageRelease(ir);*/
            /**************************************************************/
        }

        ImGui::Render();
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}