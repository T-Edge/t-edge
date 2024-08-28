#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <err.h>
#include <tee_client_api.h>

#include "../user_ta_header_defines.h"
#include <fpga_load_ta.h>

struct attestation_signature {
    uint8_t hash_nonce[32];
    uint8_t hash_message[32];
    uint8_t signature[64];
};

#define optee(func, ...)                                  \
    do {                                                  \
        TEEC_Result res = func(__VA_ARGS__);              \
        if (res != TEEC_SUCCESS) {                        \
            errx(1, #func " failed with code 0x%x", res); \
        }                                                 \
    } while (0)

#define optee_orig(func, ...)                                                                \
    do {                                                                                     \
        uint32_t orig;                                                                       \
        TEEC_Result res = func(__VA_ARGS__, &orig);                                          \
        if (res != TEEC_SUCCESS) {                                                           \
            errx(1, #func " failed with %s origin %s", opteestrerr(res), opteestrerr(orig)); \
        }                                                                                    \
    } while (0)

#define eputs(msg)                                                \
    do {                                                          \
        fprintf(stderr, "%s:%d - " msg "\n", __FILE__, __LINE__); \
    } while (0)

#define eprintf(fmt, ...)                                                      \
    do {                                                                       \
        fprintf(stderr, "%s:%d - " fmt "\n", __FILE__, __LINE__, __VA_ARGS__); \
    } while (0)

const TEEC_UUID uuid = TA_FPGA_LOAD_UUID;

uint64_t get_random(void)
{
    uint64_t random;
    FILE *fp = fopen("/dev/urandom", "r");
    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fread(&random, sizeof(random), 1, fp);
    fclose(fp);
    return random;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        errx(1, "Usage: %s <bitstream>\n", argv[0]);
    }

    // Initialize a context connecting us to the TEE
    TEEC_Context ctx;
    eputs("TEEC_InitializeContext before");
    optee(TEEC_InitializeContext, NULL, &ctx);
    eputs("TEEC_InitializeContext after");

    // Open a session
    TEEC_Session sess;
    eputs("TEEC_OpenSession before");
    optee_orig(TEEC_OpenSession, &ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL);
    eputs("TEEC_OpenSession after");

    // Execute a function in the TA
    TEEC_Operation op;

    /* get soc version */
    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_OUTPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    optee_orig(TEEC_InvokeCommand, &sess, PTA_CMD_ZYNQMP_SOC_VERSION, &op);
    eprintf(">>> soc version = %x", op.params[0].value.a);

    eputs("============================================================");

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    eprintf("file length is %ld", length);

    eputs("------------------------------------------------------------");

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    op.params[0].value.a = length;
    optee_orig(TEEC_InvokeCommand, &sess, TA_FPGA_LOAD_INIT, &op);

    eputs("------------------------------------------------------------");

#define STEP_SIZE 0x10000

    char buf[STEP_SIZE];
    unsigned nbyte = 0;
    unsigned total = 0;

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = buf;
    op.params[0].tmpref.size = 0;

    while ((nbyte = fread(buf, 1, STEP_SIZE, fp)) > 0) {
        op.params[0].tmpref.size = nbyte;
        optee_orig(TEEC_InvokeCommand, &sess, TA_FPGA_LOAD_UPDATE, &op);
        total += nbyte;
        eprintf(">>> %u / %ld bytes loaded", total, length);
    }

    eputs("------------------------------------------------------------");

    /* Load FPGA Bitstream */
    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);

    uint64_t nonce = get_random();
    printf("nonce: 0x%016llx\n", nonce);

    op.params[0].tmpref.buffer = &nonce;
    op.params[0].tmpref.size = sizeof(nonce);

    struct attestation_signature sig;
    op.params[1].tmpref.buffer = &sig;
    op.params[1].tmpref.size = sizeof(sig);

    optee_orig(TEEC_InvokeCommand, &sess, TA_FPGA_LOAD_FIN, &op);

    // hexdump of signature
    for (int i = 0; i < sizeof(sig); i++) {
        printf("%02x", ((uint8_t *)&sig)[i]);
    }
    printf("\n");

    eputs("============================================================");

    eputs("TEEC_CloseSession");
    TEEC_CloseSession(&sess);
    eputs("TEEC_CloseSession");

    eputs("TEEC_FinalizeContext");
    TEEC_FinalizeContext(&ctx);
    eputs("TEEC_FinalizeContext");

    return 0;
}
