#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/fs.h>
#include <linux/mm_types.h>

static int pid = -1;
static unsigned long long addr = 0;

module_param(pid, int, 0644);
MODULE_PARM_DESC(pid, "Process ID of the target process");
module_param(addr, ullong, 0644);
MODULE_PARM_DESC(addr, "Virtual memory address in the process space");

static int __init memory_manager_init(void) {
    struct task_struct *task;
    struct mm_struct *mm;
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    unsigned long pfn;
    unsigned long phys_addr;
    swp_entry_t swap_entry;

    if (pid < 0 || addr == 0) {
        printk(KERN_ERR "[CSE330-Memory-Manager] Invalid parameters: PID and address must be non-negative.\n");
        return -EINVAL;
    }

    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task) {
        printk(KERN_ERR "[CSE330-Memory-Manager] PID [%d]: Process not found.\n", pid);
        return -ESRCH;
    }

    mm = task->mm;
    if (!mm) {
        printk(KERN_ERR "[CSE330-Memory-Manager] PID [%d]: No memory descriptor available.\n", pid);
        return -EFAULT;
    }

    pgd = pgd_offset(mm, addr);
    if (pgd_none(*pgd) || unlikely(pgd_bad(*pgd)))
        goto invalid;

    p4d = p4d_offset(pgd, addr);
    if (p4d_none(*p4d) || unlikely(p4d_bad(*p4d)))
        goto invalid;

    pud = pud_offset(p4d, addr);
    if (pud_none(*pud) || unlikely(pud_bad(*pud)))
        goto invalid;

    pmd = pmd_offset(pud, addr);
    if (pmd_none(*pmd) || unlikely(pmd_bad(*pmd)))
        goto invalid;

    pte = pte_offset_kernel(pmd, addr);
    if (!pte)
        goto invalid;

    if (pte_present(*pte)) {
        pfn = pte_pfn(*pte);
        phys_addr = (pfn << PAGE_SHIFT) | (addr & ~PAGE_MASK);
        printk(KERN_INFO "[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [%lx] swap identifier [NA]\n",
               pid, addr, phys_addr);
    } else if (pte_none(*pte)) {
        goto invalid;
    } else {
        swap_entry = pte_to_swp_entry(*pte);
        printk(KERN_INFO "[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [NA] swap identifier [%lx]\n",
               pid, addr, swap_entry.val);
    }
    return 0;

invalid:
    printk(KERN_INFO "[CSE330-Memory-Manager] PID [%d]: virtual address [%llx] physical address [NA] swap identifier [NA]\n",
           pid, addr);
    return 0;
}

static void __exit memory_manager_exit(void) {
    printk(KERN_INFO "[CSE330-Memory-Manager] Module unloaded.\n");
}

module_init(memory_manager_init);
module_exit(memory_manager_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("CSE330 Project 5: Memory Management Kernel Module");
