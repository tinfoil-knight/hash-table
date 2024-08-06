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

void insert_n_items(hash_table* ht, const unsigned int n) {
  for (int i = 0; i < n; i++) {
    char *key, *value;
    safe_asprintf(&key, "key%d", i);
    safe_asprintf(&value, "value%d", i);

    insert(ht, key, value);

    free(key);
    free(value);
  }
}

// [start, end)
void verify_items(const hash_table* ht, unsigned int start, unsigned int end) {
  for (int i = start; i < end; i++) {
    char *key, *value;
    safe_asprintf(&key, "key%d", i);
    safe_asprintf(&value, "value%d", i);

    char* result = search(ht, key);
    assert(result != NULL);
    assert(strcmp(result, value) == 0);

    free(key);
    free(value);
  }
}

void test_insert_and_search() {
  hash_table* ht = new_hashtable();
  const int NUM_ITEMS = 50;

  insert_n_items(ht, NUM_ITEMS);
  verify_items(ht, 0, NUM_ITEMS);

  assert(search(ht, "nonexistent_key") == NULL);

  del_hashtable(ht);
  success("test_insert_and_search");
}

void test_delete() {
  hash_table* ht = new_hashtable();
  const int NUM_ITEMS = 50;

  insert_n_items(ht, NUM_ITEMS);

  const char* to_del = "key0";
  del(ht, to_del);

  char* value = search(ht, to_del);
  assert(value == NULL);

  verify_items(ht, 1, NUM_ITEMS);

  del_hashtable(ht);
  success("test_delete");
}

void test_resize() {
  hash_table* ht = new_hashtable();
  const int NUM_ITEMS = 500; // should be large enough to trigger resizing

  // upward resize
  insert_n_items(ht, NUM_ITEMS);

  // verify that all items are inserted correctly
  verify_items(ht, 0, NUM_ITEMS);

  // downward resize
  for (int i = 0; i < NUM_ITEMS / 2; i++) {
    char* key;
    safe_asprintf(&key, "key%d", i);
    del(ht, key);
    free(key);
  }

  // verify that remaining items are still stored correctly
  verify_items(ht, NUM_ITEMS / 2, NUM_ITEMS);

  del_hashtable(ht);
  success("test_resize");
}

void run_tests() {
  test_insert_and_search();
  test_delete();
  test_resize();
}

int main() {
  setbuf(stdout, NULL);

  run_tests();
  printf("All tests completed.\n");
  return 0;
}
