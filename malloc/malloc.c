//
// >>>> malloc challenge! <<<<
//
// Your task is to improve utilization and speed of the following malloc
// implementation.
// Initial implementation is the same as the one implemented in simple_malloc.c.
// For the detailed explanation, please refer to simple_malloc.c.

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// Interfaces to get memory pages from OS
//

void *mmap_from_system(size_t size);
void munmap_to_system(void *ptr, size_t size);

//
// Struct definitions
//

typedef struct my_metadata_t {
  size_t size;
  struct my_metadata_t *next;
} my_metadata_t;

typedef struct my_heap_t {
  my_metadata_t *free_head;
  my_metadata_t dummy;
} my_heap_t;

//
// Static variables (DO NOT ADD ANOTHER STATIC VARIABLES!)
//
// my_heap_t my_heap;
my_heap_t my_heap[4];
// heapの数を8に変更して実行してみたけれど変わらなかった。むしろ探索場所が増える分、実行時間が長くなった
// my_heap_t my_heap[8];

// サイズに応じてindexを返す関数
int get_index(size_t size) {

  // sizeを1000で割った数に対応させる
  int index = size/1000;

  // それ以上であれば3にする
  if (index > 3){
    index = 3;
  }
  return index;
  //  // それ以上であれば7にする
  // if (index > 7){
  //   index = 3;
  // }
 
}

//
// Helper functions (feel free to add/remove/edit!)
//

// void my_add_to_free_list(my_metadata_t *metadata) {
//   assert(!metadata->next);
//   metadata->next = my_heap.free_head;
//   my_heap.free_head = metadata;
// }

// 4つのheapに対応させる
void my_add_to_free_list(my_metadata_t *metadata) {
  assert(!metadata->next);
  int index = get_index(metadata -> size);
  metadata->next = my_heap[index].free_head;
  my_heap[index].free_head = metadata;
}

// void my_remove_from_free_list(my_metadata_t *metadata, my_metadata_t *prev) {
//   if (prev) {
//     prev->next = metadata->next;
//   } else {
//     my_heap.free_head = metadata->next;
//   }
//   metadata->next = NULL;
// }

// 4つのheapに対応させる
void my_remove_from_free_list(my_metadata_t *metadata, my_metadata_t *prev) {
  
  int index = get_index(metadata -> size);

  if (prev) {
    prev->next = metadata->next;
  } else {
    my_heap[index].free_head = metadata->next;
  }

  metadata->next = NULL;
}

//
// Interfaces of malloc (DO NOT RENAME FOLLOWING FUNCTIONS!)
//

// This is called at the beginning of each challenge.
// void my_initialize() {
//   my_heap.free_head = &my_heap.dummy;
//   my_heap.dummy.size = 0;
//   my_heap.dummy.next = NULL;
// }

// 4つのheap全て初期化する必要がある
void my_initialize() {
  for (int i = 0; i < 4; ++i) {
    my_heap[i].free_head = &my_heap[i].dummy;
    my_heap[i].dummy.size = 0;
    my_heap[i].dummy.next = NULL;
  }
  // for (int i = 0; i < 8; ++i) {
  //   my_heap[i].free_head = &my_heap[i].dummy;
  //   my_heap[i].dummy.size = 0;
  //   my_heap[i].dummy.next = NULL;
  // }
}


// my_malloc() is called every time an object is allocated.
// |size| is guaranteed to be a multiple of 8 bytes and meets 8 <= |size| <=
// 4000. You are not allowed to use any library functions other than
// mmap_from_system() / munmap_to_system().

// // bestfitに変更
// void *my_malloc(size_t size) {
//   my_metadata_t *metadata = my_heap.free_head;
//   my_metadata_t *prev = NULL;
//   // First-fit: Find the first free slot the object fits.
//   // TODO: Update this logic to Best-fit!
//   // while (metadata && metadata->size < size) {
//   //   prev = metadata;
//   //   metadata = metadata->next;
//   // }

//   //bestfit格納用の変数を用意
//   my_metadata_t *bestfit = NULL;
//   my_metadata_t *bestfit_prev = NULL;

//   while(metadata){
//     // もし欲しいサイズよりも大きいサイズのものを見つけたら
//     if (metadata->size >= size){
//       // bestfitが存在しなければそれを格納、またはbestfitの現在格納されているサイズよりも小さいものを見つけたら更新
//       if(bestfit == NULL || (bestfit -> size) > (metadata -> size) ){
//         bestfit = metadata;
//         bestfit_prev = prev;
//       }
//     }
//   }

  

  
//   // now, metadata points to the first free slot
//   // and prev is the previous entry.

