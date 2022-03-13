KrakenSvm

# Motativation
This was apart of my "Understanding of OS theoretical concepts" and emulator series. I enjoy my time with this project, if you're interested in AMD virtualization then read chapter 15 of the amd manual.
## Warning
  This isn't finish yet, it's still a W.I.P so they may be errors. Feel free to report when possible and hooks aren't finish yet.
## Credit - Special Thanks:
  Thanks to these OGs, for the spark of inspiration/support and just being good friends/acquaintances overall on my continuous effort on this project and for helping me understand certain concepts within HyperVisor development Journey. =)
  * [xeroxz](https://twitter.com/_xeroxz?lang=en) - Helping explain concepts around HV and allowing me to use his code semantics for his amazing Hypervisor project
  * [Daax](https://twitter.com/daax_rynd) - His big brain coming in clutch like always. He's given me a great layout on the semantics of a AMD HyperVisor and his approach on it, sharing resources left and right. Even answered my most trivial questions. Daax never misses
  * **horsie** - sharing some resource
  * [tandasat](https://github.com/tandasat/SimpleSvm/) - Amazing resource
  * [Irql0](https://github.com/irql0) - explaining certain Windows kernel driver, OS concepts and getting me out of tough assembly problems
  * [iPower](https://github.com/iPower) - sharing an approach to hooking System Calls and helping fix my VMCB issues
  * [Matthias](https://github.com/not-matthias) - For providing information that lead me on to fixing a VMEXIT_INVALID bug i came across and for going out of his way to even debug my HyperVisor to point logic issues and errors. Thank you so much 
  * [Jason](https://github.com/johnsonjason)
  * [Luminous](https://github.com/389850689) 
   
 ## Reference - Resource I relied on:
   * [AMD CPUID Specification](http://developer.amd.com/wordpress/media/2012/10/254811.pdf)
   * [AMD Manual](https://www.amd.com/system/files/TechDocs/24593.pdf) 
   * [AMD Pacifica Docs](https://course9+s.cs.vt.edu/~cs5204/fall07-kafura/Papers/Virtualization/AMD-Pacifica-slides.pdf)
   * [Intel Manual](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
   * [Xeroxz Hypervisor](https://githacks.org/_xeroxz/bluepill)
   * [Irql Hypervisor](https://github.com/irql0/limevisor)
   * [VMM Intercepts](https://performancebydesign.blogspot.com/2017/12/hyper-v-architecture-intercepts.html)
   * [Kernel Play Guide - AMD-V](https://nskernel.gitbook.io/kernel-play-guide/kvm/amd-v-and-sev)
   * [System calls on Windows x64](https://www.n4r1b.com/posts/2019/03/system-calls-on-windows-x64/)
   * [[windows] kernel internals](https://www.matteomalvica.com/minutes/windows_kernel/)
 <!-- 
 for any unknown understandings
  - windows stack ABI = https://www.gamasutra.com/view/news/178446/Indepth_Windows_x64_ABI_Stack_frames.php
  -->
