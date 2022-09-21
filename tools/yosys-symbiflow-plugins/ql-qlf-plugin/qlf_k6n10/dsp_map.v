// Copyright (C) 2020-2021  The SymbiFlow Authors.
//
// Use of this source code is governed by a ISC-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/ISC
//
// SPDX-License-Identifier:ISC

module \$__MUL16X16 (input [15:0] A, input [15:0] B, output [31:0] Y);
	parameter A_SIGNED = 0;
	parameter B_SIGNED = 0;
	parameter A_WIDTH = 0;
	parameter B_WIDTH = 0;
	parameter Y_WIDTH = 0;

	QL_DSP #(
		.A_REG(1'b0),
		.B_REG(1'b0),
		.C_REG(1'b0),
		.D_REG(1'b0),
		.ENABLE_DSP(1'b1),
	) _TECHMAP_REPLACE_ (
		.A(A),
		.B(B),
		.O(Y),
	);
endmodule