//   if (!metadata) {
//     // There was no free slot available. We need to request a new memory region
//     // from the system by calling mmap_from_system().
//     //
//     //     | metadata | free slot |
//     //     ^
//     //     metadata
//     //     <---------------------->
//     //            buffer_size
//     size_t buffer_size = 4096;
//     my_metadata_t *metadata = (my_metadata_t *)mmap_from_system(buffer_size);
//     metadata->size = buffer_size - sizeof(my_metadata_t);
//     metadata->next = NULL;
//     // Add the memory region to the free list.
//     my_add_to_free_list(metadata);
//     // Now, try my_malloc() again. This should succeed.
//     return my_malloc(size);
//   }

//   // 最終的にmetadataをbestfitで見つけたものに変更
//   metadata = bestfit;
//   prev = bestfit_prev;

//   // |ptr| is the beginning of the allocated object.
//   //
//   // ... | metadata | object | ...
//   //     ^          ^
//   //     metadata   ptr
//   void *ptr = metadata + 1;
//   size_t remaining_size = metadata->size - size;
//   // Remove the free slot from the free list.
//   my_remove_from_free_list(metadata, prev);

//   if (remaining_size > sizeof(my_metadata_t)) {
//     // Shrink the metadata for the allocated object
//     // to separate the rest of the region corresponding to remaining_size.
//     // If the remaining_size is not large enough to make a new metadata,
//     // this code path will not be taken and the region will be managed
//     // as a part of the allocated object.
//     metadata->size = size;
//     // Create a new metadata for the remaining free slot.
//     //
//     // ... | metadata | object | metadata | free slot | ...
//     //     ^          ^        ^
//     //     metadata   ptr      new_metadata
//     //                 <------><---------------------->
//     //                   size       remaining size
//     my_metadata_t *new_metadata = (my_metadata_t *)((char *)ptr + size);
//     new_metadata->size = remaining_size - sizeof(my_metadata_t);
//     new_metadata->next = NULL;
//     // Add the remaining free slot to the free list.
//     my_add_to_free_list(new_metadata);
//   }
//   return ptr;
// }

// heap4つに対応
void *my_malloc(size_t size) {

  int index = get_index(size);
  my_metadata_t *metadata = NULL;
  my_metadata_t *prev = NULL;

  //bestfit格納用の変数を用意
  my_metadata_t *bestfit = NULL;
  my_metadata_t *bestfit_prev = NULL;

  for(int i = index;i < 4;i++){

    // 見つからなかった時のために、index以上のリストを探索できるようにする
    my_metadata_t *metadata = my_heap[i].free_head;
    my_metadata_t *prev = NULL;

    while(metadata){
    // もし欲しいサイズよりも大きいサイズのものを見つけたら
      if (metadata->size >= size){
      // bestfitが存在しなければそれを格納、またはbestfitの現在格納されているサイズよりも小さいものを見つけたら更新
        if(bestfit == NULL || (bestfit -> size) > (metadata -> size) ){
          bestfit = metadata;
          bestfit_prev = prev;
        }
      }
      prev = metadata;
      metadata = metadata->next;
    }

    // もしbestfitに該当するものが見つかったらroopを抜ける
    if(bestfit != NULL){
      break;
    }
  }
  // for(int i = index;i < 8;i++){

  //   // 見つからなかった時のために、index以上のリストを探索できるようにする
  //   my_metadata_t *metadata = my_heap[i].free_head;
  //   my_metadata_t *prev = NULL;

  //   while(metadata){
  //   // もし欲しいサイズよりも大きいサイズのものを見つけたら
  //     if (metadata->size >= size){
  //     // bestfitが存在しなければそれを格納、またはbestfitの現在格納されているサイズよりも小さいものを見つけたら更新
  //       if(bestfit == NULL || (bestfit -> size) > (metadata -> size) ){
  //         bestfit = metadata;
  //         bestfit_prev = prev;
  //       }
  //     }
  //     prev = metadata;
  //     metadata = metadata->next;
  //   }

  //   // もしbestfitに該当するものが見つかったらroopを抜ける
  //   if(bestfit != NULL){
  //     break;
  //   }
  // }
  
  // now, metadata points to the first free slot
  // and prev is the previous entry.

  if (!bestfit) {
    // There was no free slot available. We need to request a new memory region
    // from the system by calling mmap_from_system().
    //
    //     | metadata | free slot |
    //     ^
    //     metadata
    //     <---------------------->
    //            buffer_size
    size_t buffer_size = 4096;
    my_metadata_t *metadata = (my_metadata_t *)mmap_from_system(buffer_size);
    metadata->size = buffer_size - sizeof(my_metadata_t);
    metadata->next = NULL;
    // Add the memory region to the free list.
    my_add_to_free_list(metadata);
    // Now, try my_malloc() again. This should succeed.
    return my_malloc(size);
  }

  // 最終的にmetadataをbestfitで見つけたものに変更
  metadata = bestfit;
  prev = bestfit_prev;

  // |ptr| is the beginning of the allocated object.
  //
  // ... | metadata | object | ...
  //     ^          ^
  //     metadata   ptr
  void *ptr = metadata + 1;
  size_t remaining_size = metadata->size - size;
  // Remove the free slot from the free list.
  my_remove_from_free_list(metadata, prev);

  if (remaining_size > sizeof(my_metadata_t)) {
    // Shrink the metadata for the allocated object
    // to separate the rest of the region corresponding to remaining_size.
    // If the remaining_size is not large enough to make a new metadata,
    // this code path will not be taken and the region will be managed
    // as a part of the allocated object.
    metadata->size = size;
    // Create a new metadata for the remaining free slot.
    //
    // ... | metadata | object | metadata | free slot | ...
    //     ^          ^        ^
    //     metadata   ptr      new_metadata
    //                 <------><---------------------->
    //                   size       remaining size
    my_metadata_t *new_metadata = (my_metadata_t *)((char *)ptr + size);
    new_metadata->size = remaining_size - sizeof(my_metadata_t);
    new_metadata->next = NULL;
    // Add the remaining free slot to the free list.
    my_add_to_free_list(new_metadata);
  }
  return ptr;
}

