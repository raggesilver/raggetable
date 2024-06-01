#include "hashtable.h"
#include "unity.h"
#include <stdbool.h>

// Force loads of collisions and resizes
// #define HASHTABLE_DEFAULT_CAPACITY 2

void setUp(void) { }

void tearDown(void) { }

static void test_hashtable_with_numbers(void)
{
    hashtable_t* table = hashtable_new();

    int num = 42;
    hashtable_set(table, "num", &num);
    int* val = hashtable_get(table, "num");
    TEST_ASSERT_EQUAL_INT(num, *val);

    hashtable_free(table);
}

static void test_hashtable_delete(void)
{
    hashtable_t* table = hashtable_new();

    int num = 42;
    hashtable_set(table, "num", &num);
    int* val = hashtable_get(table, "num");
    TEST_ASSERT_EQUAL_INT(num, *val);

    bool deleted = hashtable_delete(table, "num");
    TEST_ASSERT_TRUE(deleted);

    val = hashtable_get(table, "num");
    TEST_ASSERT_NULL(val);

    hashtable_free(table);
}

static void test_hashtable_remove(void)
{
    hashtable_t* table = hashtable_new();

    int num = 42;
    hashtable_set(table, "num", &num);
    int* val = hashtable_get(table, "num");
    TEST_ASSERT_EQUAL_INT(num, *val);

    void* removed;
    bool deleted = hashtable_remove(table, "num", &removed);
    TEST_ASSERT_TRUE(deleted);
    TEST_ASSERT_EQUAL_INT(num, *(int*)removed);

    val = hashtable_get(table, "num");
    TEST_ASSERT_NULL(val);

    hashtable_free(table);
}

static void test_hashtable_exists(void)
{
    hashtable_t* table = hashtable_new();

    int num = 42;
    hashtable_set(table, "num", &num);

    TEST_ASSERT_TRUE(hashtable_exists(table, "num"));
    TEST_ASSERT_FALSE(hashtable_exists(table, "num1"));

    hashtable_set(table, "num1", &num);
    hashtable_set(table, "num2", &num);
    hashtable_set(table, "num3", &num);
    hashtable_set(table, "num4", &num);
    hashtable_set(table, "num5", &num);

    TEST_ASSERT_TRUE(hashtable_exists(table, "num1"));
    TEST_ASSERT_TRUE(hashtable_exists(table, "num2"));
    TEST_ASSERT_TRUE(hashtable_exists(table, "num3"));
    TEST_ASSERT_TRUE(hashtable_exists(table, "num4"));
    TEST_ASSERT_TRUE(hashtable_exists(table, "num5"));
    TEST_ASSERT_FALSE(hashtable_exists(table, "num6"));

    hashtable_free(table);
}

static void test_hashtable_foreach(void)
{
    hashtable_t* table = hashtable_new();

    int num = 42;
    hashtable_set(table, "num1", &num);
    hashtable_set(table, "num2", &num);
    hashtable_set(table, "num3", &num);
    hashtable_set(table, "num4", &num);
    hashtable_set(table, "num5", &num);
    hashtable_set(table, "num6", &num);

    hashtable_foreach(table, {
        int* val = (int*)value;
        TEST_ASSERT_EQUAL_INT(num, *val);
    });

    hashtable_free(table);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_hashtable_with_numbers);
    RUN_TEST(test_hashtable_delete);
    RUN_TEST(test_hashtable_remove);
    RUN_TEST(test_hashtable_foreach);
    RUN_TEST(test_hashtable_exists);

    return UNITY_END();
}
