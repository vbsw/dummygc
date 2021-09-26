/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

// dummygc creates an OpenGL 1.0 graphic context on Windows systems.
package dummygc

// #cgo LDFLAGS: -luser32 -lgdi32 -lOpenGL32
// #include "dummygc.h"
import "C"
import (
	"errors"
	"unsafe"
)

var (
	ErrGetInst   error
	ErrRegCls    error
	ErrCreateWin error
	ErrGetDC     error
	ErrCPF       error
	ErrSPF       error
	ErrCreateRC  error
	ErrAlloc     error
	ErrMakeCurr  error
)

func init() {
	ErrGetInst = errors.New("get module instance failed")
	ErrRegCls = errors.New("register dummy class failed")
	ErrCreateWin = errors.New("create dummy window failed")
	ErrGetDC = errors.New("get dummy device context failed")
	ErrCPF = errors.New("choose dummy pixel format failed")
	ErrSPF = errors.New("set dummy pixel format failed")
	ErrCreateRC = errors.New("create dummy render context failed")
	ErrAlloc = errors.New("memory allocation failed")
	ErrMakeCurr = errors.New("make context current failed")
}

type GC struct {
	internalData unsafe.Pointer
	// HDC
	DCPtr unsafe.Pointer
	// HGLRC
	RCPtr unsafe.Pointer
}

func errNumToError(errNum int) error {
	switch errNum {
	case 1:
		return ErrGetInst
	case 2:
		return ErrRegCls
	case 3:
		return ErrCreateWin
	case 4:
		return ErrGetDC
	case 5:
		return ErrCPF
	case 6:
		return ErrSPF
	case 7:
		return ErrCreateRC
	case 8:
		return ErrAlloc
	}
	return nil
}

// // New creates a new instance of GC and returns it.
func New() (*GC, error) {
	var data unsafe.Pointer
	var dcPtr unsafe.Pointer
	var rcPtr unsafe.Pointer
	var errNum C.int
	C.ctx_new(&data, &dcPtr, &rcPtr, &errNum)
	if errNum == 0 {
		gc := new(GC)
		gc.internalData = data
		gc.DCPtr = dcPtr
		gc.RCPtr = rcPtr
		return gc, nil
	}
	return nil, errNumToError(int(errNum))
}

// MakeCurrent makes the graphic context current.
func (gc *GC) MakeCurrent() error {
	var errNum C.int
	C.ctx_make_current(gc.DCPtr, gc.RCPtr, &errNum)
	if errNum == 0 {
		return nil
	}
	return ErrMakeCurr
}

// Destroy releases all memory associated with the context.
func (gc *GC) Destroy() {
	C.ctx_destroy(gc.internalData)
	gc.internalData = nil
	gc.DCPtr = nil
	gc.RCPtr = nil
}
