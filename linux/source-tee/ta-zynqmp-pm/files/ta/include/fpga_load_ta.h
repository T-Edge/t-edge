/*
 * Copyright (c) 2016-2017, Linaro Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef TA_FPGA_LOAD_H
#define TA_FPGA_LOAD_H

/*
 * This UUID is generated with uuidgen
 * the ITU-T UUID generator at http://www.itu.int/ITU-T/asn1/uuid.html
 */

#define PTA_ZYNQMP_PM_UUID                                 \
    {                                                      \
        0x68f3df04, 0x66f6, 0x11ed,                        \
        {                                                  \
            0xb9, 0xcb, 0x33, 0xb0, 0xf8, 0xb5, 0xe6, 0x1e \
        }                                                  \
    }

#define TA_FPGA_LOAD_UUID                                  \
    {                                                      \
        0x94e2e4be, 0x66fd, 0x11ed,                        \
        {                                                  \
            0xb9, 0xcb, 0x33, 0xb0, 0xf8, 0xb5, 0xe6, 0x1e \
        }                                                  \
    }

/* The function IDs implemented in this TA */
#define PTA_CMD_ZYNQMP_SOC_VERSION 0x0018
#define PTA_CMD_ZYNQMP_EFUSE_READ  0x0035
#define PTA_CMD_ZYNQMP_EFUSE_WRITE 0x0135
#define PTA_CMD_ZYNQMP_FPGA_LOAD   0x0016
#define TA_FPGA_LOAD_INIT          0x0116
#define TA_FPGA_LOAD_UPDATE        0x0216
#define TA_FPGA_LOAD_FIN           0x0316

static const char *opteestrerr(unsigned err)
{
    switch (err) {
    case 0x00000000:
        return "TEEC_SUCCESS";
    case 0xF0100003:
        return "TEEC_ERROR_STORAGE_NOT_AVAILABLE";
    case 0xFFFF0000:
        return "TEEC_ERROR_GENERIC";
    case 0xFFFF0001:
        return "TEEC_ERROR_ACCESS_DENIED";
    case 0xFFFF0002:
        return "TEEC_ERROR_CANCEL";
    case 0xFFFF0003:
        return "TEEC_ERROR_ACCESS_CONFLICT";
    case 0xFFFF0004:
        return "TEEC_ERROR_EXCESS_DATA";
    case 0xFFFF0005:
        return "TEEC_ERROR_BAD_FORMAT";
    case 0xFFFF0006:
        return "TEEC_ERROR_BAD_PARAMETERS";
    case 0xFFFF0007:
        return "TEEC_ERROR_BAD_STATE";
    case 0xFFFF0008:
        return "TEEC_ERROR_ITEM_NOT_FOUND";
    case 0xFFFF0009:
        return "TEEC_ERROR_NOT_IMPLEMENTED";
    case 0xFFFF000A:
        return "TEEC_ERROR_NOT_SUPPORTED";
    case 0xFFFF000B:
        return "TEEC_ERROR_NO_DATA";
    case 0xFFFF000C:
        return "TEEC_ERROR_OUT_OF_MEMORY";
    case 0xFFFF000D:
        return "TEEC_ERROR_BUSY";
    case 0xFFFF000E:
        return "TEEC_ERROR_COMMUNICATION";
    case 0xFFFF000F:
        return "TEEC_ERROR_SECURITY";
    case 0xFFFF0010:
        return "TEEC_ERROR_SHORT_BUFFER";
    case 0xFFFF0011:
        return "TEEC_ERROR_EXTERNAL_CANCEL";
    case 0xFFFF3024:
        return "TEEC_ERROR_TARGET_DEAD";
    case 0xFFFF3041:
        return "TEEC_ERROR_STORAGE_NO_SPACE";
    case 0x00000001:
        return "TEEC_ORIGIN_API";
    case 0x00000002:
        return "TEEC_ORIGIN_COMMS";
    case 0x00000003:
        return "TEEC_ORIGIN_TEE";
    case 0x00000004:
        return "TEEC_ORIGIN_TRUSTED_APP";
    default:
        return "(unknown)";
    }
}

#endif /*TA_FPGA_LOAD_H*/
