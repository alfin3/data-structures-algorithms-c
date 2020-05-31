/**
   dijkstra.c

   Functions for running Dijkstra's algorithm on a graph with generic 
   non-negative weights.

   Edge weights are of any basic type (e.g. char, int, double, long double).
   Edge weight initialization and operations are defined in init_wt_fn, 
   add_wt_fn, and cmp_wt_fn functions. 
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "dijkstra.h"

static int *vt_ptr(adj_lst_t *a, void *vts, int i);
static void *wt_ptr(adj_lst_t *a, void *wts, int i);
static int cmp_vt_fn(void *a, void *b){return *(int *)a - *(int *)b;}
static void free_vt_fn(void *a){}

/**
   Computes and copies the shortest distances from s to a distance array.
*/
void dijkstra(adj_lst_t *a,
	      int s,
	      void *dist,
	      void (*init_wt_fn)(int, int, void *),
	      void (*add_wt_fn)(void *, void *, void *),
	      int (*cmp_wt_fn)(void *, void *)){
  for (int i = 0; i < a->num_vts; i++){
    init_wt_fn(s, i, wt_ptr(a, dist, i));
  }
  if (a->wts == NULL){return;} //no edges
  heap_t h;
  int h_size = 1;
  int vt_size = sizeof(int);
  int wt_size = a->wt_size;
  int u;
  int v;
  void *wt = malloc(wt_size); //edge weight buffer
  assert(wt != NULL);
  bool *in_heap = calloc(a->num_vts, sizeof(bool));
  assert(in_heap != NULL);
  heap_init(&h, h_size, vt_size, wt_size, cmp_vt_fn, cmp_wt_fn, free_vt_fn);
  heap_push(&h, &s, wt_ptr(a, dist, s));
  in_heap[s] = true;
  while (h.num_elts > 0){
    heap_pop(&h, &u, wt);
    in_heap[u] = false;
    for (int i = 0; i < a->vts[u]->num_elts; i++){
      v = *vt_ptr(a, a->vts[u]->elts, i);
      //u was popped => wt is not infinity
      add_wt_fn(wt,
		wt_ptr(a, dist, u),
		wt_ptr(a, a->wts[u]->elts, i));
      if (cmp_wt_fn(wt_ptr(a, dist, v), wt) > 0){
	memcpy(wt_ptr(a, dist, v), wt, wt_size);
	if (in_heap[v]){
	  heap_update(&h, &v, wt_ptr(a, dist, v));
	}else{
	  heap_push(&h, &v, wt_ptr(a, dist, v));
	  in_heap[v] = true;
	}
      }
    }
  }
  heap_free(&h);
  free(wt);
  free(in_heap);
  wt = NULL;
  in_heap = NULL;
}

/** Helper functions */

/**
   Computes a pointer to an entry in an array of vertices.
*/
static int *vt_ptr(adj_lst_t *a, void *vts, int i){
  return (int *)((char *)vts + i * sizeof(int));
}

/**
   Computes a pointer to an entry in an array of weights.
*/
static void *wt_ptr(adj_lst_t *a, void *wts, int i){
  return (void *)((char *)wts + i * a->wt_size);
}
