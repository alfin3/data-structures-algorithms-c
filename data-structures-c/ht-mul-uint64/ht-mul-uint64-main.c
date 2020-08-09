/**
   ht-mul-uint64-main.c

   Examples of a hash table with generic hash keys and generic elements.
   The implementation is based on an multiplication method for hashing into 
   upto 2^63 slots (the upper range requiring > 2^64 addresses) and an 
   open addressing method for resolving collisions.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "ht-mul-uint64.h"
#include "dll.h"
#include "utilities-ds.h"

void print_test_result(int result);
static void insert_search_free_alphas(uint64_t num_inserts,
				      float alphas[],
				      int num_alphas,
				      int key_size,
				      int elt_size,
				      int (*cmp_key_fn)(void *, void *),
				      void (*cstr_elt_fn)(void *, uint64_t),
				      uint64_t (*elt_val_fn)(void *),
				      void (*free_elt_fn)(void *));

/**
   Run tests of a hash table on distinct keys and uint64_t elements across 
   key sizes and load factor upper bounds. A pointer to an element is passed 
   as elt in ht_mul_uint64_insert and the element is fully copied into a 
   block pointed from the node at the slot index computed by a hash function.
   A NULL as free_elt_fn is sufficient to delete the element.
*/

int cmp_uint64_fn(void *a, void *b){
  return memcmp(a, b, sizeof(uint64_t));
}
/*
int cmp_32_fn(void *a, void *b){
  return memcmp(a, b, 32);
}

int cmp_256_fn(void *a, void *b){
  return memcmp(a, b, 256);
}
*/
/**
   Constructs an uint64_t element. The elt parameter is a pointer that
   points to a preallocated block of size elt_size, i.e. sizeof(uint64_t).
*/
void cstr_uint64_fn(void *elt, uint64_t val){
  uint64_t *s = elt;
  *s = val;
  s = NULL;
}

uint64_t val_uint64_fn(void *elt){
  return *(uint64_t *)elt;
}

/**
   Runs a ht_mul_uint64_{insert, search, free} test on distinct keys and 
   uint64_t elements across key sizes and load factor upper bounds.
*/
void run_insert_search_free_uint64_elt_test(){
  uint64_t num_inserts = 1000000; //< 2^63 for this test
  float alphas[4] = {0.1, 0.2, 0.4, 0.8};
  int num_alphas = 4;
  //key_size >= sizeof(uint64_t) for this test
  int key_sizes[1] = {sizeof(uint64_t)}; 
  int num_key_sizes = 1;
  int (*cmp_fn_arr[1])(void *, void *) = {cmp_uint64_fn};
  for (int i = 0; i < num_key_sizes; i++){
    printf("Run a ht_mul_uint64_{insert, search, free} test on distinct "
	   "%d-byte keys and uint64_t elements\n", key_sizes[i]);
    insert_search_free_alphas(num_inserts,
			      alphas,
			      num_alphas,
			      key_sizes[i],
			      sizeof(uint64_t),
			      cmp_fn_arr[i],
			      cstr_uint64_fn,
			      val_uint64_fn,
			      NULL);
  }
}

/**
   Run tests of a hash table on distinct keys and multilayered uint64_ptr_t 
   elements across key sizes and load factor upper bounds. A pointer to a 
   pointer to an element is passed as elt in ht_mul_uint64_insert, and
   the pointer to the element is copied into a block pointed from the node at
   the slot index computed by a hash function. An element-specific 
   free_elt_fn is necessary to delete the element.
*/

typedef struct{
  uint64_t *val;
} uint64_ptr_t;

/**
   Constructs an uint64_ptr_t element. The elt parameter is a pointer that
   points to a preallocated block of size elt_size, i.e. 
   sizeof(uint64_ptr_t *).
*/
void cstr_uint64_ptr_fn(void *elt, uint64_t val){
  uint64_ptr_t **s = elt;
  *s = malloc(sizeof(uint64_ptr_t));
  assert(*s != NULL);
  (*s)->val = malloc(sizeof(uint64_t));
  assert((*s)->val != NULL);
  *((*s)->val) = val;
  s = NULL;
}

