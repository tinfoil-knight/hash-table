#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"
#include "prime.h"

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

static hash_table* new_sized_hashtable(const unsigned int base_size) {
  hash_table* ht = malloc(sizeof(hash_table));
  if (ht == NULL) {
    fprintf(stderr, "memory allocation failed for hashtable\n");
    exit(EXIT_FAILURE);
  }

  ht->base_size = base_size;
  // Using a prime number leads to lower collisions.
  ht->size = next_prime(ht->base_size);
  ht->count = 0;
  ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
  if (ht->items == NULL) {
    fprintf(stderr, "memory allocation failed for hashtable items\n");
    exit(EXIT_FAILURE);
  }

  return ht;
}

hash_table* new_hashtable() { return new_sized_hashtable(HT_INIT_SIZE); }

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

void print_ht_item(const ht_item* item) {
  if (item == NULL) {
    printf("ht_item is NULL\n");
    return;
  }

  printf("ht_item{ \"%s\" : \"%s\" }\n", item->key ? item->key : "NULL",
         item->value ? item->value : "NULL");
}

static void resize(hash_table* ht, unsigned int base_size) {
  if (base_size < HT_INIT_SIZE) {
    base_size = HT_INIT_SIZE;
  }

  hash_table* new_ht = new_sized_hashtable(base_size);
  for (int i = 0; i < ht->size; i++) {
    ht_item* item = ht->items[i];
    if (item != NULL && item != &HT_DELETED_ITEM) {
      insert(new_ht, item->key, item->value);
    }
  }

  ht->base_size = new_ht->base_size;
  ht->count = new_ht->count;

  // swap values and delete new_ht

  const int tmp_size = ht->size;
  ht->size = new_ht->size;
  new_ht->size = tmp_size;

  ht_item** tmp_items = ht->items;
  ht->items = new_ht->items;
  new_ht->items = tmp_items;

  del_hashtable(new_ht);
}

static void resize_up(hash_table* ht) { resize(ht, ht->base_size * 2); }
static void resize_down(hash_table* ht) { resize(ht, ht->base_size / 2); }

void insert(hash_table* ht, const char* key, const char* value) {
  const int load = ht->count * 100 / ht->size;
  if (load > 70)
    resize_up(ht);

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

char* search(const hash_table* ht, const char* key) {
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
  const int load = ht->count * 100 / ht->size;
  if (load < 30)
    resize_down(ht);

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
