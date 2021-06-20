KrakenSvm

# Motativation
This was apart of my "Understanding of OS theoretical concepts" and emulator series. I had a great amount of fun with project, if you're interested in AMD virtualization then read chapter 15 of the amd manual 

A simple x86_64 AMD-v hypervisor type-2, with syscall and paging hooks. Programmed with C++
# Resource I relied on:
   * [AMD CPUID Specification](http://developer.amd.com/wordpress/media/2012/10/254811.pdf)
   * [AMD Manual](https://www.amd.com/system/files/TechDocs/24593.pdf) 
   * [AMD Pacifica Docs](https://courses.cs.vt.edu/~cs5204/fall07-kafura/Papers/Virtualization/AMD-Pacifica-slides.pdf)
   * [Intel Manual](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
   * [Xeroxz Hypervisor](https://githacks.org/_xeroxz/bluepill)
   * [Irql Hypervisor](https://github.com/irql0/limevisor)
   * [VMM Intercepts] (https://performancebydesign.blogspot.com/2017/12/hyper-v-architecture-intercepts.html)

# Table of content of things I've learnt:
* Support

* AMD-V
   * [VMLOAD Semantics](#vmload)
   * [VMRUN  Semantics](#vmrun)
   * [VMSAVE Semantics](#vmload)
   * [HyperVisor (VMM) key terms](#hyper_keyterms)
* OS Concepts

## AMD-V
*   <a name="hyper_keyterms"> ** HyperVisor (VMM) key terms ** </a> <br>
   * World Switch - is the act of switching between Host and Guest. The host will excute VMRUN to start-up or run the Guest. The sequence would usually be Host -> Guest -> Host 
   * vCPUs or CPUs - when the intel or amd manual discusses about a VMM having a CPUs. It's referring to it having CPU Cores, rather than single entity of your CPU
   * Nest Page Table (NPT)-
   * Intercepting - 
   * Virtual Memory Control Block (VMCB) - For intel this would be called VMCS


