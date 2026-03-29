#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "graph/GraphAlgorithms.h"
#include "graph/GraphOps.h"
#include "stack/SqStack.h"

VertexId* TopologicalSort(const void* graph_context,
                          const BaseGraph* graph_methods) {
  VertexId* res = NULL;
  SqStack* s = NULL;
  Weight* mat = NULL;
  int* restrict indegree = NULL;

  if (graph_context == NULL || graph_methods == NULL) goto cleanup;
  const void* G = graph_context;
  const BaseGraph* bg = graph_methods;

  ptrdiff_t nv = bg->iops.vertex_count(G);

  res = (VertexId*)malloc(sizeof(VertexId) * nv);
  bool ok = InitStack(&s, sizeof(VertexId), nv);
  if (!ok) goto cleanup;
  mat = bg->qops.get_materix(G);
  if (mat == NULL) goto cleanup;
  indegree = (int*)calloc(nv, sizeof(int));
  if (indegree == NULL) goto cleanup;

  for (ptrdiff_t i = 0; i < nv; i++) {
    const Weight* restrict row = &mat[i * nv];
    for (ptrdiff_t j = 0; j < nv; j++) {
      if (row[j] > 0) indegree[j] += 1;
    }
  }
  for (VertexId i = 0; i < nv; i++) {
    if (indegree[i] == 0) {
      bool push_ok = Push(s, &i);
      if (!push_ok) {
        fprintf(stderr,
                "Interal Error: Failed to Push Vertex id into stack!\n");
        goto cleanup;
      }
    }
  }

  int count = 0;
  while (!isStackEmpty(s)) {
    VertexId curr = -1;
    bool pop_ok = Pop(s, &curr);
    if (!pop_ok) {
      fprintf(stderr, "Internal Error: Trying to Pop from an empty stack.\n");
      goto cleanup;
    }
    res[count++] = curr;
    Weight* restrict row = &mat[nv * curr];
    for (ptrdiff_t j = 0; j < nv; j++) {
      if (row[j] <= 0) continue;

      indegree[j]--;
      if (indegree[j] != 0) continue;

      VertexId new_v = (VertexId)j;
      bool push_ok = Push(s, &new_v);
      if (!push_ok) {
        fprintf(stderr,
                "Interal Error: Failed to Push Vertex id into stack!\n");
        goto cleanup;
      }
    }
  }
  if (count < (int)nv) {  // circle exists in the graph
    free(res);
    res = NULL;
  }

cleanup:
  if (s) DestroyStack(&s);
  if (mat) free(mat);
  if (indegree) free(indegree);
  return res;
}
