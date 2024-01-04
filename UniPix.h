#ifndef UNIPIX_H
#define UNIPIX_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define unipix_winapi // TODO: remove

#ifdef unipix_winapi
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

struct DC;

typedef struct {
    size_t width;
    size_t height;
} Dim;

typedef void (*DCRF)(struct DC *window,
                     Dim dim);

typedef struct DC {
    DCRF renderFunction;
    bool initialized;
    bool running;
    const char *id;
    void *userData;
    bool callRFonRS;
#ifdef unipix_winapi
    HWND hwnd;
    HDC hdc;
#endif
} DC;

enum DCS {
    DCS_OK,     // running
    DCS_UTerm,  // user terminated
    DCS_Err,    // other error
    DCS_AlrRun, // already running
    DCS_NoInit, // not initialized
    DCS_NoRun,  // not running
};

#ifdef unipix_winapi
#define RGBBMI(width,height,bytes) (BITMAPINFO) { \
        .bmiColors = { 0 },                       \
        .bmiHeader = (BITMAPINFOHEADER) {         \
            .biSize = sizeof(BITMAPINFOHEADER),   \
            .biWidth = width,                     \
            .biHeight = -height,                  \
            .biPlanes = 1,                        \
            .biBitCount = bytes * 8,              \
            .biCompression = BI_RGB,              \
            .biSizeImage = width * height * bytes,\
            .biXPelsPerMeter = 0,                 \
            .biYPelsPerMeter = 0,                 \
            .biClrUsed = 0,                       \
            .biClrImportant = 0,                  \
        }                                         \
    }

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY: {
            DC *dc = (DC *)
                    GetWindowLongPtr(hwnd,
                                     GWLP_USERDATA);
            dc->running = false;
            PostQuitMessage(0);
        }
        break;

        case WM_SIZE: {
            DC *dc = (DC *)
                    GetWindowLongPtr(hwnd,
                                     GWLP_USERDATA);
            if (dc->callRFonRS) {
                Dim dim = (Dim) {
                    LOWORD(lParam),
                    HIWORD(lParam)
                };
                dc->renderFunction(dc,
                                   dim);
            }
        }
        break;

        default: {
            return DefWindowProc(hwnd,
                                 uMsg,
                                 wParam,
                                 lParam);
        }
    }
    return 0;
}
#endif

enum DCS DCInit(DC *dc,
            DCRF rf,
            const char *name,
            const char *id,
            bool callRFonRS) {
    dc->renderFunction = rf;
    dc->running = false;
    dc->initialized = true;
    dc->id = id;
    dc->callRFonRS = callRFonRS;
#ifdef unipix_winapi
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = id;
    RegisterClass(&wc);

    HWND hwnd =
            CreateWindowEx(0,
                           id,
                           name,
                           WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT,
                           CW_USEDEFAULT,
                           CW_USEDEFAULT,
                           CW_USEDEFAULT,
                           NULL,
                           NULL,
                           GetModuleHandle(NULL),
                           NULL);

    if (hwnd == NULL) {
        return DCS_Err;
    }

    SetWindowLongPtr(hwnd,
                     GWLP_USERDATA,
                     (long long) dc);

    dc->hwnd = hwnd;
#endif
    return DCS_OK;
}

enum DCS DCStart(DC *dc) {
    if (!dc->initialized) {
        return DCS_NoInit;
    }

    if (dc->running) {
        return DCS_AlrRun;
    }
    dc->running = true;

#ifdef unipix_winapi
    HWND hwnd = dc->hwnd;
    ShowWindow(hwnd, SW_SHOW);
    HDC hdc = GetDC(hwnd);
    dc->hdc = hdc;
#endif

    return DCS_OK;
}

enum DCS DCTick(DC *dc) {
    if (!dc->running) {
        return DCS_NoRun;
    }

    MSG msg = { };
    bool run = GetMessage(&msg, NULL, 0, 0);
    if (!run) {
        dc->running = false;
        dc->initialized = false;
        return DCS_UTerm;
    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    if (!dc->running) {
        dc->initialized = false;
        return DCS_UTerm;
    }
    return DCS_OK;
}

enum DCS DCStop(DC *dc) {
    if (!dc->running) {
        return DCS_NoRun;
    }
    if (dc->initialized) {
        DestroyWindow(dc->hwnd);
        UnregisterClass(dc->id, GetModuleHandle(NULL));
    }
    return DCS_OK;
}

enum PFM {
    PFM_RGB
};

typedef struct {
    enum PFM mode;
    size_t perPixel;
} PBF;

typedef struct {
    size_t width;
    size_t height;
#ifdef unipix_winapi
    BITMAPINFO bmi;
#endif
} PBH;

void PBHInit(PBH *pbh,
             size_t width,
             size_t height,
             PBF format) {
#ifdef unipix_winapi
    BITMAPINFO bmi;
    if (format.mode == PFM_RGB) {
        bmi = RGBBMI(width, height, format.perPixel);
    }
#endif
    *pbh = (PBH) {
        .width = width,
        .height = height,
#ifdef unipix_winapi
        .bmi = bmi
#endif
    };
}

enum DCS DCPixelData(DC *dc,
                    size_t destX,
                    size_t destY,
                    PBH bufCfg,
                    char *pixels) {
    if (!dc->running) {
        return DCS_NoRun;
    }

#ifdef unipix_winapi
    int res = SetDIBitsToDevice(
            dc->hdc,
            (int) destX, (int) destY,
            bufCfg.width, bufCfg.height,
            0, 0,
            0, bufCfg.height,
            pixels,
            &bufCfg.bmi,
            0);

    if (res == 0) {
        return DCS_Err;
    }
#endif

    return DCS_OK;
}

Dim DCDim(DC *dc) {
#ifdef unipix_winapi
    RECT rect;
    if(GetWindowRect(dc->hwnd, &rect)) {
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        return (Dim) { width, height };
    }
#endif
}

#endif