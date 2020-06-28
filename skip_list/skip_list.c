#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h> // for thread
#include <linux/slab.h> // for kmalloc
#include <linux/delay.h>
#include <linux/random.h>
#include <linux/time.h>

#define SKIPLIST_MAX_LEVEL 6
#define RAND_MAX 3000
#define TEST_SIZE_THREAD 25000
#define TEST_SIZE 100000

struct timespec64 t0_thread,t1_thread;
long long s_t_thread;
long long e_t_thread;

int count = 0;
int sizt_t = 0;

struct hello_node {
    int num;
    struct list_head list;
};


/********************SKIPLIST REFERENCE*******************/

int rand_denominator;

static int rand(void){
	return  get_random_u32()/3001;
}

typedef struct snode {
    int key;
    int value;
    int dup_stack; // min define
    struct snode **forward;
} snode;
 
typedef struct skiplist {
    int level;
    int size;
    struct snode *header;
} skiplist;
 
skiplist *skiplist_init(skiplist *list) {
    int i;
    snode *header = (snode *) kmalloc(sizeof(struct snode), GFP_KERNEL);
    list->header = header;
    header->key = INT_MAX;
    header->forward = (snode **) kmalloc(
            sizeof(snode*) * (SKIPLIST_MAX_LEVEL + 1), GFP_KERNEL);
    for (i = 0; i <= SKIPLIST_MAX_LEVEL; i++) {
        header->forward[i] = list->header;
    }
 
    list->level = 1;
    list->size = 0;
 
    return list;
}
 
static int rand_level(void) {
    int level = 1;
    while (rand() < RAND_MAX / 2 && level < SKIPLIST_MAX_LEVEL)
        level++;
    return level;
}
 
int skiplist_insert(skiplist *list, int key, int value) {
    snode *update[SKIPLIST_MAX_LEVEL + 1];
    snode *x = list->header;
    int i, level;
    for (i = list->level; i >= 1; i--) {
        while (x->forward[i]->key < key)
            x = x->forward[i];
        update[i] = x;
    }
    x = x->forward[1];
 
    if (key == x->key) {
        x->value = value;
        x->dup_stack = x->dup_stack + 1; // min define
        return 0;
    } else {
        level = rand_level();
        x->dup_stack = 1; // min define
        if (level > list->level) {
            for (i = list->level + 1; i <= level; i++) {
                update[i] = list->header;
            }
            list->level = level;
        }
 
        x = (snode *) kmalloc(sizeof(snode), GFP_KERNEL);
        x->key = key;
        x->value = value;
        x->forward = (snode **) kmalloc(sizeof(snode*) * (level + 1), GFP_KERNEL);
        for (i = 1; i <= level; i++) {
            x->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = x;
        }
    }
    return 0;
}
 
snode *skiplist_search(skiplist *list, int key) {
    snode *x = list->header;
    int i;
  //  printk("오류3");
    for (i = list->level; i >= 1; i--) {
        while (x->forward[i]->key < key)
            x = x->forward[i];
    }
   // printk("오류4");
    if (x->forward[1]->key == key) {
        //printk("찾음");
        return x->forward[1];
    } else {
         //printk("오류5");
        return NULL;
    }
    return NULL;
}
 
static void skiplist_node_free(snode *x) {
    if (x) {
        kfree(x->forward);
        kfree(x);
    }
}
 
int skiplist_delete(skiplist *list, int key) { // min define
    int i;
    snode *update[SKIPLIST_MAX_LEVEL + 1];
    snode *x = list->header;
    for (i = list->level; i >= 1; i--) {
        while (x->forward[i]->key < key)
            x = x->forward[i];
        update[i] = x;
    }
 
    x = x->forward[1];
    if (x->key == key) {
        if(x->dup_stack < 2){
            for (i = 1; i <= list->level; i++) {
                if (update[i]->forward[i] != x)
                    break;
                update[i]->forward[1] = x->forward[i];
            }
            skiplist_node_free(x);
 
            while (list->level > 1 && list->header->forward[list->level] == list->header){
                list->level--;
            }
            return 0;
        }
        else{
            x->dup_stack = x->dup_stack - 1; // min define
            return 0;
        }

    }
    return 1;
}
 
static void skiplist_dump(skiplist *list) {
    snode *x = list->header;
    while (x && x->forward[1] != list->header) {
        printk("%d[%d]->", x->forward[1]->key, x->forward[1]->value);
        x = x->forward[1];
    }
    printk("NIL\n");
}

/********************SKIPLIST REFERENCE*******************/


/********************THREAD SKIPLIST HEADER DEFINE*******************/
typedef struct skiplist_thread_4 {
    skiplist *s_1;
    skiplist *s_2;
    skiplist *s_3;
    skiplist *s_4;
} skiplist_thread_4;

