#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/resource.h>



typedef struct pair_node{
    int  *arr;
    int n;
} pair;
int n_t = 8;

double CPU_TIME() {
        struct rusage ruse;
        getrusage(RUSAGE_SELF, &ruse);
        return ruse.ru_utime.tv_sec+ruse.ru_utime.tv_usec/1000000.0 + ruse.ru_stime.tv_sec+ruse.ru_stime.tv_usec/1000000.0;
}
void merge(int *a, int n, int m){

        int i, j, k;
        int *x = malloc(n* sizeof(int));
        for (i=0, j =m, k=0; k <n ; k++){
                x[k] = j == n ? a[i++]
                         : i == m ? a[j++]
                         : a[j] < a[i] ? a[j++]
                         : a[i++];
        }

        for ( i = 0; i< n ; i++) {
                a[i] = x[i];
        }

        free(x);
}

void merge_sort(int *a, int n) {
        if (n < 2)
                return;
        int m = n /2;
        merge_sort(a,m);
        merge_sort(a+m, n-m);
        merge(a,n,m);


}
void* thread_implement(void* val){
        struct pair_node *arg = (struct pair_node *)val;
        int n = arg->n;
        int *a = arg->arr;
        if (n < 2)
                return;
        int m = n /2;
        if(n_t<2){
            merge_sort(a,m);
            merge_sort(a+m, n-m);
        } else {
            n_t = n_t -2;
            pthread_t thread[2];
         struct pair_node** arg1 = malloc(sizeof(pair*) * 2); // for two threads split
            arg1[0] = malloc(sizeof(pair));
            arg1[1] = malloc(sizeof(pair));
             arg1[0]->arr = a;
            arg1[0]->n = m;
            arg1[1]->arr = a+m;
            arg1[1]->n = n-m;

           int i;
            for(i = 0; i < 2; i++) pthread_create(&thread[i], NULL, thread_implement, arg1[i]);
            for(i = 0; i < 2; i++) pthread_join(thread[i], NULL);

        }
        merge(a,n,m);
}
int main(){
        //clock_t begin = clock();
        struct timeval start, end;
        long delta;


        pthread_t t;
        int i, size;
        /* here, do your time-consuming job */
        printf("\nEnter work size (integer):");
        scanf("%d", &size);

        int a[size], arr_t1[size];
        for(i=0; i<size; i++){
                a[i] = rand()%200;
        }
        for(i=0; i<size; i++){
                arr_t1[i] = a[i];
        }
        gettimeofday(&start, NULL);
        printf("\nBefore merge_sort");

        merge_sort(a, size);
        gettimeofday(&end, NULL);
       // printf("\nSEQ SORTED ARRAY:");
       // for(i=0; i<size; i++){
                //a[i] = rand();
         //     printf("%d\t", a[i]);
        //}       
        delta = end.tv_usec - start.tv_usec;
        printf("\nSeq Time is : %ld microseconds\n",delta);

        struct pair_node *val = malloc(sizeof(struct pair_node));
        val->arr = arr_t1;
        val->n = size;
        gettimeofday(&start, NULL);
        pthread_create(&t, NULL, thread_implement, val);
        pthread_join(t, NULL);
        gettimeofday(&end, NULL);
                delta = end.tv_usec - start.tv_usec;

        //printf("\nPAR SORTED ARRAY:");
        //for(i=0; i<size; i++)
        //         printf("%d\t", a[i]);

        printf("\nParallel Time is: %ld microseconds\n",delta);

        free(val);
        return 0;
}
~                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
~            
