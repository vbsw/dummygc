/*
 *          Copyright 2021, Vitali Baumtrok.
 * Distributed under the Boost Software License, Version 1.0.
 *     (See accompanying file LICENSE or copy at
 *        http://www.boost.org/LICENSE_1_0.txt)
 */

package dummygc

import (
	"testing"
)

func TestNew(t *testing.T) {
	gcA, errA := New()
	if errA != nil {
		t.Error("A -", errA.Error())
	} else if gcA.DCPtr == nil {
		t.Error("A - dc is nil")
	} else if gcA.RCPtr == nil {
		t.Error("A - rc is nil")
	} else {
		gcB, errB := New()
		if errB != nil {
			t.Error("B -", errB.Error())
		} else if gcB.DCPtr == nil {
			t.Error("B - dc is nil")
		} else if gcB.RCPtr == nil {
			t.Error("B - rc is nil")
		} else {
			errA = gcA.MakeCurrent()
			errB = gcB.MakeCurrent()
			if errA != nil {
				t.Error("A -", errA.Error())
			}
			if errB != nil {
				t.Error("B -", errB.Error())
			}
			gcB.Destroy()
		}
		gcA.Destroy()
	}
}
