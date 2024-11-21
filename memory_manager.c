#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/swap.h>
#include <linux/swapops.h>
#include <linux/mm_types.h>
#include <linux/pid.h>
#include <asm/pgtable.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aakash Khepar");

static int pid = 0;
module_param(pid, int, 0);
static unsigned long long addr = 0;
module_param(addr, ullong, 0);

static pte_t *page_table_walk(int pid, unsigned long long addr) {
    struct task_struct *task;
    struct mm_struct *mm;
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    task = pid_task(find_get_pid(pid), PIDTYPE_PID);
    if (!task) {
        return NULL;
    }
    mm = task->mm;
    if (!mm) {
        return NULL;
    }
    pgd = pgd_offset(mm, addr);
    if (pgd_none(*pgd) || unlikely(pgd_bad(*pgd))) {
        return NULL;
    }
    p4d = p4d_offset(pgd, addr);
    if (p4d_none(*p4d) || unlikely(p4d_bad(*p4d))) {
        return NULL;
    }
    pud = pud_offset(p4d, addr);
    if (pud_none(*pud) || unlikely(pud_bad(*pud))) {
        return NULL;
    }
    pmd = pmd_offset(pud, addr);
    if (pmd_none(*pmd) || unlikely(pmd_bad(*pmd))) {
        return NULL;
    }
    pte = pte_offset_kernel(pmd, addr);
    return pte;
}

static int __init memory_manager_init(void) {
    pte_t *pte = page_table_walk(pid, addr);
    if (!pte) {
        printk(KERN_INFO "[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [NA] swap identifier [NA]\n", pid, addr);
    } else {
        if (pte_present(*pte)) {
            unsigned long pfn = pte_pfn(*pte);
            unsigned long physical_addr = (pfn << PAGE_SHIFT) | (addr & ((1UL << PAGE_SHIFT) - 1));
            printk(KERN_INFO "[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [%lx] swap identifier [NA]\n", pid, addr, physical_addr);
        } else {
            swp_entry_t entry = pte_to_swp_entry(*pte);
            printk(KERN_INFO "[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [NA] swap identifier [%lx]\n", pid, addr, entry.val);
        }
    }
    return 0;
}

static void __exit memory_manager_exit(void) {}

module_init(memory_manager_init);
module_exit(memory_manager_exit);
