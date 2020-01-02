#include<linux/sched.h>
#include<linux/mm_types.h>
#include<linux/nodemask.h>
struct pid_key{
	u32 pid;
};
struct running_info{
	char task_name[64];
	u64 last_run_time;
	u64 count;
};



struct time_key{
	u64 time;
};
struct mem_info{
	u32 pid;
	char task_name[64];
	unsigned int order;
};
BPF_HASH(cpu_use,struct pid_key,struct running_info);
BPF_HASH(memory_use,struct time_key,struct mem_info);
int cpuprobe(struct pt_regs *ctx, struct task_struct *prev)
{
	
	struct pid_key key;
	if(prev->tgid==TARGET_TGID)
	{
		key.pid=prev->pid;
		struct running_info* ri_ptr=cpu_use.lookup(&key);
		if(ri_ptr==NULL)
			return 0;
		u64 current_time=bpf_ktime_get_ns();
		ri_ptr->count+=current_time-ri_ptr->last_run_time;		
		return 0;
	}
	u64 tgid_pid=bpf_get_current_pid_tgid();
	u32 tgid=tgid_pid>>32;	
	if(tgid!=TARGET_TGID)
		return 0;	
	u32 pid=(u32)tgid_pid;
	key.pid=pid;
	struct running_info* ri_ptr=cpu_use.lookup(&key);
	if(ri_ptr==NULL)
	{
		struct running_info ri;
		bpf_get_current_comm(&(ri.task_name),64);
		ri.last_run_time=bpf_ktime_get_ns();
		ri.count=0;
		cpu_use.update(&key,&ri);
	}		
	else
	{
		ri_ptr->last_run_time=bpf_ktime_get_ns();
	}
	
	return 0;
}
int alloc_page_probe(struct pt_regs *ctx,gfp_t gfp_mask, unsigned int order, int preferred_nid,nodemask_t *nodemask)
{
	
	u64 tgid_pid=bpf_get_current_pid_tgid();
	u32 tgid=tgid_pid>>32;
	if(tgid!=TARGET_TGID)
		return 0;
	u32 pid=(u32)tgid_pid;
	struct time_key key;
	key.time=bpf_ktime_get_ns();
	struct mem_info mi;
	mi.pid=pid;
	bpf_get_current_comm(&(mi.task_name),64);
	mi.order=order;
	memory_use.update(&key,&mi);
	
	
	return 0;
}
int free_page_probe(struct pt_regs *ctx,unsigned long addr, unsigned int order)
{
	return 0;
}