skiplist_thread_4 *skiplist_thread_4_init(skiplist_thread_4 *thread_list){
    skiplist *header_1 = (skiplist *) kmalloc(sizeof(struct skiplist), GFP_KERNEL);
    skiplist *header_2 = (skiplist *) kmalloc(sizeof(struct skiplist), GFP_KERNEL);
    skiplist *header_3 = (skiplist *) kmalloc(sizeof(struct skiplist), GFP_KERNEL);
    skiplist *header_4 = (skiplist *) kmalloc(sizeof(struct skiplist), GFP_KERNEL);

    int list_size = 0;

    thread_list->s_1 = header_1;
    thread_list->s_2 = header_2;
    thread_list->s_3 = header_3;
    thread_list->s_4 = header_4;

    skiplist_init(thread_list->s_1);
    skiplist_init(thread_list->s_2);
    skiplist_init(thread_list->s_3);
    skiplist_init(thread_list->s_4);

    return thread_list;
}

int skiplist_thread_4_insert(skiplist_thread_4 *thread_list, int key, int value){

    //skiplist *insert_target;
    //int i;
    //int min;
    //int num = 1;
    int step;

    step = count % 4;

    if (step == 0) {
         skiplist_insert(thread_list->s_1, key, value);
         sizt_t++;
    }
    if (step == 1) {
         skiplist_insert(thread_list->s_1, key, value);
    }
    if (step == 2) {
         skiplist_insert(thread_list->s_1, key, value);
    }
    if (step == 3) {
         skiplist_insert(thread_list->s_1, key, value);
    }
    count++;

/*
    if (thread_list->s_2->size < min) {
        insert_target = thread_list->s_2;
         min = thread_list->s_2->size;
         num = 2;
    }
    if (thread_list->s_3->size < min) {
        insert_target = thread_list->s_3;
         min = thread_list->s_3->size;
         num = 3;
    }
    if (thread_list->s_4->size < min) {
        insert_target = thread_list->s_4;
         min = thread_list->s_4->size;
         num = 4;
    }
    */

    //insert_target->size = insert_target->size + 1;
    //printk("insert list num: %d", key);
    //skiplist_insert(insert_target, key, value);
    return 0;
}


int serach_key;

int search_thread(void *arg)
{

    ktime_get_real_ts64(&t0_thread);
    int i;
    skiplist *list = (skiplist*)arg;
    int key = serach_key;
    
    snode *x = list->header;
  //  printk("오류3");
    for (i = list->level; i >= 1; i--) {
        while (x->forward[i]->key < key)
            x = x->forward[i];
    }
   // printk("오류4");
    if (x->forward[1]->key == key) {
        printk("찾음");
            
    ktime_get_real_ts64(&t1_thread);
    s_t_thread = (long long)t0_thread.tv_sec*1000000000 + (long long)t0_thread.tv_nsec;
    e_t_thread = (long long)t1_thread.tv_sec*1000000000 + (long long)t1_thread.tv_nsec;
    printk("thread_______________________search 100000 skiplist: %lld ns", e_t_thread - s_t_thread);
        return 0;
    } else {
         //printk("오류5");
        return 0;
    }
    return 0;
}


void thread_skiplist_search(skiplist_thread_4 *thread_list, int key) {
    int i;

	/* thread create */
    serach_key = key;
    kthread_run(&search_thread, (void*)(thread_list->s_1), "search_thread");
}

/********************THREAD SKIPLIST HEADER DEFINE*******************/


/*
int test_thread(void *_arg)
{
    skiplist list;
    skiplist_init(&list);
    int i;

    
    //printk("Insert:--------------------\n");
    ktime_get_real_ts64(&t0_thread);
    for (i = 0; i < TEST_SIZE_THREAD; i++) {
        skiplist_insert(&list, i, i);
    }
    printk("________________End thead");
    ktime_get_real_ts64(&t1_thread);
    s_t_thread = (long long)t0_thread.tv_sec*1000000000 + (long long)t0_thread.tv_nsec;
    e_t_thread = (long long)t1_thread.tv_sec*1000000000 + (long long)t1_thread.tv_nsec;
    printk("thread_______________________Insert1000 skiplist: %lld ns", e_t_thread - s_t_thread);
}

void thread_create(void)
{
	int i;
	// thread create 
	for(i=0; i<4; i++){
        int* arg = (int*)kmalloc(sizeof(int),GFP_KERNEL);
        *arg = i;
        kthread_run(&test_thread,(void*)arg,"test_thread");
	}
}
*/


