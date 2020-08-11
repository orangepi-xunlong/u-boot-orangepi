#ifndef  __SPRITE_AUTO_UPDATE_H__
#define  __SPRITE_AUTO_UPDATE_H__

static inline void *malloc_aligned(u32 size, u32 alignment)
{
     void *ptr = (void*)malloc(size + alignment);
      if (ptr)
      {
            void * aligned =(void *)(((long)ptr + alignment) & (~(alignment-1)));

            /* Store the original pointer just before aligned pointer*/
            ((void * *) aligned) [-1]  = ptr;
             return aligned;
      }

      return NULL;
}

static inline void free_aligned(void *aligned_ptr)
{
     if (aligned_ptr)
        free (((void * *) aligned_ptr) [-1]);
}

extern int fat_fs_read(const char *filename, void *addr, int offset, int len);

#endif	/* __SPRITE_AUTO_UPDATE_H__ */
