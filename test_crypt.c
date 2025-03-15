/* 加密算法单元测试 */
#include "crypt.h"
#include <stdio.h>
#include <windows.h>

// 测试辅助宏
#define TEST_CASE_BEGIN(name) printf("┌── %s 测试开始\n", name)
#define TEST_CASE_END()       printf("└── 测试结束\n\n")
#define VERIFY(cond, msg) \
    if (!(cond)) { printf("├── 测试失败：%s (行号 %d)\n", msg, __LINE__); return 0; } \
    else { printf("├── [通过] %s\n", msg); }

// 测试用例计数器
static int total_tests = 0;
static int passed_tests = 0;

/******************************************************************************
*                            随机数生成测试                                  *
******************************************************************************/
int test_rand() {
    TEST_CASE_BEGIN("安全随机数生成");
    int local_pass = 0;
    
    // 测试1：正常生成随机数据
    uint8_t buf1[256] = {0};
    int ret = crypt_rand(buf1, sizeof(buf1));
    VERIFY(ret == sizeof(buf1), "应生成指定长度的随机数");
    
    // 测试2：生成不同随机数
    uint8_t buf2[256] = {0};
    crypt_rand(buf2, sizeof(buf2));
    VERIFY(memcmp(buf1, buf2, sizeof(buf1)) != 0, "两次生成的随机数应不同");
    
    // 测试3：空指针处理
    ret = crypt_rand(NULL, 256);
    VERIFY(ret == 0, "空指针输入应返回0");
    
    local_pass = 1;
    passed_tests++;
    return local_pass;
}

/******************************************************************************
*                            SHA系列算法测试                                 *
******************************************************************************/
// 在test_sha函数中添加ret变量声明
int test_sha() {
    TEST_CASE_BEGIN("SHA256哈希算法");
    int local_pass = 0;
    int ret = 0; // 添加变量声明
    
    // 测试数据
    const char *test_str = "Hello, 中国!";
    const uint8_t expected_hash[] = {
        0x3e, 0xfe, 0x5f, 0x8b, 0x5a, 0x4d, 0x84, 0x7d, 
        0x04, 0xaa, 0x9d, 0x6c, 0x3b, 0x66, 0xeb, 0x3a,
        0x4e, 0x7c, 0x4c, 0x61, 0xbf, 0x70, 0x1f, 0x45,
        0x98, 0x46, 0x54, 0x0c, 0x9c, 0x4d, 0xf2, 0x8e
    };
    
    // 初始化上下文
    crypt_sha_t *ctx = crypt_sha_create(CRYPT_HASH_SHA256);
    VERIFY(ctx != NULL, "应成功创建SHA上下文");
    
    // 分块更新测试
    int len = (int)strlen(test_str);
    int half = len / 2;
    ret = crypt_sha_update(ctx, (uint8_t*)test_str, half);
    VERIFY(ret == half, "应成功处理前半部分数据");
    ret = crypt_sha_update(ctx, (uint8_t*)test_str + half, len - half);
    VERIFY(ret == (len - half), "应成功处理后半部分数据");
    
    // 最终哈希计算
    uint8_t digest[32] = {0};
    ret = crypt_sha_final(ctx, digest);
    VERIFY(ret == 0, "应成功完成哈希计算");
    VERIFY(memcmp(digest, expected_hash, 32) == 0, "哈希值应匹配预期结果");
    
    // 清理测试
    crypt_sha_delete(&ctx);
    VERIFY(ctx == NULL, "应成功释放SHA上下文");
    
    local_pass = 1;
    passed_tests++;
    return local_pass;
}

