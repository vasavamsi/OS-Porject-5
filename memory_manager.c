#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/sched/task.h>
#include <linux/swapops.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vamsi Krishna Vasa");
MODULE_DESCRIPTION("Memory Manager Module");

static int pid = -1;
module_param(pid, int, 0644);

static unsigned long long addr = 0;
module_param(addr, ullong, 0644);

static int __init memory_manager_init(void) {
    struct task_struct *task;
    struct mm_struct *mm;
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    unsigned long physical_addr;
    swp_entry_t swap_entry;

    // Find the process by PID
    task = get_pid_task(find_get_pid(pid), PIDTYPE_PID);
    if (!task) {
        printk(KERN_INFO "[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [NA] swap identifier [NA]\n", 
               pid, addr);
        return -ESRCH;
    }

    // Get the memory descriptor
    mm = task->mm;
    if (!mm) {
        put_task_struct(task);
        printk(KERN_INFO "[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [NA] swap identifier [NA]\n", 
               pid, addr);
        return -EINVAL;
    }

    // Walk the page table
    pgd = pgd_offset(mm, addr);
    if (pgd_none(*pgd) || unlikely(pgd_bad(*pgd))) {
        put_task_struct(task);
        printk(KERN_INFO "[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [NA] swap identifier [NA]\n", 
               pid, addr);
        return 0;
    }

    p4d = p4d_offset(pgd, addr);
    if (p4d_none(*p4d) || unlikely(p4d_bad(*p4d))) {
        put_task_struct(task);
        printk(KERN_INFO "[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [NA] swap identifier [NA]\n", 
               pid, addr);
        return 0;
    }

    pud = pud_offset(p4d, addr);
    if (pud_none(*pud) || unlikely(pud_bad(*pud))) {
        put_task_struct(task);
        printk(KERN_INFO "[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [NA] swap identifier [NA]\n", 
               pid, addr);
        return 0;
    }

    pmd = pmd_offset(pud, addr);
    if (pmd_none(*pmd) || unlikely(pmd_bad(*pmd))) {
        put_task_struct(task);
        printk(KERN_INFO "[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [NA] swap identifier [NA]\n", 
               pid, addr);
        return 0;
    }

    pte = pte_offset_kernel(pmd, addr);
    if (!pte || pte_none(*pte)) {
        put_task_struct(task);
        printk(KERN_INFO "[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [NA] swap identifier [NA]\n", 
               pid, addr);
        return 0;
    }

    // Check if page is present in memory
    if (pte_present(*pte)) {
        physical_addr = (pte_pfn(*pte) << PAGE_SHIFT) | (addr & ~PAGE_MASK);
        printk(KERN_INFO "[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [%lx] swap identifier [NA]\n", 
               pid, addr, physical_addr);
    }
    // Check if page is in swap
    else {
        swap_entry = pte_to_swp_entry(*pte);
        printk(KERN_INFO "[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [NA] swap identifier [%lx]\n", 
               pid, addr, swap_entry.val);
    }

    put_task_struct(task);
    return 0;
}

static void __exit memory_manager_exit(void) {
    // Cleanup code (if needed)
}

module_init(memory_manager_init);
module_exit(memory_manager_exit);
