#include "hash_table.h"
#include "prime.h"

static const int HT_PRIME_1 = 151;
static const int HT_PRIME_2 = 163;
static ht_item HT_DELETED_ITEM = {NULL, NULL};
static const int HT_INITIAL_BASE_SIZE = 53;

/*  
    * Initialize a hash table item &  a hash table 
*/
static ht_item* ht_new_item(const char* k, const char* v) {
    ht_item* item = malloc(sizeof(ht_item));
    item -> key   = strdup(k);
    item -> value = strdup(v);
    return item;
}

/*
    * We will resize the hash table if load factor(ratio of filled buckets to the total bucket) gets higher or lower than certain values
    ! If load > 0.7; resize the table to the prime number greter than 2 * curr_size
    ! If load < 0.1; resize the table to the prime number greater than cur_size / 2
    * Below is a functional implementaion of creating a new hash table and resize operation 
*/

static ht_hash_table* ht_new_sized(const int base_size) {
    ht_hash_table* ht = malloc(sizeof(ht_hash_table));
    ht -> base_size   = base_size;
    ht -> size        = next_prime(ht -> base_size);
    ht -> count       = 0;
    ht -> items       = calloc((size_t)ht -> size, sizeof(ht_item*));
    return ht;
}


ht_hash_table* ht_new() {
    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

static void ht_resize(ht_hash_table* ht, const int base_size) {
    if (base_size < HT_INITIAL_BASE_SIZE) {
        return;
    }

    ht_hash_table* new_ht = ht_new_sized(base_size);
    for (int i = 0; i < ht->size; i ++) {
        ht_item* item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            ht_insert(new_ht, item->key, item->value);
        }
    }

    new_ht->base_size = ht->base_size;
    new_ht->count     = ht->count;

    ht->base_size = new_ht->base_size;
    ht->count = new_ht->count;

    // To delete new_ht, we give it ht's size and items 
    const int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    ht_item** tmp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;

    ht_del_hash_table(new_ht);
}

static void ht_resize_up(ht_hash_table* ht) {
    const int new_size = ht -> base_size * 2;
    ht_resize(ht, new_size);
}

static void ht_resize_down(ht_hash_table* ht) {
    const int new_size = ht->base_size / 2;
    ht_resize(ht, new_size);
}

/*
    * Deleting a hash item and a hash table
*/

static void ht_del_item(ht_item *item) {
    free(item -> key);
    free(item -> value);
    free(item);
}

void ht_del_hash_table(ht_hash_table *ht) {
    for (int i = 0; i < ht -> size; i ++) {
        ht_item* item = ht -> items[i];
        if (item && item != &HT_DELETED_ITEM) {
            ht_del_item(item);
        }
    }

    free(ht -> items);
    free(ht);
}

/*
    * Implement a hash function.
        ? A good Hash function should spread the values evenely. A uniform distribution is favoured since it evenely spreads out keys
        ? This will also lead to reduced number of collision 
        ! a sufficiently large Prime number is used in hash calculations for this purpose
        ? Hash fun should always give same output for the same input. For this, any helper calulations should produce reliable & same results
        !char_code(A) is used for this purpose

    * Hash is calulcated by converting string to very large number(possibly unique) and puttin it in the range 0 to m

    @param : s = string(key), a = prime number > ASCII of largest alphabet, m = size of hash table
*/

static int ht_hash(const char* s, const int a, const int m) {
    long hash       = 0;
    const int len_s = strlen(s);
    for (int i = 0; i < len_s; i++) {
        hash += (long)pow(a, len_s - (i + 1)) * s[i];
        hash = hash % m;
    }

    return (int)hash;
}

/* 
    * In case of collision, a double-hashing method is impemented.
    * A new hash is calculated using a existing hash or the new one, making sure that it is a unique number after 'x' attempts
*/
static int ht_get_hash(const char *s, const int num_buckets, const int attempt) {
    const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
    const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);

    return (hash_a + (attempt * (hash_b + 1)) % num_buckets);
}

/*
    * Hash table APIs are below
*/

char* ht_search(ht_hash_table *ht, const char *key) {
    int i         = 0;
    int index     = ht_get_hash(key, ht->size, i);
    ht_item *item = ht->items[index];
    ++i;

    while (item != NULL) {
        if (item != &HT_DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {
                return item->value;
            }
        }
        index = ht_get_hash(key, ht->size, i);
        item  = ht->items[index];
        ++i;
    }
    return NULL;
}

void ht_insert(ht_hash_table *ht, const char* key, const char* value) {
    const int load = ht -> count * 100 / ht -> size;
    if (load > 70) {
        ht_resize_up(ht);
    }

    ht_item* item     = ht_new_item(key, value);
    int index         = ht_get_hash(item->key, ht->size, 0);
    ht_item* cur_item = ht->items[index];
    int i             = 1;

    while(cur_item != NULL) {
        if (cur_item != &HT_DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {
                ht_del_item(cur_item);
                ht->items[index] = item;
                return;
            }
        }
        index    = ht_get_hash(item->key, ht->size, i);
        cur_item = ht->items[index];
        ++i;
    }

    ht->items[index] = item;
    ht->count       += 1;
}

void ht_delete(ht_hash_table* ht, const char* key) {
    const int load = ht -> count * 100 / ht -> size;
    if (load < 10) {
        ht_resize_down(ht);
    }

    int index     = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i         = 1;

    while (item != NULL) {
        if (item != &HT_DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {
                ht_del_item(item);
                ht->items[index] = &HT_DELETED_ITEM;
            }
        }
        index  = ht_get_hash(key, ht->size, i);
        item   = ht->items[index];
        ++i; 
    }
    ht->count -= 1;
}

int main() {
    // Create a new hash table
    ht_hash_table* ht = ht_new();

    // Insert key-value pairs
    ht_insert(ht, "name", "John");
    ht_insert(ht, "age", "30");
    ht_insert(ht, "city", "New York");

    // Search for values by keys
    char* name = ht_search(ht, "name");
    char* age = ht_search(ht, "age");
    char* city = ht_search(ht, "city");

    if (name) {
        printf("Name: %s\n", name);
    }
    if (age) {
        printf("Age: %s\n", age);
    }
    if (city) {
        printf("City: %s\n", city);
    }

    // Delete a key-value pair
    ht_delete(ht, "age");

    // Search for the deleted key
    char* deleted_age = ht_search(ht, "age");
    if (deleted_age == NULL) {
        printf("Age has been deleted.\n");
    }
    // Clean up and delete the hash table
   ht_del_hash_table(ht);
}