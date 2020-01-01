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
match=bpf.num_open_kprobes()
if match!=1:
	exit(1);
time_count=0
try:
	while True:
		sleep(1.0);
		time_count+=1		
except KeyboardInterrupt:
	print('\r  \r',end='')
	print('monitoring time:%d sec'%time_count,end='\n\n')
	total_execution_time_ns=0
	for key,value in bpf['hash'].items():
		print('Thread id:%d'%key.pid)
		print('Thread name:%s'%value.task_name)
		print('excution_time:%lf'%(value.count*(1e-9)),end='\n\n')
		total_execution_time_ns+=value.count
	print("total_execution_time:%lf sec"%(total_execution_time_ns*(1e-9)))
