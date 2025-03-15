/* �����㷨��Ԫ���� */
#include "crypt.h"
#include <stdio.h>
#include <windows.h>

// ���Ը�����
#define TEST_CASE_BEGIN(name) printf("������ %s ���Կ�ʼ\n", name)
#define TEST_CASE_END()       printf("������ ���Խ���\n\n")
#define VERIFY(cond, msg) \
    if (!(cond)) { printf("������ ����ʧ�ܣ�%s (�к� %d)\n", msg, __LINE__); return 0; } \
    else { printf("������ [ͨ��] %s\n", msg); }

// ��������������
static int total_tests = 0;
static int passed_tests = 0;

/******************************************************************************
*                            ��������ɲ���                                  *
******************************************************************************/
int test_rand() {
    TEST_CASE_BEGIN("��ȫ���������");
    int local_pass = 0;
    
    // ����1�����������������
    uint8_t buf1[256] = {0};
    int ret = crypt_rand(buf1, sizeof(buf1));
    VERIFY(ret == sizeof(buf1), "Ӧ����ָ�����ȵ������");
    
    // ����2�����ɲ�ͬ�����
    uint8_t buf2[256] = {0};
    crypt_rand(buf2, sizeof(buf2));
    VERIFY(memcmp(buf1, buf2, sizeof(buf1)) != 0, "�������ɵ������Ӧ��ͬ");
    
    // ����3����ָ�봦��
    ret = crypt_rand(NULL, 256);
    VERIFY(ret == 0, "��ָ������Ӧ����0");
    
    local_pass = 1;
    passed_tests++;
    return local_pass;
}

/******************************************************************************
*                            SHAϵ���㷨����                                 *
******************************************************************************/
// ��test_sha���������ret��������
int test_sha() {
    TEST_CASE_BEGIN("SHA256��ϣ�㷨");
    int local_pass = 0;
    int ret = 0; // ��ӱ�������
    
    // ��������
    const char *test_str = "Hello, �й�!";
    const uint8_t expected_hash[] = {
        0x3e, 0xfe, 0x5f, 0x8b, 0x5a, 0x4d, 0x84, 0x7d, 
        0x04, 0xaa, 0x9d, 0x6c, 0x3b, 0x66, 0xeb, 0x3a,
        0x4e, 0x7c, 0x4c, 0x61, 0xbf, 0x70, 0x1f, 0x45,
        0x98, 0x46, 0x54, 0x0c, 0x9c, 0x4d, 0xf2, 0x8e
    };
    
    // ��ʼ��������
    crypt_sha_t *ctx = crypt_sha_create(CRYPT_HASH_SHA256);
    VERIFY(ctx != NULL, "Ӧ�ɹ�����SHA������");
    
    // �ֿ���²���
    int len = (int)strlen(test_str);
    int half = len / 2;
    ret = crypt_sha_update(ctx, (uint8_t*)test_str, half);
    VERIFY(ret == half, "Ӧ�ɹ�����ǰ�벿������");
    ret = crypt_sha_update(ctx, (uint8_t*)test_str + half, len - half);
    VERIFY(ret == (len - half), "Ӧ�ɹ������벿������");
    
    // ���չ�ϣ����
    uint8_t digest[32] = {0};
    ret = crypt_sha_final(ctx, digest);
    VERIFY(ret == 0, "Ӧ�ɹ���ɹ�ϣ����");
    VERIFY(memcmp(digest, expected_hash, 32) == 0, "��ϣֵӦƥ��Ԥ�ڽ��");
    
    // �������
    crypt_sha_delete(&ctx);
    VERIFY(ctx == NULL, "Ӧ�ɹ��ͷ�SHA������");
    
    local_pass = 1;
    passed_tests++;
    return local_pass;
}

