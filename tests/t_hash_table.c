#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/hash_table.h"

void success(const char* test_name) { printf("%s: Passed\n", test_name); }

void safe_asprintf(char** strp, const char* format, ...) {
  va_list args;
  va_start(args, format);
  int ret = vasprintf(strp, format, args);
  va_end(args);

  if (ret == -1) {
    perror("asprintf");
    exit(EXIT_FAILURE);
  }
}

void test_insert_and_search() {
  hash_table* ht = new_hashtable();
  const int NUM_ITEMS = 50;

  for (int i = 0; i < NUM_ITEMS; i++) {
    char *key, *value;
    safe_asprintf(&key, "key%d", i);
    safe_asprintf(&value, "value%d", i);

    insert(ht, key, value);

    free(key);
    free(value);
  }

  char* result;
  for (int i = 0; i < NUM_ITEMS; i++) {
    char *key, *value;
    safe_asprintf(&key, "key%d", i);
    safe_asprintf(&value, "value%d", i);

    result = search(ht, key);

    assert(result != NULL);
    assert(strcmp(result, value) == 0);

    free(key);
    free(value);
  }

  assert(search(ht, "nonexistent_key") == NULL);

  del_hashtable(ht);
  success("test_insert_and_search");
}

void test_delete() {
  hash_table* ht = new_hashtable();
  const int NUM_ITEMS = 50;

  for (int i = 0; i < NUM_ITEMS; i++) {
    char *key, *value;
    safe_asprintf(&key, "key%d", i);
    safe_asprintf(&value, "value%d", i);

    insert(ht, key, value);

    free(key);
    free(value);
  }

  const char* to_del = "key7";
  del(ht, to_del);

  char* value = search(ht, to_del);
  assert(value == NULL);

  char* result;
  for (int i = 0; i < NUM_ITEMS; i++) {
    char *key, *value;
    safe_asprintf(&key, "key%d", i);
    safe_asprintf(&value, "value%d", i);

    if (strcmp(to_del, key) != 0) {
      result = search(ht, key);
      assert(result != NULL);
      assert(strcmp(result, value) == 0);
    }

    free(key);
    free(value);
  }

  del_hashtable(ht);
  success("test_delete");
}

void run_tests() {
  test_insert_and_search();
  test_delete();
}

int main() {
  setbuf(stdout, NULL);

  run_tests();
  printf("All tests completed.\n");
  return 0;
}
