#define unipix_winapi
#include "UniPix.h"
#include "Matrix.h"
#include "Image.h"
#include "format/PPM.h"

#include "stdio.h"

struct MyData {
    Matrix mat;
    PBH bufCfg;
};

void myRF(DC *window, Dim dim) {
    struct MyData data = *((struct MyData *) window->userData);
    /*
    DCPixelData(window, 0, 0, data.bufCfg, data.mat.data);
     */
    size_t minDimComp = dim.height;
    if (dim.width < dim.height) {
        minDimComp = dim.width;
    }
    size_t minSize = data.mat.width;
    if (data.mat.height < data.mat.width) {
        minSize = data.mat.height;
    }
    float scale = (float) minDimComp / (float) minSize;
    Matrix *upscaled = Upscale(data.mat, NULL, scale, scale);
    PBF pbf = (PBF) {
            .mode = PFM_RGB,
            .perPixel = 3
    };
    PBH pbh;
    PBHInit(&pbh, upscaled->width, upscaled->height, pbf);
    DCPixelData(window, 0, 0, pbh, upscaled->data);
}

int main() {
    DC dc;
    enum DCS err;

    err = DCInit(&dc,
                 myRF,
                 "UniPixTest",
                 "UniPix Test Window",
                 true);
    if (err != DCS_OK) {
        return err;
    }

    char *imgPath = "C:\\Users\\Alex\\Downloads\\house.ppm";
    FILE *imgFile = fopen(imgPath, "rb");
    if (imgFile == NULL) {
        return -1;
    }
    enum PPMErr perr;
    Matrix img = PPM(&perr, imgFile, true, true);
    fclose(imgFile);
    if (perr != PPMErr_OK) {
        free(img.data);
        return perr;
    }
    // MatrixRotateBlocks(mat, 3);

    const size_t width = img.width;
    const size_t height = img.height;

    struct MyData data;
    data.mat = img;
    PBF pbf = (PBF) {
        .mode = PFM_RGB,
        .perPixel = 3
    };
    PBHInit(&data.bufCfg, width, height, pbf);
    dc.userData = &data;

    err = DCStart(&dc);
    if (err != DCS_OK) {
        return err;
    }

    myRF(&dc, DCDim(&dc));

    while (err == DCS_OK) {
        err = DCTick(&dc);
    }

    free(img.data);

    err = DCStop(&dc);
    if (err != DCS_OK) {
        return err;
    }

/*
    // Step 1: Register the window class
    const char *CLASS_NAME = "MyWindowClass";

    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
            0,                              // Optional window styles
            CLASS_NAME,                     // Window class
            "My Window",                   // Window title
            WS_OVERLAPPEDWINDOW,            // Window style

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

            NULL,       // Parent window
            NULL,       // Menu
            GetModuleHandle(NULL),  // Instance handle
            NULL        // Additional application data
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, SW_SHOW);

    HDC hdc = GetDC(hwnd);

    const int width = 50;
    const int height = 50;

    char *pixels = malloc(width * height * sizeof(char) * 4);

    const int xDest = 15;
    const int yDest = 15;

    BITMAPINFO bmi = CreateRGBBitMapInfo(width, height);

    int res = SetDIBitsToDevice(
            hdc,
            xDest, yDest,
            width, height,
            0, 0,
            0, height,
            pixels,
            &bmi,
            0);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (res == 0) {
        return 1;
    }

    free(pixels);
    DestroyWindow(hwnd);
    UnregisterClass(CLASS_NAME, GetModuleHandle(NULL));
    */

    return 0;
}