/******************************************************************************
*                            AES�����㷨����                                 *
******************************************************************************/
int test_aes() {
    TEST_CASE_BEGIN("AES-GCMģʽ����");
    int local_pass = 0;
    
    // ��������
    const uint8_t key[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
                          0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
    const uint8_t iv[]  = {0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
                          0x18,0x19,0x1a,0x1b};
    const char *plaintext = "����һ����Ҫ���ܵĲ�������";
    
    // ��ʼ������������
    crypt_aes_t *enc_ctx = crypt_aes_create(CRYPT_AES_MODE_GCM);
    VERIFY(enc_ctx != NULL, "Ӧ�ɹ�����AES����������");
    
    // ������Կ
    int ret = crypt_aes_setkey(enc_ctx, key, sizeof(key), iv, sizeof(iv));
    VERIFY(ret == 0, "Ӧ�ɹ�����AES��Կ��IV");
    
    // ���ܲ���
    size_t data_len = strlen(plaintext) + 1;
    uint8_t *ciphertext = malloc(data_len);
    memcpy(ciphertext, plaintext, data_len);
    ret = crypt_aes_encrypt(enc_ctx, ciphertext, data_len);
    VERIFY(ret == data_len, "Ӧ�ɹ���������");
    
    // ��ʼ������������
    crypt_aes_t *dec_ctx = crypt_aes_create(CRYPT_AES_MODE_GCM);
    VERIFY(dec_ctx != NULL, "Ӧ�ɹ�����AES����������");
    ret = crypt_aes_setkey(dec_ctx, key, sizeof(key), iv, sizeof(iv));
    VERIFY(ret == 0, "Ӧ�ɹ����ý�����Կ");
    
    // ���ܲ���
    ret = crypt_aes_decrypt(dec_ctx, ciphertext, data_len);
    VERIFY(ret == data_len, "Ӧ�ɹ���������");
    VERIFY(strcmp((char*)ciphertext, plaintext) == 0, "���ܺ�����Ӧ��ԭ��һ��");
    
    // �������
    free(ciphertext);
    crypt_aes_delete(&enc_ctx);
    crypt_aes_delete(&dec_ctx);
    
    local_pass = 1;
    passed_tests++;
    return local_pass;
}

/******************************************************************************
*                            ���ܻ�׼����                                    *
******************************************************************************/
// �������ܲ��Ժ�������
void benchmark_sha256() {
    TEST_CASE_BEGIN("SHA256���ܻ�׼");
    
    crypt_sha_t *ctx = crypt_sha_create(CRYPT_HASH_SHA256);
    VERIFY(ctx != NULL, "��ʼ��������");
    
    // 1MB��������
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
    printf("������ ��ϣ��������%.2f MB/s\n", (data_size / (1024 * 1024)) / (time / 1000));
    
    free(data);
    crypt_sha_delete(&ctx);  // ɾ���ڲ�����Ƕ�׵ĺ�������
}

// �޸��߳�������Ϊ��
#define THREADS 16 // ���ļ��������

// �������̲߳��Ժ���
void test_thread_safety() {
    TEST_CASE_BEGIN("���̰߳�ȫ����");
    HANDLE threads[THREADS];
    
    for (int i = 0; i < THREADS; i++) {
        threads[i] = CreateThread(NULL, 0, thread_test, NULL, 0, NULL);
        VERIFY(threads[i] != NULL, "Ӧ�ɹ������߳�");  // ʹ��threads����
    }
    
    WaitForMultipleObjects(THREADS, threads, TRUE, INFINITE);
    printf("������ [ͨ��] ���%d�������̲߳���\n", THREADS);
}

/******************************************************************************
*                                �����Գ���                                  *
******************************************************************************/
int main() {
    printf("\n================ �����㷨��Ԫ���� ================\n");
    
    total_tests += 1; passed_tests += test_rand();
    total_tests += 1; passed_tests += test_sha();
    total_tests += 1; passed_tests += test_aes();
    
    // ���ܲ���
    benchmark_sha256();
    test_thread_safety();
    
    printf("\n���Խ����%d/%d ͨ�� (%.1f%%)\n", 
          passed_tests, total_tests, 
          (float)passed_tests / total_tests * 100);
    return (passed_tests == total_tests) ? 0 : 1;
}