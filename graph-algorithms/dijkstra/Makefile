CC = gcc
DS_DIR = ../../data-structures/
ALG_DIR = ../
BFS_DIR   = $(ALG_DIR)bfs/
GRAPH_DIR = $(DS_DIR)graph/
HEAP_DIR  = $(DS_DIR)heap/
HT_DIV_UINT64_DIR = $(DS_DIR)ht-div-uint64/
HT_MUL_UINT64_DIR = $(DS_DIR)ht-mul-uint64/
DLL_DIR   = $(DS_DIR)dll/
QUEUE_DIR = $(DS_DIR)queue/
STACK_DIR = $(DS_DIR)stack/
UTILS_MEM_DIR = ../../utilities/utilities-mem/
UTILS_MOD_DIR = ../../utilities/utilities-mod/
CFLAGS = -I$(BFS_DIR)                          \
         -I$(GRAPH_DIR)                        \
         -I$(HEAP_DIR)                         \
         -I$(HT_DIV_UINT64_DIR)                \
         -I$(HT_MUL_UINT64_DIR)                \
         -I$(DLL_DIR)                          \
         -I$(QUEUE_DIR)                        \
         -I$(STACK_DIR)                        \
         -I$(UTILS_MEM_DIR)                    \
         -I$(UTILS_MOD_DIR)                    \
         -std=c90 -Wall -Wextra -Wpedantic -O3

OBJ = dijkstra-main.o                     \
      dijkstra.o                          \
      $(BFS_DIR)bfs.o                     \
      $(GRAPH_DIR)graph.o                 \
      $(HEAP_DIR)heap.o                   \
      $(HT_DIV_UINT64_DIR)ht-div-uint64.o \
      $(HT_MUL_UINT64_DIR)ht-mul-uint64.o \
      $(DLL_DIR)dll.o                     \
      $(QUEUE_DIR)queue.o                 \
      $(STACK_DIR)stack.o                 \
      $(UTILS_MEM_DIR)utilities-mem.o     \
      $(UTILS_MOD_DIR)utilities-mod.o

dijkstra : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ 

dijkstra-main.o                     : dijkstra.h                          \
                                      $(BFS_DIR)bfs.h                     \
                                      $(HEAP_DIR)heap.h                   \
                                      $(HT_DIV_UINT64_DIR)ht-div-uint64.h \
                                      $(HT_MUL_UINT64_DIR)ht-mul-uint64.h \
                                      $(GRAPH_DIR)graph.h                 \
                                      $(STACK_DIR)stack.h                 \
                                      $(UTILS_MEM_DIR)utilities-mem.h     \
                                      $(UTILS_MOD_DIR)utilities-mod.h
dijkstra.o                          : dijkstra.h                          \
                                      $(GRAPH_DIR)graph.h                 \
                                      $(HEAP_DIR)heap.h                   \
                                      $(STACK_DIR)stack.h                 \
                                      $(UTILS_MEM_DIR)utilities-mem.h
$(BFS_DIR)bfs.o                     : $(BFS_DIR)bfs.h                     \
                                      $(GRAPH_DIR)graph.h                 \
                                      $(QUEUE_DIR)queue.h                 \
                                      $(STACK_DIR)stack.h                 \
                                      $(UTILS_MEM_DIR)utilities-mem.h
$(GRAPH_DIR)graph.o                 : $(GRAPH_DIR)graph.h                 \
                                      $(STACK_DIR)stack.h                 \
                                      $(UTILS_MEM_DIR)utilities-mem.h
$(HEAP_DIR)heap.o                   : $(HEAP_DIR)heap.h                   \
                                      $(UTILS_MEM_DIR)utilities-mem.h
$(HT_DIV_UINT64_DIR)ht-div-uint64.o : $(HT_DIV_UINT64_DIR)ht-div-uint64.h \
                                      $(DLL_DIR)dll.h                     \
                                      $(UTILS_MEM_DIR)utilities-mem.h     \
                                      $(UTILS_MOD_DIR)utilities-mod.h
$(HT_MUL_UINT64_DIR)ht-mul-uint64.o : $(HT_MUL_UINT64_DIR)ht-mul-uint64.h \
                                      $(DLL_DIR)dll.h                     \
                                      $(UTILS_MEM_DIR)utilities-mem.h     \
                                      $(UTILS_MOD_DIR)utilities-mod.h
$(DLL_DIR)dll.o                     : $(DLL_DIR)dll.h                     \
                                      $(UTILS_MEM_DIR)utilities-mem.h
$(QUEUE_DIR)queue.o                 : $(QUEUE_DIR)queue.h                 \
                                      $(UTILS_MEM_DIR)utilities-mem.h
$(STACK_DIR)stack.o                 : $(STACK_DIR)stack.h                 \
                                      $(UTILS_MEM_DIR)utilities-mem.h
$(UTILS_MEM_DIR)utilities-mem.o     : $(UTILS_MEM_DIR)utilities-mem.h
$(UTILS_MOD_DIR)utilities-mod.o     : $(UTILS_MOD_DIR)utilities-mod.h

.PHONY : clean clean-all

clean :
	rm $(OBJ)
clean-all : 
	rm -f dijkstra $(OBJ)
