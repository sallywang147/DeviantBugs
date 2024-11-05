//kernel5.15: https://github.com/CachyOS/kernel-patches/blob/master/5.15/0001-NEST.patch
 b/arch/x86/kernel/smpboot.c
 void arch_scale_freq_tick(void)
 	this_cpu_write(arch_prev_aperf, aperf);
 	this_cpu_write(arch_prev_mperf, mperf);
 
	trace_printk("freq %lld\n", div64_u64((cpu_khz * acnt), mcnt));

 	if (check_shl_overflow(acnt, 2*SCHED_CAPACITY_SHIFT, &acnt))
 		goto error;
 
 b/include/linux/sched.h
struct kmap_ctrl {
 #endif
 };
 
struct expand_mask {
	spinlock_t			lock;
	cpumask_t			expand_mask, reserve_mask;
	int				start;
	int				count;
};

extern void init_expand_mask(void);
extern void clear_expand_mask(int cpu);
extern void reset_expand_mask(int cpu);

 struct task_struct {
 #ifdef CONFIG_THREAD_INFO_IN_TASK

 	int				recent_used_cpu;
 	int				wake_cpu;
	int				use_expand_mask;
	int				patience;
	int				attached;
 #endif
 	int				on_rq;
 b/include/linux/sched/topology.h
@@ -75,6 +75,8 @@ struct sched_domain_shared {
 	atomic_t	nr_busy_cpus;
 	int		has_idle_cores;
 	int		nr_idle_scan;
	int		has_idle_threads;
	int		left_off;
 };
 
 struct sched_domain {

b/kernel/sched/core.c
 __do_set_cpus_allowed(struct task_struct *p, const struct cpumask *new_mask, u32
 
 void do_set_cpus_allowed(struct task_struct *p, const struct cpumask *new_mask)
 {
	p->use_expand_mask = 0;
 	__do_set_cpus_allowed(p, new_mask, 0);
 }
 
int select_task_rq(struct task_struct *p, int cpu, int wake_flags)
 	 * [ this allows ->select_task() to simply return task_cpu(p) and
 	 *   not worry about this generic constraint ]
 	 */
	if (unlikely(!is_cpu_allowed(p, cpu))) {
		if (p->use_expand_mask)
			atomic_set(&cpu_rq(p->thread_info.cpu)->taken,0);
 		cpu = select_fallback_rq(task_cpu(p), p);
	}
 
 	return cpu;
 }
 b/arch/x86/kernel/smpboot.c
 void arch_scale_freq_tick(void)
 	this_cpu_write(arch_prev_aperf, aperf);
 	this_cpu_write(arch_prev_mperf, mperf);
 
	trace_printk("freq %lld\n", div64_u64((cpu_khz * acnt), mcnt));

 	if (check_shl_overflow(acnt, 2*SCHED_CAPACITY_SHIFT, &acnt))
 		goto error;
 
 b/include/linux/sched.h
 struct kmap_ctrl {
 #endif
 };
 
struct expand_mask {
	spinlock_t			lock;
	cpumask_t			expand_mask, reserve_mask;
	int				start;
	int				count;
};

extern void init_expand_mask(void);
extern void clear_expand_mask(int cpu);
extern void reset_expand_mask(int cpu);

 struct task_struct {
 #ifdef CONFIG_THREAD_INFO_IN_TASK

 	int				recent_used_cpu;
 	int				wake_cpu;
	int				use_expand_mask;
	int				patience;
	int				attached;
 #endif
 	int				on_rq;
 

 b/include/linux/sched/topology.h
 struct sched_domain_shared {
 	atomic_t	nr_busy_cpus;
 	int		has_idle_cores;
 	int		nr_idle_scan;
	int		has_idle_threads;
	int		left_off;
 };
 
 struct sched_domain {
b/kernel/sched/core.c
 __do_set_cpus_allowed(struct task_struct *p, const struct cpumask *new_mask, u32
 
 void do_set_cpus_allowed(struct task_struct *p, const struct cpumask *new_mask)
 {
	p->use_expand_mask = 0;
 	__do_set_cpus_allowed(p, new_mask, 0);
 }
 
 int select_task_rq(struct task_struct *p, int cpu, int wake_flags)
 	 * [ this allows ->select_task() to simply return task_cpu(p) and
 	 *   not worry about this generic constraint ]
 	 */
	if (unlikely(!is_cpu_allowed(p, cpu))) {
		if (p->use_expand_mask)
			atomic_set(&cpu_rq(p->thread_info.cpu)->taken,0);
 		cpu = select_fallback_rq(task_cpu(p), p);
	}
 
 	return cpu;
 }