/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define CLASS_NAME TEXT("vb_cls_dmy")

static BOOL cls_initialized = FALSE;
static HINSTANCE instance;
static WNDCLASSEX cls;

typedef struct {
	HWND win;
	HDC dc;
	HGLRC rc;
} ctx_data_t;

static LRESULT CALLBACK defaultProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	return DefWindowProc(hWnd, message, wParam, lParam);
}

static BOOL cls_is_registered() {
	WNDCLASSEX wndcls;
	const BOOL success = (GetClassInfoEx(instance, CLASS_NAME, &wndcls) != 0);
	return success;
}

static void ctx_new_cont(void **p_data, void **p_dc, void **p_rc, int *p_err) {
	HWND const win = CreateWindow(CLASS_NAME, TEXT("Dummy"), WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, NULL, NULL, instance, NULL);
	if (win) {
		HDC const dc = GetDC(win);
		if (dc) {
			int pixelFormat;
			PIXELFORMATDESCRIPTOR pixelFormatDesc;
			ZeroMemory(&pixelFormatDesc, sizeof(PIXELFORMATDESCRIPTOR));
			pixelFormatDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
			pixelFormatDesc.nVersion = 1;
			pixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
			pixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
			pixelFormatDesc.cColorBits = 32;
			pixelFormatDesc.cAlphaBits = 8;
			pixelFormatDesc.cDepthBits = 24;
			pixelFormat = ChoosePixelFormat(dc, &pixelFormatDesc);
			if (pixelFormat) {
				if (SetPixelFormat(dc, pixelFormat, &pixelFormatDesc)) {
					HGLRC const rc = wglCreateContext(dc);
					if (rc) {
						ctx_data_t *const data = malloc(sizeof(ctx_data_t));
						if (data) {
							data->win = win;
							data->dc = dc;
							data->rc = rc;
							*p_data = data;
							*p_dc = dc;
							*p_rc = rc;
							*p_err = 0;
						} else {
							ReleaseDC(win, dc);
							DestroyWindow(win);
							UnregisterClass(CLASS_NAME, instance);
							*p_err = 8;
						}
					} else {
						ReleaseDC(win, dc);
						DestroyWindow(win);
						UnregisterClass(CLASS_NAME, instance);
						*p_err = 7;
					}
				} else {
					ReleaseDC(win, dc);
					DestroyWindow(win);
					UnregisterClass(CLASS_NAME, instance);
					*p_err = 6;
				}
			} else {
				ReleaseDC(win, dc);
				DestroyWindow(win);
				UnregisterClass(CLASS_NAME, instance);
				*p_err = 5;
			}
		} else {
			DestroyWindow(win);
			UnregisterClass(CLASS_NAME, instance);
			*p_err = 4;
		}
	} else {
		UnregisterClass(CLASS_NAME, instance);
		*p_err = 3;
	}
}

void ctx_new(void **p_data, void **p_dc, void **p_rc, int *p_err) {
	if (!cls_initialized) {
		instance = GetModuleHandle(NULL);
		if (instance) {
			cls.cbSize = sizeof(WNDCLASSEXW);
			cls.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
			cls.lpfnWndProc = defaultProc;
			cls.cbClsExtra = 0;
			cls.cbWndExtra = 0;
			cls.hInstance = instance;
			cls.hIcon = LoadIcon(NULL, IDI_WINLOGO);
			cls.hCursor = LoadCursor(NULL, IDC_ARROW);
			cls.hbrBackground = NULL;
			cls.lpszMenuName = NULL;
			cls.lpszClassName = CLASS_NAME;
			cls.hIconSm = NULL;
			cls_initialized = TRUE;
			if (RegisterClassEx(&cls) != INVALID_ATOM) {
				ctx_new_cont(p_data, p_dc, p_rc, p_err);
			} else {
				*p_err = 2;
			}
		} else {
			*p_err = 1;
		}
	} else if (cls_is_registered()) {
		ctx_new_cont(p_data, p_dc, p_rc, p_err);
	} else if (RegisterClassEx(&cls) != INVALID_ATOM) {
		ctx_new_cont(p_data, p_dc, p_rc, p_err);
	} else {
		*p_err = 2;
	}
}

void ctx_make_current(void *p_dc, void *p_rc, int *p_err) {
	if (p_dc && p_rc) {
		HDC const dc = (HDC)p_dc;
		HGLRC const rc = (HGLRC)p_rc;
		if (wglMakeCurrent(dc, rc))
			*p_err = 0;
		else
			*p_err = 1;
	}
}

void ctx_destroy(void *p_data) {
	if (p_data) {
		ctx_data_t *const data = (ctx_data_t*)p_data;
		wglMakeCurrent(data->dc, NULL);
		wglDeleteContext(data->rc);
		ReleaseDC(data->win, data->dc);
		DestroyWindow(data->win);
		UnregisterClass(CLASS_NAME, instance);
		free(p_data);
	}
}