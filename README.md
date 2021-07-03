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
   * [VMM Intercepts](https://performancebydesign.blogspot.com/2017/12/hyper-v-architecture-intercepts.html)
   * [Kernel Play Guide - AMD-V](https://nskernel.gitbook.io/kernel-play-guide/kvm/amd-v-and-sev)

# Table of Contents:
* [Preamble](#preamble)
   * [Purpose & Intentions](#purpose--intentions) 
   * [Support](#support)
   <!-- for #purpose talk about why you did this
		the lack of resource they had for this.-->
* [Terminology](#terminology)
<!-- Add a introduction later on, once you start writing a blog-->

* [Overview - KrakenSvm Hypervisor](#overview)
   * [SVM Semantics](#hv_semantics)
   * [Virtual Machine Control Block (VMCB) - Parts](#vmcb_parts)
	  * [Control Area](#control_area)
	  * [Save State](#save_state)
   * [SVM - Secure Virtual Machine Instruction Set](#intru_set)
	  * [VMLOAD](#vmload)
	  * [VMRUN](#vmrun)
	  * [VMSAVE](#vmload)
	  * [VMMCALL](#vmcall)
* [Credit - Special Thanks](#credit---special-thanks)

<!-- 
	put kernel driver concepts
	i.e. Dispatch and passive levels, IRQ levels,
	IOCTL
	
	explain what a guess mode is, explain what the 
	host is, explain how this HV is different from 
	mainstream HVs (VMWARE, VBOX), explain world switching,
	explain how injection works, explain the VM instructions
	better. Explain something that you may not have considered
-->

## Preamble
### Purpose & Intentions
There was a lack of complacent with the amount of menial projects
I've accomplished. I wanted to hit the big fish, something more precedence than my former projects. That was going to be a challenge 
and helpful for any others whom might decide to make a AMD HyperVisor.

### Support
Me having been new to HyperVisor programming, I'll struggle quite a lot making sure to all VMWare to support Nest Virtualization.
In order to test KrakenSvm in VMWARE make sure to follow these steps
## Terminology
   * **World Switch** - is the act of switching between Host and Guest. The host will excute VMRUN to start-up or run the Guest. The sequence would usually be Host -> Guest -> Host 
   * **vCPUs or CPUs** - when the intel or amd manual discusses about a VMM having a CPUs. It's referring to it having CPU Cores, rather than single entity of your CPU
   * **Nest Page Table** (NPT)-
   * **Intercepting** - 
   * **Virtual Memory Control Block (VMCB)** - For intel this would be called VMCS

## Credit - Special Thanks:
  Thanks to these OGs, for the spark of inspiration/support on my continuous effort on this project and for helping me understand certain concepts within HyperVisor development Journey. =)
  * [xeroxz](https://twitter.com/_xeroxz?lang=en) - helping explain concepts around HV
  * [Daax](https://twitter.com/daax_rynd) - his big brain coming in clutch like always. Having given me a great layout on the semantics of a AMD HyperVisor
  * [tandasat](https://github.com/tandasat/SimpleSvm/)
  * [Irql0](https://github.com/irql0) - explaining kernel driver concepts
<br>

   *  **Honorable fam mentions**: <br>
   [LLE](https://discord.gg/MvtdVcUsJs) members <br>
   [Jason](https://github.com/johnsonjason)
   <br>
