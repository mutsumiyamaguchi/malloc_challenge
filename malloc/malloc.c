//
// >>>> malloc challenge! <<<<
//
// Your task is to improve utilization and speed of the following malloc
// implementation.
// Initial implementation is the same as the one implemented in simple_malloc.c.
// For the detailed explanation, please refer to simple_malloc.c.

// メモ
// 前と後ろにmetadataをおいておけば左結合の実装をすることできる
// 今はfreeをするときに右結合のみ実装している。その際次のアドレスを取得して、while分でlinkedlistに含まれているかを探索し、含まれていれば右結合をするということをしている
// metadataの中にfreelist_flagのようなfreelistであるかを明記するフラグを格納しておいてその中身を確認するだけにすれば実行時間は短くなるはず
// utilizationは小さくなってしまうかもしれない

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<math.h>

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
  // prevとusedかどうかのフラグをメタデータに格納しておいて、freeの時に次のアドレスのこの要素を参照することで、一つ前の要素と使われているかどうかの判断をするようにすると
  // 実行時間がはやくなる
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

  // サイズの分布を確認
  // printf("size = %zu\n", size);

  // sizeのlogをとることで、indexが小さい範囲にはsizeの範囲が小さくなり、（8~15など）
  // indexの大きい範囲にはsizeの範囲が大きく格納されるので今回受け取る引数の分布に適したlinked listを作成することができる
  // メモリ配列を9個用意(sizeの範囲が8~4096なので)

  int index = (int)(log2(size));
      index -= 3;

  // segmentation fault予防
  if (index < 0) {
    // indexが小さすぎたら0にする
    index = 0;
  }
  if (index > 8){
    // indexの範囲が8より大きくなったら8にする
    index = 8;
  }

  // sizeを1000で割った数に対応させる
  // int index = size/1000;

  // // それ以上であれば3にする
  // if (index > 3){
  //   index = 3;
  // }

  return index;
 
}

//
// Helper functions (feel free to add/remove/edit!)
//

// void my_add_to_free_list(my_metadata_t *metadata) {
//   assert(!metadata->next);
//   metadata->next = my_heap.free_head;
//   my_heap.free_head = metadata;
// }

// 9つのheapに対応させる
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

// 9つのheapに対応させる
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

// 9つのheap全て初期化する必要がある
void my_initialize() {
  for (int i = 0; i < 9; ++i) {
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

// heap9つに対応
void *my_malloc(size_t size) {

  int index = get_index(size);
  my_metadata_t *metadata = NULL;
  my_metadata_t *prev = NULL;

  //bestfit格納用の変数を用意
  my_metadata_t *bestfit = NULL;
  my_metadata_t *bestfit_prev = NULL;

  for(int i = index;i < 9;i++){

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
  // metadataだけだと構造体自体のサイズを返してしまうので、castする必要がある
  my_metadata_t *metadata_next = (my_metadata_t *)((char *)metadata + sizeof(my_metadata_t) + metadata->size);

  // metadata_nextが4096の倍数であるかを確認しないと、今私が触っていい場所以外の部分を参照している可能性がある
  // metadata_nextの宣言の時に*がついていたらそれはポインタなのでアドレスが入っている
  // if metadata_next % 4096 == 0ならmetadata_nextがフリーであるかを探索するという内容を実装

  // free_listであるかを探索する必要があるかを保存するフラグ
  bool free_flag = false;

  // もしmetadata_nextの中身が4096の倍数であれば、flagにtrueを入れてwhile分を回すようにする（free_listであるかの探索をする）
  if ((uintptr_t)metadata_next % 4096 == 0){
    free_flag = true;
  }

 
  // 一つ前の要素を格納するバッファを用意
  my_metadata_t *buf = NULL;
  // sizeからfreelistとして含まれている可能性のあるindexを取得
  int index = get_index(metadata_next->size);
  my_metadata_t *nowlist = my_heap[index].free_head;

   // フラグがtrueなら右隣のアドレスがfreelistに含まれているか探索する
  while ((nowlist) && (free_flag)){

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
    // いずれNULLになるので絶対にwhileは抜ける
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
