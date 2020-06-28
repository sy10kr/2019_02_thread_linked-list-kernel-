#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/string.h>

#define SKIPLIST_MAX_LEVEL 6
#define RAND_MAX 3000
#define TEST_SIZE_THREAD 25000
#define TEST_SIZE 100000
int insert_stack_1 = 0;
int insert_stack_2 = 0;
int insert_stack_3 = 0;

int search_check_1_1 = 0;
int search_check_1_2 = 0;
int search_check_2_1 = 0;
int search_check_2_2 = 0;
int search_check_3_1 = 0;
int search_check_3_2 = 0;

spinlock_t counter_lock;
spinlock_t counter_lock_tr;

struct timespec64 t0_thread,t1_thread;
long long s_t_thread;
long long e_t_thread;

struct timespec64 t0_thread_d,t1_thread_d;
long long s_t_thread_d;
long long e_t_thread_d;

// 자료구조를 위한 전역
int count = 0;
int del_value = 0;
int del_flag = 0;
int th_stop = 1;
int traverse_stop = 0;

struct task_struct *thread_1_1;
struct task_struct *thread_1_2;
struct task_struct *thread_2_1;
struct task_struct *thread_2_2;
struct task_struct *thread_3_1;
struct task_struct *thread_3_2;

// 자료구조를 위한 전역


//============================================================================================================
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//============================================================================================================자료형
struct hello_node {
    int num;
    struct list_head list;
};

struct head_node {
    //int del_flag = 0;
    struct list_head list_one;
    struct list_head list_two;
    struct list_head list_three;
};
//struct head_node *h_node;

void list_head_init(struct head_node *list){ 

    INIT_LIST_HEAD(&(list->list_one));
    INIT_LIST_HEAD(&(list->list_two));
    INIT_LIST_HEAD(&(list->list_three));
}
//============================================================================================================자료형
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//============================================================================================================삽입기능
int thread_3_insert(struct head_node *h_node, int i){

    int step;
    struct hello_node *node;

    step = count % 3;

    if (step == 0) {
        node = kmalloc(sizeof(struct hello_node), GFP_KERNEL);
        node->num = i;
        list_add(&node->list, &(h_node->list_one));
        insert_stack_1++;
    }
    if (step == 1) {
        node = kmalloc(sizeof(struct hello_node), GFP_KERNEL);
        node->num = i;
        list_add(&node->list, &(h_node->list_two));
        insert_stack_2++;
    }
    if (step == 2) {
        node = kmalloc(sizeof(struct hello_node), GFP_KERNEL);
        node->num = i;
        list_add(&node->list, &(h_node->list_three));
        insert_stack_3++;
    }
    count++;
    return 0;
}
//============================================================================================================삽입기능
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//============================================================================================================서치기능
int search_thread1_1(void *arg){
    struct hello_node *node;
    struct list_head *ptr_1, *ptrn_1;
    struct list_head *list1 = (struct list_head *)arg;
    
    list_for_each(ptr_1, list1) {
        node = list_entry(ptr_1, struct hello_node, list);
        //printk("순회쓰레드1_1: %d", node->num);
        search_check_1_1++;
        if((insert_stack_1/2) <= search_check_1_1)
            break;
    }

    spin_lock(&counter_lock_tr);
    traverse_stop++;
    spin_unlock(&counter_lock_tr);

    search_check_1_1 = 0;
    do_exit(0);
}

