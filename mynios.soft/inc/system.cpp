void abort(void)
{
  while(1);
}

void jmp(int val){
  __asm__ __volatile__("jmp %[input1]"::[input1]"r"(val));
}

void setOff(int val){
  __asm__ __volatile__("stoff %[input1]"::[input1]"r"(val));
}

void* __eof__();
int malloc_index = 0;
void* malloc(int size){
  size = (size & (~0x03))+4;
  int idx = (int)__eof__();
  idx += malloc_index;
  malloc_index += size;
  return (void*)idx;
}

void mfree(int size){
  malloc_index -= size;
}

void flushCache(void* addr){
  int tmp = (int)addr;
  tmp = tmp >> 10;
  int status = sti(0);
  IOWR(CACHE_CTL, 0, 0x80000000 | tmp);
  volatile int a = *((int*)addr);
  IOWR(CACHE_CTL, 0, 0);
  if(status){
    sti(1);
  }
}

void memcpy(void *dst, const void *src, int len){
  for(int i=0;i<len;i++){
    ((char*)(dst))[i] = ((char*)(src))[i];
  }
}
