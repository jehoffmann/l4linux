#ifndef __ASM_L4__L4X_I386__EXCEPTION_H__
#define __ASM_L4__L4X_I386__EXCEPTION_H__

#include <asm/ptrace.h>

#include <l4/sys/utcb.h>

enum l4x_cpu_modes {
	L4X_MODE_KERNEL = 0,
	L4X_MODE_USER   = 3,
};

static inline void l4x_set_cpu_mode(struct pt_regs *r, enum l4x_cpu_modes mode)
{
	r->cs = mode;
}

static inline void l4x_set_user_mode(struct pt_regs *r)
{
	l4x_set_cpu_mode(r, L4X_MODE_USER);
}

static inline void l4x_set_kernel_mode(struct pt_regs *r)
{
	l4x_set_cpu_mode(r, L4X_MODE_KERNEL);
}

static inline unsigned long l4x_get_cpu_mode(struct pt_regs *r)
{
	return r->cs & 3;
}

static inline void l4x_make_up_kernel_regs(struct pt_regs *r)
{
	l4x_set_cpu_mode(r, L4X_MODE_KERNEL);
	r->ip = (unsigned long)__builtin_return_address(0);
	r->sp = current_stack_pointer;
	r->flags = native_save_fl();
}

#ifdef CONFIG_L4_VCPU

#define V2P(p, pr, v, vr) do { (p)->pr = (v)->vr; } while (0)
static inline void vcpu_to_ptregs(l4_vcpu_state_t *v,
                                  struct pt_regs *regs)
{
	V2P(regs, ax,    &v->r, ax);
	V2P(regs, bx,    &v->r, bx);
	V2P(regs, cx,    &v->r, cx);
	V2P(regs, dx,    &v->r, dx);
	V2P(regs, di,    &v->r, di);
	V2P(regs, si,    &v->r, si);
	V2P(regs, bp,    &v->r, bp);
	V2P(regs, ip,    &v->r, ip);
	V2P(regs, flags, &v->r, flags);
	V2P(regs, sp,    &v->r, sp);
	V2P(regs, fs,    &v->r, fs);
	if (v->saved_state & L4_VCPU_F_IRQ)
	        regs->flags |= X86_EFLAGS_IF;
	else
	        regs->flags &= ~X86_EFLAGS_IF;
	regs->cs = (regs->cs & ~3) | ((v->saved_state & L4_VCPU_F_USER_MODE) ? 3 : 0);
}
#undef V2P

#define P2V(p, pr, v, vr)   do { (v)->vr = (p)->pr; } while (0)
static inline void ptregs_to_vcpu(l4_vcpu_state_t *v,
                                  struct pt_regs *regs)
{
	P2V(regs, ax,    &v->r, ax);
	P2V(regs, bx,    &v->r, bx);
	P2V(regs, cx,    &v->r, cx);
	P2V(regs, dx,    &v->r, dx);
	P2V(regs, di,    &v->r, di);
	P2V(regs, si,    &v->r, si);
	P2V(regs, bp,    &v->r, bp);
	P2V(regs, ip,    &v->r, ip);
	P2V(regs, flags, &v->r, flags);
	P2V(regs, sp,    &v->r, sp);
	P2V(regs, fs,    &v->r, fs);
	v->saved_state &= ~(L4_VCPU_F_IRQ | L4_VCPU_F_USER_MODE);
	if (regs->flags & X86_EFLAGS_IF)
	        v->saved_state |= L4_VCPU_F_IRQ;
	if (regs->cs & 3)
	        v->saved_state |= L4_VCPU_F_USER_MODE;
}
#undef P2V

#endif

#define UE2P(p, pr, e, er)   do { p->pr = e->er; } while (0)
static inline void utcb_exc_to_ptregs(l4_exc_regs_t *exc, struct pt_regs *ptregs)
{
	UE2P(ptregs, ax,    exc, eax);
	UE2P(ptregs, bx,    exc, ebx);
	UE2P(ptregs, cx,    exc, ecx);
	UE2P(ptregs, dx,    exc, edx);
	UE2P(ptregs, di,    exc, edi);
	UE2P(ptregs, si,    exc, esi);
	UE2P(ptregs, bp,    exc, ebp);
	UE2P(ptregs, ip,    exc, ip);
	UE2P(ptregs, flags, exc, flags);
	UE2P(ptregs, sp,    exc, sp);
	ptregs->fs = exc->fs;
}
#undef UE2P

#define P2UE(e, er, p, pr) do { e->er = p->pr; } while (0)
static inline void ptregs_to_utcb_exc(struct pt_regs *ptregs, l4_exc_regs_t *exc)
{
	P2UE(exc, eax,    ptregs, ax);
	P2UE(exc, ebx,    ptregs, bx);
	P2UE(exc, ecx,    ptregs, cx);
	P2UE(exc, edx,    ptregs, dx);
	P2UE(exc, edi,    ptregs, di);
	P2UE(exc, esi,    ptregs, si);
	P2UE(exc, ebp,    ptregs, bp);
	P2UE(exc, ip,     ptregs, ip);
	P2UE(exc, flags,  ptregs, flags);
	P2UE(exc, sp,     ptregs, sp);
	exc->fs = ptregs->fs;
}
#undef P2UE

#endif /* ! __ASM_L4__L4X_I386__EXCEPTION_H__ */