int search_thread1_2(void *arg){
    struct hello_node *node;
    struct list_head *ptr_1, *ptrn_1;
    struct list_head *list1 = (struct list_head *)arg;
    
    list_for_each_prev(ptr_1, list1) {
        node = list_entry(ptr_1, struct hello_node, list);
        //printk("순회쓰레드1_2: %d", node->num);
        search_check_1_2++;
        if(((insert_stack_1+1)/2) <= search_check_1_2)
            break;
    }

    spin_lock(&counter_lock_tr);
    traverse_stop++;
    spin_unlock(&counter_lock_tr);
    search_check_1_2 = 0;
    do_exit(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int search_thread2_1(void *arg){
    struct hello_node *node;
    struct list_head *ptr_1, *ptrn_1;
    struct list_head *list1 = (struct list_head *)arg;
    
    list_for_each(ptr_1, list1) {
        node = list_entry(ptr_1, struct hello_node, list);
        //printk("순회쓰레드2_1: %d", node->num);
        search_check_2_1++;
        if((insert_stack_2/2) <= search_check_2_1)
            break;
    }

    //ktime_get_real_ts64(&t1_thread);
    spin_lock(&counter_lock_tr);
    traverse_stop++;
    spin_unlock(&counter_lock_tr);
    search_check_2_1 = 0;
    do_exit(0);
}

int search_thread2_2(void *arg){
    struct hello_node *node;
    struct list_head *ptr_1, *ptrn_1;
    struct list_head *list1 = (struct list_head *)arg;
    
    list_for_each_prev(ptr_1, list1) {
        node = list_entry(ptr_1, struct hello_node, list);
        //printk("순회쓰레드2_2: %d", node->num);
        search_check_2_2++;
        if(((insert_stack_2+1)/2) <= search_check_2_2)
            break;
    }

    //ktime_get_real_ts64(&t1_thread);
    spin_lock(&counter_lock_tr);
    traverse_stop++;
    spin_unlock(&counter_lock_tr);
    search_check_2_2 = 0;
    do_exit(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int search_thread3_1(void *arg){
    struct hello_node *node;
    struct list_head *ptr_1, *ptrn_1;
    struct list_head *list1 = (struct list_head *)arg;
    
    list_for_each(ptr_1, list1) {
        node = list_entry(ptr_1, struct hello_node, list);
        //printk("순회쓰레드3_1: %d", node->num);
        search_check_3_1++;
        if((insert_stack_3/2) <= search_check_3_1)
            break;
    }

    //ktime_get_real_ts64(&t1_thread);
    spin_lock(&counter_lock_tr);
    traverse_stop++;
    spin_unlock(&counter_lock_tr);
    search_check_3_1 = 0;
    do_exit(0);
}

int search_thread3_2(void *arg){
    struct hello_node *node;
    struct list_head *ptr_1, *ptrn_1;
    struct list_head *list1 = (struct list_head *)arg;
    
    list_for_each_prev(ptr_1, list1) {
        node = list_entry(ptr_1, struct hello_node, list);
        //printk("순회쓰레드3_2:% d", node->num);
        search_check_3_2++;
        if(((insert_stack_3+1)/2) <= search_check_3_2)
            break;
    }


    spin_lock(&counter_lock_tr);
    traverse_stop++;
    spin_unlock(&counter_lock_tr);
    search_check_3_2 = 0;
    do_exit(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int thread_3_traverse_all(struct head_node *head){
    int wait = 0;
    kthread_run(&search_thread1_1, (void*)&(head->list_one), "search_thread");
    kthread_run(&search_thread1_2, (void*)&(head->list_one), "search_thread");
    kthread_run(&search_thread2_1, (void*)&(head->list_two), "search_thread");
    kthread_run(&search_thread2_2, (void*)&(head->list_two), "search_thread");
    kthread_run(&search_thread3_1, (void*)&(head->list_three), "search_thread");
    kthread_run(&search_thread3_2, (void*)&(head->list_three), "search_thread");

    
    
    while(traverse_stop != 6 ){
        msleep(1);
    }


    traverse_stop = 0;
    return 0;
}
//============================================================================================================서치기능
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//============================================================================================================삭제기능
int del_thread_1(void *arg){
    struct hello_node *node;
    struct list_head *ptr_1, *ptrn_1;
    struct list_head *list1 = (struct list_head *)arg;
    
    list_for_each_safe(ptr_1, ptrn_1, list1) {
        node = list_entry(ptr_1, struct hello_node, list);
        int temp_flag = 0;
        if(th_stop == 0)
            break;

        if(del_value == node->num){
            spin_lock(&counter_lock);
            if(del_flag == 1){
                spin_unlock(&counter_lock);
                del_flag = 0;
                list_del(ptr_1);
                kfree(node);
                th_stop = 0;
                insert_stack_1--;
            }
        }
        search_check_1_1++;
        if((insert_stack_1/2) <= search_check_1_1)
            break;
    }
    search_check_1_1 = 0;
    do_exit(0);
}

int del_thread_1_prev(void *arg){
    struct hello_node *node;
    struct list_head *ptr_1, *ptrn_1;
    struct list_head *list1 = (struct list_head *)arg;
    
    list_for_each_prev_safe(ptr_1, ptrn_1, list1) {
        node = list_entry(ptr_1, struct hello_node, list);
        int temp_flag = 0;
        if(th_stop == 0)
            break;

        if(del_value == node->num){
            spin_lock(&counter_lock);
            if(del_flag == 1){
                spin_unlock(&counter_lock);
                del_flag = 0;
                list_del(ptr_1);
                kfree(node);
                th_stop = 0;
                insert_stack_1--;
            }
            
        }
        search_check_1_2++;
        if(((insert_stack_1+1)/2) <= search_check_1_1)
            break;
    }
    search_check_1_2 = 0;
    do_exit(0);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int del_thread_2(void *arg){
    struct hello_node *node;
    struct list_head *ptr_1, *ptrn_1;
    struct list_head *list1 = (struct list_head *)arg;
    
    list_for_each_safe(ptr_1, ptrn_1, list1) {
        node = list_entry(ptr_1, struct hello_node, list);
        int temp_flag = 0;
        if(th_stop == 0)
            break;

        if(del_value == node->num){
            spin_lock(&counter_lock);
            if(del_flag == 1){
                spin_unlock(&counter_lock);
                del_flag = 0;
                list_del(ptr_1);
                kfree(node);
                th_stop = 0;
                insert_stack_2--;
            }
        }
        search_check_2_1++;
        if((insert_stack_2/2) <= search_check_2_1)
            break;
    }
    search_check_2_1 = 0;
    do_exit(0);
}

int del_thread_2_prev(void *arg){
    struct hello_node *node;
    struct list_head *ptr_1, *ptrn_1;
    struct list_head *list1 = (struct list_head *)arg;
    
    list_for_each_prev_safe(ptr_1, ptrn_1, list1) {
        node = list_entry(ptr_1, struct hello_node, list);
        int temp_flag = 0;
        if(th_stop == 0)
            break;

        if(del_value == node->num){
            spin_lock(&counter_lock);
            if(del_flag == 1){
                spin_unlock(&counter_lock);
                del_flag = 0;
                list_del(ptr_1);
                kfree(node);
                th_stop = 0;
                insert_stack_2--;
            }
        }
        search_check_2_2++;
        if(((insert_stack_2+1)/2) <= search_check_2_1)
            break;
    }
    search_check_2_2 = 0;
    do_exit(0);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int del_thread_3(void *arg){
    struct hello_node *node;
    struct list_head *ptr_1, *ptrn_1;
    struct list_head *list1 = (struct list_head *)arg;
    
    list_for_each_safe(ptr_1, ptrn_1, list1) {
        node = list_entry(ptr_1, struct hello_node, list);
        int temp_flag = 0;
        if(th_stop == 0)
            break;

        if(del_value == node->num){
            spin_lock(&counter_lock);
            if(del_flag == 1){
                spin_unlock(&counter_lock);
                del_flag = 0;
                list_del(ptr_1);
                kfree(node);
                th_stop = 0;
                insert_stack_3--;
            }
        }
        search_check_3_1++;
        if((insert_stack_3/2) <= search_check_3_1)
            break;
    }
    search_check_3_1 = 0;
    do_exit(0);
}

int del_thread_3_prev(void *arg){
    struct hello_node *node;
    struct list_head *ptr_1, *ptrn_1;
    struct list_head *list1 = (struct list_head *)arg;
    
    list_for_each_prev_safe(ptr_1, ptrn_1, list1) {
        node = list_entry(ptr_1, struct hello_node, list);
        int temp_flag = 0;
        if(th_stop == 0)
            break;

        if(del_value == node->num){
            spin_lock(&counter_lock);
            if(del_flag == 1){
                spin_unlock(&counter_lock);
                del_flag = 0;
                list_del(ptr_1);
                kfree(node);
                th_stop = 0;
                insert_stack_3--;
            }
        }
        search_check_3_2++;
        if(((insert_stack_3+1)/2) <= search_check_3_1)
            break;
    }
    search_check_3_2 = 0;
    do_exit(0);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int thread_3_del(struct head_node *head, int val){

    del_value = val;
    del_flag = 1;
    th_stop = 1;
    thread_1_1 = kthread_run(&del_thread_1, (void*)&(head->list_one), "search_thread");
    thread_2_1 = kthread_run(&del_thread_2, (void*)&(head->list_two), "search_thread");
    thread_3_1 = kthread_run(&del_thread_3, (void*)&(head->list_three), "search_thread");

    thread_1_2 = kthread_run(&del_thread_1_prev, (void*)&(head->list_one), "search_thread");
    thread_2_2= kthread_run(&del_thread_2_prev, (void*)&(head->list_two), "search_thread");
    thread_3_2 = kthread_run(&del_thread_3_prev, (void*)&(head->list_three), "search_thread");

    while(th_stop != 0){
        msleep(1);
    }

    th_stop = 1;
    return 0;
}
//============================================================================================================삭제기능
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//============================================================================================================메인
int __init hello_module_init(void)
{    
    printk("=======================================");
    struct timespec64 t0,t1;
    long long s_t;
    long long e_t;


    int i;
    struct hello_node *node;

    struct head_node *h_node = kmalloc(sizeof(struct head_node), GFP_KERNEL);

    list_head_init(h_node);

    ktime_get_real_ts64(&t0);

    for (i = 0; i < 30000000; i++) {
        thread_3_insert(h_node, i);
    }
    ktime_get_real_ts64(&t1);
    s_t = (long long)t0.tv_sec*1000000000 + (long long)t0.tv_nsec;
    e_t = (long long)t1.tv_sec*1000000000 + (long long)t1.tv_nsec;
    printk("InsertㅡwithㅡThread:   %lld ns", e_t - s_t);

/////////////////////////////////////////////////--

    ktime_get_real_ts64(&t0_thread);

    thread_3_traverse_all(h_node);
 
    ktime_get_real_ts64(&t1_thread);
    s_t_thread = (long long)t0_thread.tv_sec*1000000000 + (long long)t0_thread.tv_nsec;
    e_t_thread = (long long)t1_thread.tv_sec*1000000000 + (long long)t1_thread.tv_nsec;
    printk("TraverseㅡwithㅡThread: %lld ns", e_t_thread - s_t_thread);


    //msleep(100);

/////////////////////////////////////////////////--


    ktime_get_real_ts64(&t0_thread);

    thread_3_del(h_node, 13777);

    ktime_get_real_ts64(&t1_thread);
    s_t_thread = (long long)t0_thread.tv_sec*1000000000 + (long long)t0_thread.tv_nsec;
    e_t_thread = (long long)t1_thread.tv_sec*1000000000 + (long long)t1_thread.tv_nsec;
    printk("DeleteㅡwithㅡThread:   %lld ns", e_t_thread - s_t_thread);

    //delete_all(&(h_node->list_one));
    //delete_all(&(h_node->list_two));
/*
    printk("=======================================");
    thread_3_traverse_all(h_node);
*/
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
    struct list_head my_list;
    INIT_LIST_HEAD(&my_list);
    ktime_get_real_ts64(&t0);
    int a;
    for(a=0; a<30000000; a++)
    {
        struct hello_node *new = kmalloc(sizeof(struct hello_node), GFP_KERNEL);
        new->num = a;
        list_add(&new->list, &my_list);
    }
    ktime_get_real_ts64(&t1);
    s_t = (long long)t0.tv_sec*1000000000 + (long long)t0.tv_nsec;
    e_t = (long long)t1.tv_sec*1000000000 + (long long)t1.tv_nsec;
    printk("Linked List Insert:     %lld ns", e_t - s_t);


    struct hello_node *current_node;
    ktime_get_real_ts64(&t0);
    struct list_head *p, *p1;
    list_for_each(p, &my_list)
    {
        current_node = list_entry(p, struct hello_node, list);
    }

    ktime_get_real_ts64(&t1);
    s_t = (long long)t0.tv_sec*1000000000 + (long long)t0.tv_nsec;
    e_t = (long long)t1.tv_sec*1000000000 + (long long)t1.tv_nsec;
    printk("Linked List Traverse:   %lld ns", e_t - s_t);

    ktime_get_real_ts64(&t0);
    list_for_each_safe(p, p1, &my_list)
    {
        current_node = list_entry(p, struct hello_node, list);
        if(current_node->num == 13777){
            list_del(p);
            kfree(p);
        }
    }
    ktime_get_real_ts64(&t1);
    s_t = (long long)t0.tv_sec*1000000000 + (long long)t0.tv_nsec;
    e_t = (long long)t1.tv_sec*1000000000 + (long long)t1.tv_nsec;
    printk("Linked List Delete:     %lld ns", e_t - s_t);

    list_for_each_safe(p, p1, &(h_node->list_one))
    {
        current_node = list_entry(p, struct hello_node, list);
        list_del(p);
        kfree(current_node);
    }

    list_for_each_safe(p, p1, &(h_node->list_two))
    {
        current_node = list_entry(p, struct hello_node, list);
        list_del(p);
        kfree(current_node);
    }

    list_for_each_safe(p, p1, &(h_node->list_three))
    {
        current_node = list_entry(p, struct hello_node, list);
        list_del(p);
        kfree(current_node);
    }


////////////////////////////////////////
    list_for_each_safe(p, p1, &my_list)
    {
        current_node = list_entry(p, struct hello_node, list);
        list_del(p);
        kfree(current_node);
    }
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void __exit hello_module_cleanup(void)
{
	printk("Bye Module_ver0.1\n");
}
module_init(hello_module_init);
module_exit(hello_module_cleanup);

MODULE_LICENSE("GPL");
