//kernel5.15: https://github.com/CachyOS/kernel-patches/blob/master/5.15/0001-NEST.patch

a/arch/x86/kernel/smpboot.c
 void arch_scale_freq_tick(void)
 	this_cpu_write(arch_prev_aperf, aperf);
 	this_cpu_write(arch_prev_mperf, mperf);
 

 	if (check_shl_overflow(acnt, 2*SCHED_CAPACITY_SHIFT, &acnt))
 		goto error;
 a/include/linux/sched.h
struct kmap_ctrl {
 #endif
 };

 struct task_struct {
 #ifdef CONFIG_THREAD_INFO_IN_TASK

 	int				recent_used_cpu;
 	int				wake_cpu;
 #endif
 	int				on_rq;
 
a/include/linux/sched/topology.h
 struct sched_domain_shared {
 	atomic_t	nr_busy_cpus;
 	int		has_idle_cores;
 	int		nr_idle_scan;

 };
 
 struct sched_domain {
a/kernel/sched/core.c
 __do_set_cpus_allowed(struct task_struct *p, const struct cpumask *new_mask, u32
 
 void do_set_cpus_allowed(struct task_struct *p, const struct cpumask *new_mask)
 {

 	__do_set_cpus_allowed(p, new_mask, 0);
 }
 
 int select_task_rq(struct task_struct *p, int cpu, int wake_flags)
 	 * [ this allows ->select_task() to simply return task_cpu(p) and
 	 *   not worry about this generic constraint ]
 	 */
	if (unlikely(!is_cpu_allowed(p, cpu)))
 
 	return cpu;
 }