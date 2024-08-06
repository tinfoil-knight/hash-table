typedef struct {
  char* key;
  char* value;
} ht_item;

typedef struct {
  unsigned int base_size;
  unsigned int size;
  unsigned int count;
  ht_item** items;
} hash_table;

hash_table* new_hashtable();
void del_hashtable(hash_table* ht);

void print_ht_item(const ht_item* item);

void insert(hash_table* ht, const char* key, const char* value);
char* search(const hash_table* ht, const char* key);
void del(hash_table* ht, const char* key);
