#include<linux/sched.h>

#include<linux/mm_types.h>
#include<linux/nodemask.h>
struct pid_key{
	u32 pid;
};
struct info{
	char task_name[64];
	u64 last_run_time;
	u64 count;
};



struct time_key{
	u64 time;
};
struct info{
	u32 pid;
	char task_name[64];
	unsigned int order;
};
BPF_HASH(hash,struct pid_key,struct info);
BPF_HASH(hash,struct time_key,struct info);
int cpuprobe(struct pt_regs *ctx, struct task_struct *prev)
{
	
	struct key key;
	if(prev->tgid==TARGET_TGID)
	{
		key.pid=prev->pid;
		struct info* info_ptr=hash.lookup(&key);
		if(info_ptr==NULL)
			return 0;
		u64 current_time=bpf_ktime_get_ns();
		info_ptr->count+=current_time-info_ptr->last_run_time;		
		return 0;
	}
	u64 tgid_pid=bpf_get_current_pid_tgid();
	u32 tgid=tgid_pid>>32;	
	if(tgid!=TARGET_TGID)
		return 0;	
	u32 pid=(u32)tgid_pid;
	key.pid=pid;
	struct info* info_ptr=hash.lookup(&key);
	if(info_ptr==NULL)
	{
		struct info ti;
		bpf_get_current_comm(&(ti.task_name),64);
		ti.last_run_time=bpf_ktime_get_ns();
		ti.count=0;
		hash.update(&key,&ti);
	}		
	else
	{
		info_ptr->last_run_time=bpf_ktime_get_ns();
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
	struct key key;
	key.time=bpf_ktime_get_ns();
	struct info ti;
	ti.pid=pid;
	bpf_get_current_comm(&(ti.task_name),64);
	ti.order=order;
	hash.update(&key,&ti);
	
	
	return 0;
}
int free_page_probe(struct pt_regs *ctx,unsigned long addr, unsigned int order)
{
	return 0;
}
