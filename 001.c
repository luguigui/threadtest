   /*********************
      * 共享内存+多线程+线程信号量 应用例子
      */
      #include <stdio.h>
      #include <string.h>
      #include <stdlib.h>
      #include <pthread.h>
      #include <semaphore.h>
      #include <sys/types.h>
      #include <sys/shm.h>
      #include <sys/ipc.h>

      sem_t bin_sem; //信号量
      int end_flag;  //结束标志符
      char *sh_ptr;

      /* 向out文件中写数据 */
      void* Tid_write(void *a)
      {
      FILE *fp;
      fp = fopen("out", "w");
      while(end_flag)
      {
      sem_wait(&bin_sem);
      if(sh_ptr[0] != 0)
      {
      fprintf(fp, "%s", sh_ptr);
      memset(sh_ptr, 0, sizeof(sh_ptr));
      }
      sem_post(&bin_sem);
      }
      fclose(fp);
      return (void*)0;
      }

      /* 从in文件中读数据 */
      void *Tid_read(void *arg)
      {
      FILE *fp;
      fp = fopen("in", "r");
      while(!feof(fp))
      {
      sem_wait(&bin_sem);
      if(sh_ptr[0] == 0)
      {
      fgets(sh_ptr, 10, fp);
      //printf("(%s)\n", sh_ptr);
      }
      sem_post(&bin_sem);
      }
      fclose(fp);
      end_flag = 0;
      return (void *)0;
      }

      int main()
      {
      int shm_id;
      end_flag = 1;
      pthread_t tid_1, tid_2;
      int err_0, err_1, err_2;

      shm_id = shmget(IPC_PRIVATE, 15, IPC_CREAT|0600); //建立共享内存
      if(shm_id == -1)
      {
      fprintf(stderr, "error");
      return 0;
      }
      sh_ptr = (char *)shmat(shm_id, NULL, 0);
      memset(sh_ptr, 0, sizeof(sh_ptr));

      err_0 = sem_init(&bin_sem, 0, 1);// 初始化信号量
      if(err_0 < 0)
      fprintf(stderr, "sem_init error!");

      err_1 = pthread_create(&tid_1, NULL, Tid_write, NULL);
      err_2 = pthread_create(&tid_2, NULL, Tid_read, NULL);

      if(err_1!=0 || err_2!=0)
      fprintf(stderr, "pthread error!");

      pthread_join(tid_1, NULL);
      pthread_join(tid_2, NULL);
      shmdt(sh_ptr); //解除共享内存绑定
      sem_destroy(&bin_sem);//释放信号量
      printf("project is over!\n");
      return 0;
      }