// This is called every time an object is freed.  You are not allowed to
// use any library functions other than mmap_from_system / munmap_to_system.
void my_free(void *ptr) {
  // Look up the metadata. The metadata is placed just prior to the object.
  //
  // ... | metadata | object | ...
  //     ^          ^
  //     metadata   ptr

  // // 現在のコード
  // // メタデータが格納されているポインタを取得
  // my_metadata_t *metadata = (my_metadata_t *)ptr - 1;
  // // Add the free slot to the free list.
  // // その容量分メモリを解放
  // my_add_to_free_list(metadata);
  
  // 右側をmergeしたい
  // メタデータが格納されているポインタを取得
  my_metadata_t *metadata = (my_metadata_t *)ptr - 1;

  // 次のアドレスを参照して、その中身がfreelistに存在していたら結合するという処理を加える
  // TODO 
  // 下のコードだとmetadataの型が(my_metadata_t)型なので、(char *)metadata + (sizeof(my_metadata_t) + metadata->size) * sizeof(my_metadata_t)
  // と同じになってしまうらしい
  // my_metadata_t *metadata_next = (my_metadata_t *) metadata + sizeof(my_matadata_t) + metadata -> size ;
  // 正しいコードは以下の通りcastする必要がある
  my_metadata_t *metadata_next = (my_metadata_t *)((char *)metadata + sizeof(my_metadata_t) + metadata->size);

  // 右隣のアドレスがfreelistに含まれているか探索する
  // バッファを用意
  my_metadata_t *buf = NULL;
  // sizeからfreelistとして含まれている可能性のあるindexを取得
  int index = get_index(metadata_next->size);
  my_metadata_t *nowlist = my_heap[index].free_head;

  while (nowlist){

    // もしmetadata_next（右隣）がfreelistに含まれていたら、
    if(nowlist == metadata_next){

      // 一度削除して
      // removeの引数は現在のアドレスから一つ前のアドレス
      my_remove_from_free_list(nowlist,buf);
      // metadataの大きさをmetadataが格納されているメモリ+データを格納することができるメモリのサイズ分増やす
      metadata->size += sizeof(my_metadata_t) + nowlist->size;
    }

    // 一つ前の情報を現在探索中のアドレスに更新
    buf = nowlist;
    // 現在探索中のアドレスを次のアドレスに更新
    // いずれNULLになるので絶対にwhikeは抜ける
    nowlist = nowlist->next;

  }

  // 右側がfreeであれば拡張済み、そうでなければそのままのmetadataをfreelistとして追加
  my_add_to_free_list(metadata);

}

// This is called at the end of each challenge.
void my_finalize() {
  // Nothing is here for now.
  // feel free to add something if you want!
}

void test() {
  // Implement here!
  assert(1 == 1); /* 1 is 1. That's always true! (You can remove this.) */
}
