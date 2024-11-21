#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/swap.h>
#include <linux/mm_types.h>
#include <linux/pid.h>
#include <asm/pgtable.h>

static int pid = 0;
static unsigned long long addr = 0;

module_param(pid, int, 0);
module_param(addr, ullong, 0);

MODULE_PARM_DESC(pid, "PID of the process");
MODULE_PARM_DESC(addr, "Virtual address in the process");

static int __init memory_manager_init(void) {
    struct task_struct *task;
    struct mm_struct *mm;
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    
    // Retrieve the task_struct for the given PID
    task = pid_task(find_get_pid(pid), PIDTYPE_PID);
    if (!task) {
        pr_err("[CSE330-Memory-Manager] PID [%d] does not exist\n", pid);
        return -ESRCH;
    }

    mm = task->mm;
    if (!mm) {
        pr_err("[CSE330-Memory-Manager] PID [%d] does not have a valid memory struct\n", pid);
        return -EINVAL;
    }

    // Walk through the page table levels
    pgd = pgd_offset(mm, addr);
    if (pgd_none(*pgd) || pgd_bad(*pgd)) {
        goto invalid_page;
    }

    p4d = p4d_offset(pgd, addr);
    if (p4d_none(*p4d) || p4d_bad(*p4d)) {
        goto invalid_page;
    }

    pud = pud_offset(p4d, addr);
    if (pud_none(*pud) || pud_bad(*pud)) {
        goto invalid_page;
    }

    pmd = pmd_offset(pud, addr);
    if (pmd_none(*pmd) || pmd_bad(*pmd)) {
        goto invalid_page;
    }

    pte = pte_offset_kernel(pmd, addr);
    if (!pte) {
        goto invalid_page;
    }

    // Check if page is present in memory
    if (pte_present(*pte)) {
        unsigned long pfn = pte_pfn(*pte);
        unsigned long phys_addr = (pfn << PAGE_SHIFT) | (addr & ~PAGE_MASK);
        pr_info("[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [%lx] swap identifier [NA]\n",
                pid, addr, phys_addr);
    } else {
        // Page is in swap
        swp_entry_t entry = pte_to_swp_entry(*pte);
        pr_info("[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [NA] swap identifier [%lx]\n",
                pid, addr, entry.val);
    }
    return 0;

invalid_page:
    pr_info("[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [NA] swap identifier [NA]\n",
            pid, addr);
    return 0;
}

static void __exit memory_manager_exit(void) {
    pr_info("[CSE330-Memory-Manager] Module unloaded\n");
}

module_init(memory_manager_init);
module_exit(memory_manager_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Memory Manager Kernel Module for CSE330");
