#ifndef INCLUDE_GRAPH_GRAPAOPS_H
#define INCLUDE_GRAPH_GRAPAOPS_H
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int VertexId;
typedef double Weight;

typedef struct {
  VertexId t;
  VertexId h;
  Weight w;
} Edge;

typedef struct GraphInfoOps {
  int (*vertex_count)(const void* G);
  int (*edge_count)(const void* G);
  bool (*valid_vertex)(const void* G, VertexId v);
  bool (*directed)(const void* G);
} GraphInfoOps;

typedef struct GraphQueryOps {
  bool (*adjacent)(const void* G, VertexId v1, VertexId v2);
  Edge (*first_neighbor)(const void* G, VertexId v);
  Edge (*next_neighbor)(const void* G, VertexId v, VertexId w);
  Edge* (*all_edges)(const void* G);
} GraphQueryOps;

typedef struct GraphMutateOps {
  VertexId (*add_vert)(void* G);
  bool (*delete_vert)(void* G, VertexId v);
  bool (*add_edge)(void* G, VertexId v1, VertexId v2, Weight w);
  bool (*remove_edge)(void* G, VertexId v1, VertexId v2);
} GraphMutateOps;

typedef struct WeightedGraphOps {
  Weight (*update_edge_weight)(void* G, VertexId v1, VertexId v2, Weight w);
  Weight (*get_edge_weight)(const void* G, VertexId v1, VertexId v2);
} WeightedGraphOps;

typedef struct {
  GraphInfoOps iops;
  GraphQueryOps qops;
  GraphMutateOps mops;
  WeightedGraphOps wops;
} BaseGraph;

#ifdef __cplusplus
}
#endif

// typedef enum {
//   WEIGHT_INT,
//   WEIGHT_DOUBLE,
//   WEIGHT_FLOAT,
// } WeightType;
//
// typedef struct {
//   WeightType type;
//   union {
//     int i;
//     double d;
//     float f;
//   } value;
// } Weight;
#endif  // !INCLUDE_GRAPH_GRAPAOPS_H
