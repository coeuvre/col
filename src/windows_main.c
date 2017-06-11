#include <Windows.h>
#include <GL/GL.h>

#include "col.h"

#define WNDCLASSNAME "COLWNDCLASS"

static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wparam,
                                   LPARAM lparam) {
    switch (msg) {
        case WM_KEYDOWN: {
            int key = (int)wparam;
            if (key == VK_ESCAPE) {
                DestroyWindow(hwnd);
            }
        } break;

        case WM_DESTROY: {
            PostQuitMessage(0);
        } break;

        default:
            break;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

/*
 * TODO: The gap between two frames is not stable even the frame cost is low.
 *       Find a way to fix this.
 */
// static void doOneFrame(FrameContext *context) {
//     CGu64 frameTimeNS = cgSecondsToNanoseconds(context->frameTime);

//     CGu64 frameStart = cgGetTicks();

//     if (CG_UPDATE_GAME) {
//         CG_UPDATE_GAME(context->userData, context->frameTime);
//     }

//     if (CG_RENDER_GAME) {
//         CG_RENDER_GAME(context->userData);
//     }

//     CGu64 frameEnd = cgGetTicks();
//     CGu64 frameCost = cgTicksToNanoseconds(frameEnd - frameStart);
//     frameCost = cgNanosecondsToMilliseconds(frameCost);
//     cgLog(DEBUG, "Frame Cost: %" PRId64 "ms", frameCost);

//     SwapBuffers(context->hdc);

//     frameEnd = cgGetTicks();
//     CGu64 elapsed = cgTicksToNanoseconds(frameEnd - frameStart);

//     if (elapsed < frameTimeNS) {
//         CGu32 t = (CGu32)cgNanosecondsToMilliseconds(frameTimeNS - elapsed);
//         cgLog(DEBUG, "Sleep for %d ms", t);
//         Sleep(t);
//     }
// }

int main(int argc, char *argv[]) {
    timeBeginPeriod(1);

    ColContext *ctx = create();

    if (ctx->lastError) {
        fprintf(stderr, ctx->lastErrorMessage);
        return 1;
    }

    HINSTANCE hinstance = GetModuleHandle(NULL);

    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    /*
     * For OpenGL:
     *
     * When you create your HWND, you need to make sure that it has the CS_OWNDC
     * set for its style.
     */
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = windowProc;
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hInstance = hinstance;
    wc.lpszClassName = WNDCLASSNAME;

    if (RegisterClassEx(&wc) == 0) {
        fprintf(stderr, "Failed to register window class.");
        return 1;
    }

    HWND hwnd = CreateWindowEx(0, WNDCLASSNAME, "CG", WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                               CW_USEDEFAULT, 0, 0, hinstance, NULL);

    if (hwnd == 0) {
        fprintf(stderr, "Failed to create window.");
        return 1;
    }

    HDC hdc = GetDC(hwnd);
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,  // Flags
        PFD_TYPE_RGBA,  // The kind of framebuffer. RGBA or palette
        32,             // Colordepth of the framebuffer
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        24,  // Number of bits for the depthbuffer
        8,   // Number of bits for the stencilbuffer
        0,   // Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0,
        0,
        0,
    };

    int pfi = ChoosePixelFormat(hdc, &pfd);
    if (pfi == 0) {
        fprintf(stderr, "Failed to choose pixel format.");
        return 1;
    }

    if (SetPixelFormat(hdc, pfi, &pfd) == FALSE) {
        fprintf(stderr, "Failed to set pixel format.");
        return 1;
    }

    HGLRC hglrc = wglCreateContext(hdc);
    if (hglrc == NULL) {
        fprintf(stderr, "Failed to create OpenGL context.");
        return 1;
    }

    if (wglMakeCurrent(hdc, hglrc) == FALSE) {
        fprintf(stderr, "Failed to make current OpenGL context.");
        return 1;
    }

    fprintf(stdout, "OpenGL Version: %s", glGetString(GL_VERSION));

    ShowWindow(hwnd, SW_SHOW);

    int running = 1;
    MSG msg;
    while (running) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = 0;
            } else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        } else {
            update(ctx);
            render(ctx);
            SwapBuffers(hdc);
        }
    }

    return 0;
}