int __init hello_module_init(void)//////////////////////////////////////////////////////////////////////////
{    
    skiplist list;
    skiplist_init(&list);
    struct timespec64 t0,t1;
    long long s_t;
    long long e_t;
    int i;

    struct hello_node *node;
    struct list_head *ptr, *ptrn;

    skiplist_thread_4 list_s;
    skiplist_thread_4_init(&list_s);

    ktime_get_real_ts64(&t0);
    for (i = 0; i < 100000; i++) {
        skiplist_thread_4_insert(&list_s, i, i);
    }
    ktime_get_real_ts64(&t1);
    s_t = (long long)t0.tv_sec*1000000000 + (long long)t0.tv_nsec;
    e_t = (long long)t1.tv_sec*1000000000 + (long long)t1.tv_nsec;
    printk("Insert1000000 skiplist: %lld ns", e_t - s_t);

    printk("사이즈 %d", sizt_t);


    thread_skiplist_search(&list_s, 3000);

  
    //ktime_get_real_ts64(&t0_thread);
    //thread_create();
    /*
    ktime_get_real_ts64(&t0);
    for (i = 0; i < TEST_SIZE; i++) {
        skiplist_insert(&list, i, i);
    }
    ktime_get_real_ts64(&t1);
    s_t = (long long)t0.tv_sec*1000000000 + (long long)t0.tv_nsec;
    e_t = (long long)t1.tv_sec*1000000000 + (long long)t1.tv_nsec;
    printk("Insert100000 skiplist: %lld ns", e_t - s_t);


    ktime_get_real_ts64(&t0);
    skiplist_search(&list, 7000);
    ktime_get_real_ts64(&t1);
    s_t = (long long)t0.tv_sec*1000000000 + (long long)t0.tv_nsec;
    e_t = (long long)t1.tv_sec*1000000000 + (long long)t1.tv_nsec;
    printk("Search100000 skiplist: %lld ns", e_t - s_t);
    */


    /******************************************************************************************/
        /* create head */
    LIST_HEAD(insert_1000);
    ktime_get_real_ts64(&t0);
    /* add to the head */

    for (i = 0; i < TEST_SIZE; i++) {
        node = kmalloc(sizeof(struct hello_node), GFP_KERNEL);
        node->num = i;
        INIT_LIST_HEAD(&node->list);
        list_add(&node->list, &insert_1000);
    }
    ktime_get_real_ts64(&t1);
    s_t = (long long)t0.tv_sec*1000000000 + (long long)t0.tv_nsec;
    e_t = (long long)t1.tv_sec*1000000000 + (long long)t1.tv_nsec;
    printk("Insert100000 linked: %lld ns", e_t - s_t);

        /* traverse */
 
    ktime_get_real_ts64(&t0);
    list_for_each_safe(ptr, ptrn, &insert_1000) {
        node = list_entry(ptr, struct hello_node, list);
    }
    ktime_get_real_ts64(&t1);
    s_t = (long long)t0.tv_sec*1000000000 + (long long)t0.tv_nsec;
    e_t = (long long)t1.tv_sec*1000000000 + (long long)t1.tv_nsec;
    printk("Search100000 linked: %lld ns", e_t - s_t);
 
    /*
    printk("Search:--------------------\n");
    int keys[] = { 3, 4, 7, 10, 111 };
 
    for (i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
        snode *x = skiplist_search(&list, keys[i]);
        if (x) {
            printk("key = %d, value = %d\n", keys[i], x->value);
        } else {
            printk("key = %d, not fuound\n", keys[i]);
        }
    }

 
    printk("Search:--------------------\n");
    skiplist_delete(&list, 3);
    skiplist_delete(&list, 9);
    skiplist_dump(&list);

	printk(KERN_EMERG "Hello Module\n");
     */


        //ktime_get_real_ts64(&t0_thread);
    //thread_create();

    ktime_get_real_ts64(&t0);
    for (i = 0; i < TEST_SIZE; i++) {
        skiplist_insert(&list, i, i);
    }
    ktime_get_real_ts64(&t1);
    s_t = (long long)t0.tv_sec*1000000000 + (long long)t0.tv_nsec;
    e_t = (long long)t1.tv_sec*1000000000 + (long long)t1.tv_nsec;
    printk("Insert100000 skiplist: %lld ns", e_t - s_t);


    ktime_get_real_ts64(&t0);
    skiplist_search(&list, 70000);
    ktime_get_real_ts64(&t1);
    s_t = (long long)t0.tv_sec*1000000000 + (long long)t0.tv_nsec;
    e_t = (long long)t1.tv_sec*1000000000 + (long long)t1.tv_nsec;
    printk("Search100000 skiplist: %lld ns", e_t - s_t);

	return 0;
}

void __exit hello_module_cleanup(void)
{
	printk("Bye Module_ver0.1--------------------------------------\n");
}
module_init(hello_module_init);
module_exit(hello_module_cleanup);


