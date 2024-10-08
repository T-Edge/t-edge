/*
 * Copyright (c) 2016, Linaro Limited
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

#include "tee_api_defines.h"
#include "trace.h"
#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <fpga_load_ta.h>

static TEE_TASessionHandle sess = TEE_HANDLE_NULL;
static const TEE_UUID fpga_load_uuid = PTA_ZYNQMP_PM_UUID;

/******************************************************************************/

static TEE_Result zynqmp_fpga_load(uint32_t cmd_id, uint32_t param_types, TEE_Param params[TEE_NUM_PARAMS])
{
    uint32_t returnOrigin;
    TEE_Result res = TEE_SUCCESS;

    res = TEE_InvokeTACommand(sess, TEE_TIMEOUT_INFINITE, cmd_id, param_types, params, &returnOrigin);
    DMSG("command %04x returns res = %s, return origin = %s", cmd_id, opteestrerr(res), opteestrerr(returnOrigin));

    return res;
}

static TEE_Result invoke_zynqmp_pm_pta(uint32_t cmd_id, uint32_t param_types, TEE_Param params[TEE_NUM_PARAMS])
{
    DMSG("Here");
    uint32_t returnOrigin;
    TEE_Result res = TEE_InvokeTACommand(sess, TEE_TIMEOUT_INFINITE, cmd_id, param_types, params, &returnOrigin);
    DMSG("TEE_InvokeTACommand returns res = %s, return origin = %s", opteestrerr(res), opteestrerr(returnOrigin));
    return res;
}
/******************************************************************************/

/*
 * Called when the instance of the TA is created. This is the first call in
 * the TA.
 */
TEE_Result TA_CreateEntryPoint(void)
{
    TEE_Result res = TEE_ERROR_GENERIC;
    DMSG("Opening PTA session ... ");
    res = TEE_OpenTASession(&fpga_load_uuid, TEE_TIMEOUT_INFINITE, 0, NULL, &sess, NULL);
    DMSG("TEE_OpenTASession returns res=0x%x", res);
    return res;
}

/*
 * Called when the instance of the TA is destroyed if the TA has not
 * crashed or panicked. This is the last call in the TA.
 */
void TA_DestroyEntryPoint(void)
{
    DMSG("Closing PTA session ... ");
    TEE_CloseTASession(sess);
}

/*
 * Called when a new session is opened to the TA. *sess_ctx can be updated
 * with a value to be able to identify this session in subsequent calls to the
 * TA. In this function you will normally do the global initialization for the
 * TA.
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types, TEE_Param __maybe_unused params[4], void __maybe_unused **sess_ctx)
{
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE,
        TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);

    DMSG("here");
    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;

    /* Unused parameters */
    (void)&params;
    (void)&sess_ctx;

    /*
     * The DMSG() macro is non-standard, TEE Internal API doesn't
     * specify any means to logging from a TA.
     */
    IMSG("Hello World!\n");

    /* If return value != TEE_SUCCESS the session will not be created. */
    return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
    (void)&sess_ctx; /* Unused parameter */
    IMSG("Goodbye!\n");
}

/*
 * Called when a TA is invoked. sess_ctx hold that value that was
 * assigned by TA_OpenSessionEntryPoint(). The rest of the paramters
 * comes from normal world.
 */
TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx, uint32_t cmd_id, uint32_t param_types, TEE_Param params[4])
{
    DMSG("TA_InvokeCommandEntryPoint");
    (void)&sess_ctx; /* Unused parameter */

    switch (cmd_id) {
    case TA_FPGA_LOAD_INIT:
    case TA_FPGA_LOAD_UPDATE:
    case TA_FPGA_LOAD_FIN:
        return zynqmp_fpga_load(cmd_id, param_types, params);
    default:
        return invoke_zynqmp_pm_pta(cmd_id, param_types, params);
    }
}
