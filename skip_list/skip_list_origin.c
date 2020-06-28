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
#define TEST_SIZE_THREAD 20000
#define TEST_SIZE 120000

struct timespec64 t0_thread,t1_thread;
long long s_t_thread;
long long e_t_thread;
    

struct hello_node {
    int num;
    struct list_head list;
};


int rand_denominator;

static int rand(void){
	return  get_random_u32()/3001;
}

typedef struct snode {
    int key;
    int value;
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
        return 0;
    } else {
        level = rand_level();
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
    for (i = list->level; i >= 1; i--) {
        while (x->forward[i]->key < key)
            x = x->forward[i];
    }
    if (x->forward[1]->key == key) {
        return x->forward[1];
    } else {
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
 
int skiplist_delete(skiplist *list, int key) {
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
        for (i = 1; i <= list->level; i++) {
            if (update[i]->forward[i] != x)
                break;
            update[i]->forward[1] = x->forward[i];
        }
        skiplist_node_free(x);
 
        while (list->level > 1 && list->header->forward[list->level]
                == list->header)
            list->level--;
        return 0;
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
    //kiplist_dump(&list);

    /*
    ktime_get_real_ts64(&t0);
    snode *x = skiplist_search(&list, 7000);
    ktime_get_real_ts64(&t1);
    s_t = (long long)t0.tv_sec*1000000000 + (long long)t0.tv_nsec;
    e_t = (long long)t1.tv_sec*1000000000 + (long long)t1.tv_nsec;
    printk("Search1000 skiplist: %lld ns", e_t - s_t);
    */
}

void thread_create(void)
{
	int i;
	/* thread create */
	for(i=0; i<6; i++){
        int* arg = (int*)kmalloc(sizeof(int),GFP_KERNEL);
        *arg = i;
        kthread_run(&test_thread,(void*)arg,"test_thread");
	}
}

int __init hello_module_init(void)
{    
    skiplist list;
    skiplist_init(&list);
    struct timespec64 t0,t1;
    long long s_t;
    long long e_t;
    int i;

    struct hello_node *node;
    struct list_head *ptr, *ptrn;

    ktime_get_real_ts64(&t0_thread);
    thread_create();

    ktime_get_real_ts64(&t0);
    for (i = 0; i < TEST_SIZE; i++) {
        skiplist_insert(&list, i, i);
    }
    ktime_get_real_ts64(&t1);
    s_t = (long long)t0.tv_sec*1000000000 + (long long)t0.tv_nsec;
    e_t = (long long)t1.tv_sec*1000000000 + (long long)t1.tv_nsec;
    printk("Insert1000 skiplist: %lld ns", e_t - s_t);


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
    printk("Insert1000 linked: %lld ns", e_t - s_t);

        /* traverse */
    ktime_get_real_ts64(&t0);
    list_for_each_safe(ptr, ptrn, &insert_1000) {
        node = list_entry(ptr, struct hello_node, list);
        if(node->num == 7000)
            break;
    }
    ktime_get_real_ts64(&t1);
    s_t = (long long)t0.tv_sec*1000000000 + (long long)t0.tv_nsec;
    e_t = (long long)t1.tv_sec*1000000000 + (long long)t1.tv_nsec;
    printk("Search1000 linked: %lld ns", e_t - s_t);
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
	return 0;
}

void __exit hello_module_cleanup(void)
{
	printk("Bye Module\n");
}
module_init(hello_module_init);
module_exit(hello_module_cleanup);