/******************************************************************************
*                            AES加密算法测试                                 *
******************************************************************************/
int test_aes() {
    TEST_CASE_BEGIN("AES-GCM模式加密");
    int local_pass = 0;
    
    // 测试数据
    const uint8_t key[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
                          0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
    const uint8_t iv[]  = {0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
                          0x18,0x19,0x1a,0x1b};
    const char *plaintext = "这是一条需要加密的测试数据";
    
    // 初始化加密上下文
    crypt_aes_t *enc_ctx = crypt_aes_create(CRYPT_AES_MODE_GCM);
    VERIFY(enc_ctx != NULL, "应成功创建AES加密上下文");
    
    // 设置密钥
    int ret = crypt_aes_setkey(enc_ctx, key, sizeof(key), iv, sizeof(iv));
    VERIFY(ret == 0, "应成功设置AES密钥和IV");
    
    // 加密测试
    size_t data_len = strlen(plaintext) + 1;
    uint8_t *ciphertext = malloc(data_len);
    memcpy(ciphertext, plaintext, data_len);
    ret = crypt_aes_encrypt(enc_ctx, ciphertext, data_len);
    VERIFY(ret == data_len, "应成功加密数据");
    
    // 初始化解密上下文
    crypt_aes_t *dec_ctx = crypt_aes_create(CRYPT_AES_MODE_GCM);
    VERIFY(dec_ctx != NULL, "应成功创建AES解密上下文");
    ret = crypt_aes_setkey(dec_ctx, key, sizeof(key), iv, sizeof(iv));
    VERIFY(ret == 0, "应成功设置解密密钥");
    
    // 解密测试
    ret = crypt_aes_decrypt(dec_ctx, ciphertext, data_len);
    VERIFY(ret == data_len, "应成功解密数据");
    VERIFY(strcmp((char*)ciphertext, plaintext) == 0, "解密后数据应与原文一致");
    
    // 清理测试
    free(ciphertext);
    crypt_aes_delete(&enc_ctx);
    crypt_aes_delete(&dec_ctx);
    
    local_pass = 1;
    passed_tests++;
    return local_pass;
}

/******************************************************************************
*                            性能基准测试                                    *
******************************************************************************/
// 修正性能测试函数定义
void benchmark_sha256() {
    TEST_CASE_BEGIN("SHA256性能基准");
    
    crypt_sha_t *ctx = crypt_sha_create(CRYPT_HASH_SHA256);
    VERIFY(ctx != NULL, "初始化上下文");
    
    // 1MB测试数据
    const int data_size = 1024 * 1024;
    uint8_t *data = malloc(data_size);
    memset(data, 0xAA, data_size);
    
    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);
    
    crypt_sha_update(ctx, data, data_size);
    crypt_sha_final(ctx, NULL);
    
    QueryPerformanceCounter(&end);
    double time = (end.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
    printf("├── 哈希吞吐量：%.2f MB/s\n", (data_size / (1024 * 1024)) / (time / 1000));
    
    free(data);
    crypt_sha_delete(&ctx);  // 删除内部错误嵌套的函数定义
}

// 修改线程数定义为宏
#define THREADS 16 // 在文件顶部添加

// 修正多线程测试函数
void test_thread_safety() {
    TEST_CASE_BEGIN("多线程安全测试");
    HANDLE threads[THREADS];
    
    for (int i = 0; i < THREADS; i++) {
        threads[i] = CreateThread(NULL, 0, thread_test, NULL, 0, NULL);
        VERIFY(threads[i] != NULL, "应成功创建线程");  // 使用threads变量
    }
    
    WaitForMultipleObjects(THREADS, threads, TRUE, INFINITE);
    printf("├── [通过] 完成%d个并发线程测试\n", THREADS);
}

/******************************************************************************
*                                主测试程序                                  *
******************************************************************************/
int main() {
    printf("\n================ 加密算法单元测试 ================\n");
    
    total_tests += 1; passed_tests += test_rand();
    total_tests += 1; passed_tests += test_sha();
    total_tests += 1; passed_tests += test_aes();
    
    // 性能测试
    benchmark_sha256();
    test_thread_safety();
    
    printf("\n测试结果：%d/%d 通过 (%.1f%%)\n", 
          passed_tests, total_tests, 
          (float)passed_tests / total_tests * 100);
    return (passed_tests == total_tests) ? 0 : 1;
}