#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

#define HT_INIT_SIZE 100
#define HT_PRIME_1 313
#define HT_PRIME_2 653

static ht_item HT_DELETED_ITEM = {NULL, NULL};

static ht_item* new_item(const char* k, const char* v) {
  ht_item* i = malloc(sizeof(ht_item));
  i->key = strdup(k);
  i->value = strdup(v);
  return i;
}

static void del_item(ht_item* i) {
  if (i != &HT_DELETED_ITEM) {
    free(i->key);
    free(i->value);
    free(i);
  }
}

hash_table* new_hashtable() {
  hash_table* ht = malloc(sizeof(hash_table));

  ht->size = HT_INIT_SIZE;
  ht->count = 0;
  ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
  return ht;
}

void del_hashtable(hash_table* ht) {
  for (int i = 0; i < ht->size; i++) {
    ht_item* item = ht->items[i];
    if (item != NULL) {
      del_item(item);
    }
  }

  free(ht->items);
  free(ht);
}

// Polynomial Rolling Hash
static int hash(const char* s, const int a, const int num_buckets) {
  // a - arbitrary constant that should ideally be a prime number larger than
  // the input set being used (> 128 for ASCII chars)

  long hash = 0;
  const int len_s = strlen(s);
  for (int i = 0; i < len_s; i++) {
    hash += (long)pow(a, len_s - (i + 1)) * s[i];
    hash = hash % num_buckets;
  }
  return (int)hash;
}

// Probe Sequence : Double Hashing
static int get_hash(const char* s, const int num_buckets, const int attempt) {
  int hash_a = hash(s, HT_PRIME_1, num_buckets);
  int hash_b = hash(s, HT_PRIME_2, num_buckets);
  if (hash_b % num_buckets == 0)
    hash_b = 1;
  return (hash_a + (attempt * hash_b)) % num_buckets;
}

void print_ht_item(ht_item* item) {
  if (item == NULL) {
    printf("ht_item is NULL\n");
    return;
  }

  printf("ht_item{ \"%s\" : \"%s\" }\n", item->key ? item->key : "NULL",
         item->value ? item->value : "NULL");
}

void insert(hash_table* ht, const char* key, const char* value) {
  ht_item* item = new_item(key, value);
  int index = get_hash(item->key, ht->size, 0);
  ht_item* cur_item = ht->items[index];
  int i = 1;
  while (cur_item != NULL) {
    if (cur_item != &HT_DELETED_ITEM && strcmp(cur_item->key, key) == 0) {
      del_item(cur_item);
      ht->items[index] = item;
      return;
    }
    index = get_hash(item->key, ht->size, i);
    cur_item = ht->items[index];
    i++;
  }
  ht->items[index] = item;
  ht->count++;
}

char* search(hash_table* ht, const char* key) {
  int index = get_hash(key, ht->size, 0);
  ht_item* item = ht->items[index];
  int i = 1;
  while (item != NULL) {
    if (item != &HT_DELETED_ITEM && strcmp(item->key, key) == 0) {
      return item->value;
    }
    index = get_hash(key, ht->size, i);
    item = ht->items[index];
    i++;
  }
  return NULL;
}

void del(hash_table* ht, const char* key) {
  int index = get_hash(key, ht->size, 0);
  ht_item* item = ht->items[index];
  int i = 1;
  while (item != NULL) {
    if (item != &HT_DELETED_ITEM && strcmp(item->key, key) == 0) {
      del_item(item);
      ht->items[index] = &HT_DELETED_ITEM;
      ht->count--;
      return;
    }
    index = get_hash(key, ht->size, i);
    item = ht->items[index];
    i++;
  }
}
