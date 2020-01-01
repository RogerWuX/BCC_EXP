#!/usr/bin/python
from __future__ import print_function
from bcc import BPF
from time import sleep

target_tgid=input()
f=open('memory.c','r')
bpf_text=f.read()
bpf_text=bpf_text.replace('TARGET_TGID',str(target_tgid))
bpf=BPF(text=bpf_text)

match=bpf.num_open_kprobes()
if match!=1:
	exit(1);
time_count=0
try:
	while True:
		sleep(1.0);
				
except KeyboardInterrupt:
	print('\r  \r',end='')
	