uint64_t val_uint64_ptr_fn(void *elt){
  uint64_ptr_t **s  = elt;
  return *((*s)->val);
}

/**
   Frees an uint64_ptr_t element and leaves a block of size elt_size pointed 
   to by the elt parameter.
*/
void free_uint64_ptr_fn(void *elt){
  uint64_ptr_t **s = elt;
  free((*s)->val);
  (*s)->val = NULL;
  free(*s);
  *s = NULL;
  s = NULL;
}

/**
   Run a ht_mul_uint64_{insert, search, free} test on distinct keys and 
   multilayered uint64_ptr_t elements across key sizes and load factor 
   upper bounds.
*/
void run_insert_search_free_uint64_ptr_elt_test(){
  uint64_t num_inserts = 1000000; //< 2^63 for this test
  float alphas[4] = {0.1, 0.2, 0.4, 0.8};
  int num_alphas = 4;
  //key_size >= sizeof(uint64_t) for this test
  int key_sizes[1] = {sizeof(uint64_t)}; 
  int num_key_sizes = 1;
  int (*cmp_fn_arr[1])(void *, void *) = {cmp_uint64_fn};
  for (int i = 0; i < num_key_sizes; i++){
    printf("Run a ht_mul_uint64_{insert, search, free} test on distinct "
	   "%d-byte keys and multilayered uint64_ptr_t elements\n",
	   key_sizes[i]);
    insert_search_free_alphas(num_inserts,
			      alphas,
			      num_alphas,
			      key_sizes[i],
			      sizeof(uint64_ptr_t *),
			      cmp_fn_arr[i],
			      cstr_uint64_ptr_fn,
			      val_uint64_ptr_fn,
			      free_uint64_ptr_fn);
  }
}

/** 
   Helper functions for the ht_mul_uint64_{insert, search, free} tests
   across key sizes and load factor upper bounds, on uint64_t and 
   uint64_ptr_t elements.
*/

static void insert_keys_elts(ht_mul_uint64_t *ht,
			     void **key_arr,
			     void **elt_arr,
			     uint64_t arr_size,
			     int *result){
  uint64_t n = ht->num_elts;
  clock_t t;
  t = clock();
  for (uint64_t i = 0; i < arr_size; i++){
    ht_mul_uint64_insert(ht, key_arr[i], elt_arr[i]);
  }
  t = clock() - t;
  printf("\t\tinsert time:                    "
	 "%.4f seconds\n", (float)t / CLOCKS_PER_SEC);
  *result *= (ht->num_elts == n + arr_size);
}

static void search_in_ht(ht_mul_uint64_t *ht,
			 void **key_arr,
			 void **elt_arr,
			 uint64_t arr_size,
			 int *result,
			 uint64_t (*elt_val_fn)(void *)){
  uint64_t n = ht->num_elts;
  void *elt;
  clock_t t;
  t = clock();
  for (uint64_t i = 0; i < arr_size; i++){
    elt = ht_mul_uint64_search(ht, key_arr[i]);
    *result *= (elt_val_fn(elt_arr[i]) == elt_val_fn(elt));
  }
  t = clock() - t;
  printf("\t\tin ht search time:              "
	 "%.4f seconds\n", (float)t / CLOCKS_PER_SEC);
  *result *= (ht->num_elts == n);
}

static void search_not_in_ht(ht_mul_uint64_t *ht,
			     void **key_arr,
			     uint64_t arr_size,
			     int *result){
  uint64_t n = ht->num_elts;
  void *elt;
  clock_t t;
  t = clock();
  for (uint64_t i = 0; i < arr_size; i++){
    elt = ht_mul_uint64_search(ht, key_arr[i]);
    *result *= (elt == NULL);
  }
  t = clock() - t;
  printf("\t\tnot in ht search time:          "
	 "%.4f seconds\n", (float)t / CLOCKS_PER_SEC);
  *result *= (ht->num_elts == n);
}

