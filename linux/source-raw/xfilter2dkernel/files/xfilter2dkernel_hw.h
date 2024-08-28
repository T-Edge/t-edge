// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.2 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
// control
// 0x00 : Control signals
//        bit 0  - ap_start (Read/Write/COH)
//        bit 1  - ap_done (Read)
//        bit 2  - ap_idle (Read)
//        bit 3  - ap_ready (Read/COR)
//        bit 4  - ap_continue (Read/Write/SC)
//        bit 7  - auto_restart (Read/Write)
//        others - reserved
// 0x04 : Global Interrupt Enable Register
//        bit 0  - Global Interrupt Enable (Read/Write)
//        others - reserved
// 0x08 : IP Interrupt Enable Register (Read/Write)
//        bit 0 - enable ap_done interrupt (Read/Write)
//        bit 1 - enable ap_ready interrupt (Read/Write)
//        bit 5 - enable ap_local_deadlock interrupt (Read/Write)
//        others - reserved
// 0x0c : IP Interrupt Status Register (Read/TOW)
//        bit 0 - ap_done (COR/TOW)
//        bit 1 - ap_ready (COR/TOW)
//        bit 5 - ap_local_deadlock (COR/TOW)
//        others - reserved
// 0x10 : Data signal of coeffs
//        bit 31~0 - coeffs[31:0] (Read/Write)
// 0x14 : Data signal of coeffs
//        bit 31~0 - coeffs[63:32] (Read/Write)
// 0x18 : reserved
// 0x1c : Data signal of factor
//        bit 31~0 - factor[31:0] (Read/Write)
// 0x20 : reserved
// 0x24 : Data signal of bias
//        bit 15~0 - bias[15:0] (Read/Write)
//        others   - reserved
// 0x28 : reserved
// 0x2c : Data signal of width
//        bit 15~0 - width[15:0] (Read/Write)
//        others   - reserved
// 0x30 : reserved
// 0x34 : Data signal of height
//        bit 15~0 - height[15:0] (Read/Write)
//        others   - reserved
// 0x38 : reserved
// 0x3c : Data signal of stride
//        bit 15~0 - stride[15:0] (Read/Write)
//        others   - reserved
// 0x40 : reserved
// 0x44 : Data signal of src
//        bit 31~0 - src[31:0] (Read/Write)
// 0x48 : Data signal of src
//        bit 31~0 - src[63:32] (Read/Write)
// 0x4c : reserved
// 0x50 : Data signal of dst
//        bit 31~0 - dst[31:0] (Read/Write)
// 0x54 : Data signal of dst
//        bit 31~0 - dst[63:32] (Read/Write)
// 0x58 : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XFILTER2DKERNEL_CONTROL_ADDR_AP_CTRL     0x00
#define XFILTER2DKERNEL_CONTROL_ADDR_GIE         0x04
#define XFILTER2DKERNEL_CONTROL_ADDR_IER         0x08
#define XFILTER2DKERNEL_CONTROL_ADDR_ISR         0x0c
#define XFILTER2DKERNEL_CONTROL_ADDR_COEFFS_DATA 0x10
#define XFILTER2DKERNEL_CONTROL_BITS_COEFFS_DATA 64
#define XFILTER2DKERNEL_CONTROL_ADDR_FACTOR_DATA 0x1c
#define XFILTER2DKERNEL_CONTROL_BITS_FACTOR_DATA 32
#define XFILTER2DKERNEL_CONTROL_ADDR_BIAS_DATA   0x24
#define XFILTER2DKERNEL_CONTROL_BITS_BIAS_DATA   16
#define XFILTER2DKERNEL_CONTROL_ADDR_WIDTH_DATA  0x2c
#define XFILTER2DKERNEL_CONTROL_BITS_WIDTH_DATA  16
#define XFILTER2DKERNEL_CONTROL_ADDR_HEIGHT_DATA 0x34
#define XFILTER2DKERNEL_CONTROL_BITS_HEIGHT_DATA 16
#define XFILTER2DKERNEL_CONTROL_ADDR_STRIDE_DATA 0x3c
#define XFILTER2DKERNEL_CONTROL_BITS_STRIDE_DATA 16
#define XFILTER2DKERNEL_CONTROL_ADDR_SRC_DATA    0x44
#define XFILTER2DKERNEL_CONTROL_BITS_SRC_DATA    64
#define XFILTER2DKERNEL_CONTROL_ADDR_DST_DATA    0x50
#define XFILTER2DKERNEL_CONTROL_BITS_DST_DATA    64

