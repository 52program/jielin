/* crypt.h - 统一加密算法库头文件 */
#ifndef CRYPT_H
#define CRYPT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 算法类型定义 */
#define CRYPT_HASH_SHA1     0
#define CRYPT_HASH_SHA256   1
#define CRYPT_HASH_SHA384   2
#define CRYPT_HASH_SHA512   3

#define CRYPT_AES_MODE_CBC  0
#define CRYPT_AES_MODE_ECB  1
#define CRYPT_AES_MODE_GCM  2

/* 通用接口 */
int32_t crypt_rand(uint8_t *buf, int32_t size);

/* SHA系列 */
typedef struct crypt_sha_ctx crypt_sha_t;
crypt_sha_t *crypt_sha_create(int algorithm);
void crypt_sha_delete(crypt_sha_t **ctx);
int32_t crypt_sha_init(crypt_sha_t *ctx);
int32_t crypt_sha_update(crypt_sha_t *ctx, const uint8_t *data, int32_t len);
int32_t crypt_sha_final(crypt_sha_t *ctx, uint8_t *digest);

/* AES加密 */
typedef struct crypt_aes_ctx crypt_aes_t;
crypt_aes_t *crypt_aes_create(int mode);
void crypt_aes_delete(crypt_aes_t **ctx);
int32_t crypt_aes_setkey(crypt_aes_t *ctx, const uint8_t *key, int32_t key_len, 
                        const uint8_t *iv, int32_t iv_len);
int32_t crypt_aes_encrypt(crypt_aes_t *ctx, uint8_t *buf, int32_t size);
int32_t crypt_aes_decrypt(crypt_aes_t *ctx, uint8_t *buf, int32_t size);

/* HMAC */
typedef struct crypt_hmac_ctx crypt_hmac_t;
crypt_hmac_t *crypt_hmac_create(int algorithm);
int32_t crypt_hmac_init(crypt_hmac_t *ctx, const uint8_t *key, int32_t key_len);
int32_t crypt_hmac_update(crypt_hmac_t *ctx, const uint8_t *data, int32_t len);
int32_t crypt_hmac_final(crypt_hmac_t *ctx, uint8_t *digest);
void crypt_hmac_delete(crypt_hmac_t **ctx);

/* 实用函数 */
uint32_t crypt_crc32(uint32_t crc, const uint8_t *buf, int32_t len);
int32_t crypt_pbkdf2(const uint8_t *pass, int32_t pass_len, const uint8_t *salt, int32_t salt_len,
                    int32_t iterations, uint8_t *key, int32_t key_len);

#ifdef __cplusplus
}
#endif

#endif /* CRYPT_H */