static void free_ht(ht_mul_uint64_t *ht){
  clock_t t;
  t = clock();
  ht_mul_uint64_free(ht);
  t = clock() - t;
  printf("\t\tfree time:                      "
	 "%.4f seconds\n", (float)t / CLOCKS_PER_SEC);
}

static void insert_search_free(uint64_t num_inserts,
			       float alpha,
			       int key_size,
			       int elt_size,
			       int (*cmp_key_fn)(void *, void *),
			       void (*cstr_elt_fn)(void *, uint64_t),
			       uint64_t (*elt_val_fn)(void *),
			       void (*free_elt_fn)(void *)){
  assert(key_size >= (int)sizeof(uint64_t));
  ht_mul_uint64_t ht;
  void **key_arr, **elt_arr, *ptr;
  int result = 1;
  //construct keys and elements to avoid construction in timing exps.
  key_arr = malloc(num_inserts * sizeof(void *));
  assert(key_arr != NULL);
  elt_arr = malloc(num_inserts * sizeof(void *));
  assert(elt_arr != NULL);
  for (uint64_t i = 0; i < num_inserts; i++){
    key_arr[i] = malloc(key_size);
    assert(key_arr[i] != NULL);
    ptr = (char *)(key_arr[i] + key_size - sizeof(uint64_t));
    *(uint64_t *)ptr = i;
    elt_arr[i] = malloc(elt_size);
    assert(elt_arr[i] != NULL);
    cstr_elt_fn(elt_arr[i], i);
  }
  ht_mul_uint64_init(&ht,
                     key_size,
	             elt_size,
		     alpha,
                     cmp_key_fn,
		     NULL,
		     free_elt_fn);
  insert_keys_elts(&ht, key_arr, elt_arr, num_inserts, &result);
  search_in_ht(&ht, key_arr, elt_arr, num_inserts, &result, elt_val_fn);
  assert(num_inserts < pow_two_uint64(63));
  for (uint64_t i = 0; i < num_inserts; i++){
    ptr = (char *)(key_arr[i] + key_size - sizeof(uint64_t));
    *(uint64_t *)ptr = i + num_inserts;
  }
  search_not_in_ht(&ht, key_arr, num_inserts, &result);
  free_ht(&ht);
  printf("\t\tsearch correctness:             ");
  print_test_result(result);
  //free key_arr and elt_arr
  for (uint64_t i = 0; i < num_inserts; i++){
    free(key_arr[i]);
    free(elt_arr[i]);
  }
  free(key_arr);
  free(elt_arr);
}

static void insert_search_free_alphas(uint64_t num_inserts,
				      float alphas[],
				      int num_alphas,
				      int key_size,
				      int elt_size,
				      int (*cmp_key_fn)(void *, void *),
				      void (*cstr_elt_fn)(void *, uint64_t),
				      uint64_t (*elt_val_fn)(void *),
				      void (*free_elt_fn)(void *)){
  float alpha;
  for (int i = 0; i < num_alphas; i++){
    alpha = alphas[i];
    printf("\tnumber of inserts: %lu, load factor upper bound: %.1f\n",
	   num_inserts, alpha);
    insert_search_free(num_inserts,
		       alpha,
		       key_size,
		       elt_size,
		       cmp_key_fn,
		       cstr_elt_fn,
		       elt_val_fn,
		       free_elt_fn);
  }
}

/**
   Print test result.
*/
void print_test_result(int result){
  if (result){
    printf("SUCCESS\n");
  }else{
    printf("FAILURE\n");
  }
}

int main(){
  run_insert_search_free_uint64_elt_test();
  run_insert_search_free_uint64_ptr_elt_test();
  return 0;
}
