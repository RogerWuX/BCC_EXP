#!/usr/bin/python
from __future__ import print_function
from bcc import BPF
from time import sleep

target_tgid=input()
f=open('cpu.c','r')
bpf_text=f.read()
bpf_text=bpf_text.replace('TARGET_TGID',str(target_tgid))
bpf=BPF(text=bpf_text)
bpf.attach_kprobe(event='finish_task_switch',fn_name='cpuprobe')
bpf.attach_kretprobe(event='__alloc_pages_nodemask',fn_name='alloc_page_probe')
bpf.attach_kretprobe(event='free_pages',fn_name='free_page_probe')

match=bpf.num_open_kprobes()
if match!=3:
	exit(1);
time_count=0
try:
	while True:
		sleep(1.0);
		time_count+=1		
except KeyboardInterrupt:
	print('\r  \r',end='')
	#print('monitoring time:%d sec'%time_count,end='\n\n')
	total_execution_time_ns=0
	for key,value in bpf['cpu_use'].items():
		print('Thread id:%d'%key.pid)
		print('Thread name:%s'%value.task_name)
		print('excution_time:%lf sec'%(value.count*(1e-9)),end='\n\n')
		total_execution_time_ns+=value.count
	print("total_execution_time:%lf sec"%(total_execution_time_ns*(1e-9)))
	for key,value in bpf['memory_use'].items():
		print('Time:%d'%key.time)
		print('Thread id:%d'%value.pid)
		print('Thread name:%s'%value.task_name)
		print('memory use:%lf'%((2^value.order)*4096),end='\n\n')

