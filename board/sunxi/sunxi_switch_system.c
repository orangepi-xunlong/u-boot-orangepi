// SPDX-License-Identifier: GPL-2.0+

#include <common.h>
#include <environment.h>

static int __check_systemAB(void)
{
	char *systemA = env_get("systemA");
	char *systemB = env_get("systemB");
	char *rootfsA = env_get("rootfsA");
	char *rootfsB = env_get("rootfsB");

	if (!systemA || !rootfsA) {
		printf("CHECK SYSTEM ERROR : env can't find systemA or rootfsA!!\n");
		return -1;
	}
	if (!systemB || !rootfsB) {
		printf("CHECK SYSTEM ERROR : env can't find systemB or rootfsB!!\n");
		return -1;
	}

	return 0;
}

//note:systemAB_now can only be modified by this function
static int __switch_systemAB(char *system)
{
	char tmp_system[5] = {0};
	char *systemA = NULL;
	char *systemB = NULL;
	char *rootfsA = NULL;
	char *rootfsB = NULL;
	char *systemAB_damage = env_get("systemAB_damage");
	char *systemAB_now = env_get("systemAB_now");
	memcpy(tmp_system, system, sizeof(tmp_system));

	if (!strcmp(tmp_system, "A")) {
		if (strcmp(systemAB_damage, "A"))
			printf("SWITCH WARNING : systemA is damaged\n");

		if ((!strcmp(systemAB_now, "B")) || (!systemAB_now)) {
			systemA = env_get("systemA");
			rootfsA = env_get("rootfsA");
			env_set("boot_partition", systemA);
			env_set("root_partition", rootfsA);
			env_set("systemAB_now", "A");
			env_save();
			printf("boot system %s\n", tmp_system);
			return 0;
		}
	} else if (!strcmp(tmp_system, "B")) {
		if (strcmp(systemAB_damage, "B"))
			printf("SWITCH WARNING : systemB is damaged\n");

		if ((!strcmp(systemAB_now, "A")) || (!systemAB_now)) {
			systemB = env_get("systemB");
			rootfsB = env_get("rootfsB");
			env_set("boot_partition", systemB);
			env_set("root_partition", rootfsB);
			env_set("systemAB_now", "B");
			env_save();
			printf("boot system %s\n", tmp_system);
			return 0;
		}
	} else {
		printf("SWITCH ERROR : input system is %s\n", tmp_system);
		printf("               Please input A or B system\n");
		return -1;
	}
	return 0;
}

//The system will switch according to systemab_next
int sunxi_auto_switch_system(void)
{
	char *systemAB_next = env_get("systemAB_next");

	if (!systemAB_next) {
		printf("AUTO SWITCH WARNING : Can't get systemAB_next\n");
		printf("                      Started system by default\n");
		return 0;
	}
	if (strcmp(systemAB_next, "A") && strcmp(systemAB_next, "B")) {
		printf("AUTO SWITCH WARNING : systemAB_next(%s) is neither A nor B\n", systemAB_next);
		printf("                      Started system by default\n");
		return 0;
	}

	if (__check_systemAB()) {
		printf("AUTO SWITCH ERROR : Check systemAB fail\n");
		return -1;
	}

	if (!strcmp(systemAB_next, "A")) {
		if (__switch_systemAB("A")) {
				printf("AUTO SWITCH ERROR : Switch to systemA fail\n");
				return -1;
			}
		}
	else {
		if (__switch_systemAB("B")) {
				printf("AUTO SWITCH ERROR : Switch to systemB fail\n");
				return -1;
			}
		}

	return 0;
}

//note:systemAB_damage can only be modified by this function
int sunxi_damage_switch_system(void)
{
	printf("==========DAMAGE SWITCH NOW==========\n");
	char *systemAB_next = env_get("systemAB_next");

	if (!systemAB_next) {
		printf("DAMAGE SWITCH ERROR : Can't get systemAB_next\n");
		printf("                      Please check the systemAB_next\n");
		return -1;
	}

	if (__check_systemAB()) {
		printf("DAMAGE SWITCH ERROR : Check systemAB fail\n");
		return -1;
	}

	if (!strcmp(systemAB_next, "A")) {
		printf("DAMAGE SWITCH : systemA is damaged;now switch to systemB\n");
		if (!__switch_systemAB("B")) {
			env_set("systemAB_next", "B");
			env_set("systemAB_damage", "A");
			bootcount_store(0);
#ifndef CONFIG_BOOTCOUNT_ENV
			/*
			 * If CONFIG_BOOTCOUNT_ENV is defined,
			 * there is no need to call env_save function to save the data to env,
			 * because the bootcount_store function contains the env_save function
			 *
			 * If CONFIG_BOOTCOUNT_ENV is not defined,
			 * need to call env_save function to save the data to env,
			 * because the bootcount_store function does not include
			 * the env_save function at this time.
			 */
			env_save();
#endif
		} else {
			printf("DAMAGE SWITCH ERROR:Switch to systemB fail\n");
			return -1;
		}
	} else {
		printf("DAMAGE SWITCH : systemB is damaged ;now switch to systemA\n");
		if (!__switch_systemAB("A")) {
			env_set("systemAB_next", "A");
			env_set("systemAB_damage", "B");
			bootcount_store(0);
#ifndef CONFIG_BOOTCOUNT_ENV
			/* The situation is the same as the comment above */
			env_save();
#endif
		} else {
			printf("DAMAGE SWITCH ERROR : Switch to systemA fail\n");
			return -1;
		}
	}

	return 0;
}
