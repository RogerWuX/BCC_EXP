#!/usr/bin/python
from __future__ import print_function
from bcc import BPF
from time import sleep

target_tgid=input()
f=open('memory.c','r')
bpf_text=f.read()
bpf_text=bpf_text.replace('TARGET_TGID',str(target_tgid))
bpf=BPF(text=bpf_text)
bpf.attach_kretprobe(event='__alloc_pages_nodemask',fn_name='alloc_page_probe')

bpf.attach_kretprobe(event='free_pages',fn_name='free_page_probe')

match=bpf.num_open_kprobes()
if match!=1:
	exit(1);
time_count=0
try:
	while True:
		sleep(1.0);
				
except KeyboardInterrupt:
	print('\r  \r',end='')
	for key,value in bpf['hash'].items():
		print('Time:%d'%key.time)
		print('Thread id:%d'%value.pid)
		print('Thread name:%s'%value.task_name)
		print('memory use:%lf'%((2^value.order)*4096),end='\n\n')




