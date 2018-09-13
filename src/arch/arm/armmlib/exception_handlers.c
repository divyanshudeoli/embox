/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @author  Alexander Kalmuk
 * @date    18.03.2015
 */

#include <kernel/panic.h>
#include <kernel/printk.h>
#include <hal/context.h>
#include <hal/reg.h>

/* SCB - System Control Block */
#define SCB_BASE 0xe000ed00
#define SCB_CONF_FAULT_STATUS (SCB_BASE + 0x28)
#define SCB_HARD_FAULT_STATUS  (SCB_BASE + 0x2C)
#define SCB_MEM_FAULT_ADDRESS  (SCB_BASE + 0x34)
#define SCB_BUS_FAULT_ADDRESS  (SCB_BASE + 0x38)

static void print_fault_status(void) {
	uint32_t conf_faults = REG32_LOAD(SCB_CONF_FAULT_STATUS);
	uint32_t bus_fault_status, mem_fault_status, usage_fault_status;

	mem_fault_status = conf_faults & 0xf;
	bus_fault_status = (conf_faults >> 8) & 0xf;
	usage_fault_status = (conf_faults >> 16) & 0xff;

	printk("MemManage Fault Status register = %x\n", mem_fault_status);
	if (mem_fault_status) {
		printk("  MemManage Fault Address = %x\n",
				REG32_LOAD(SCB_MEM_FAULT_ADDRESS));
	}
	printk("Bus Fault Status register = %x\n", bus_fault_status);
	if (bus_fault_status) {
		printk("  Bus Fault Address = %x\n",
				REG32_LOAD(SCB_BUS_FAULT_ADDRESS));
	}
	printk("Usage Fault Status register = %x\n", usage_fault_status);
	printk("Hard Fault Status register = %x\n",
			REG32_LOAD(SCB_HARD_FAULT_STATUS));
}

/* Print exception info and return */
void exc_default_handler(struct exc_saved_base_ctx *ctx, int xpsr) {
	int exp_nr = xpsr & 0xFF;

	printk("\nEXCEPTION (0x%x):\n"
			"Exception saved context:\n"
			"  r0=%08x r1=%08x r2=%08x r3=%08x\n"
			"  r12=%08x lr=%08x pc=%08x xPSR=%08x\n",
			exp_nr,
			ctx->r[0], ctx->r[1], ctx->r[2], ctx->r[3],
			ctx->r[4], ctx->lr, ctx->pc, ctx->psr);

	print_fault_status();
	panic("\n%s\n", __func__);
}
