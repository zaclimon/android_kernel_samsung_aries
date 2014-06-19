#include <linux/types.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

static char buyer_code[80];

const char *
samsung_efs_buyer_code(void)
{
	return buyer_code;
}
EXPORT_SYMBOL(samsung_efs_buyer_code);

static ssize_t
buyer_code_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	return sprintf(buf, "%s", buyer_code);
}

static ssize_t
buyer_code_store(struct kobject *kobj, struct kobj_attribute *attr,
		 const char *buf, size_t count)
{
	char *p;

	if (count >= sizeof(buyer_code)-1)
		return -EINVAL;

	memcpy(buyer_code, buf, count);
	buyer_code[count+1] = '\0';

	/* Trim any trailing whitespace */
	for (p = buyer_code; *p; ++p)
		if (*p <= ' ')
			*p = '\0';

	return count;
}

static char bt_addr[80];

const char *
samsung_efs_bt_addr(void)
{
	return bt_addr;
}
EXPORT_SYMBOL(samsung_efs_bt_addr);

static ssize_t
bt_addr_show(struct kobject *kobj, struct kobj_attribute *attr,
	     char *buf)
{
	return sprintf(buf, "%s", bt_addr);
}

static ssize_t
bt_addr_store(struct kobject *kobj, struct kobj_attribute *attr,
	      const char *buf, size_t count)
{
	char *p;
	size_t n;

	if (count >= sizeof(bt_addr)-1)
		return -EINVAL;

	memcpy(bt_addr, buf, count);
	bt_addr[count+1] = '\0';

	/* Convert "bt_addr:xxxxxxxxxxxx" to "xx:xx:xx:xx:xx:xx" */
	if (count >= 11+6*2 && !memcmp(bt_addr, "bt_macaddr:", 11)) {
		for (n = 0; n < 6; ++n) {
			bt_addr[n*3] = bt_addr[11+n*2];
			bt_addr[n*3+1] = bt_addr[11+n*2+1];
			bt_addr[n*3+2] = ':';
		}
		bt_addr[6*3-1] = '\0';
	}

	/* Trim any trailing whitespace */
	for (p = bt_addr; *p; ++p)
		if (*p <= ' ')
			*p = '\0';

	return count;
}

static struct kobj_attribute efs_buyer_code_attr =
	__ATTR(buyer_code, 0644, buyer_code_show, buyer_code_store);

static struct kobj_attribute efs_bt_addr_attr =
	__ATTR(bt_addr, 0644, bt_addr_show, bt_addr_store);

struct kobject *efs_kobj;

static int __init
efs_init(void)
{
	int ret;

	efs_kobj = kobject_create_and_add("efs", firmware_kobj);
	if (!efs_kobj) {
		printk(KERN_WARNING "kobject_create_and_add efs failed\n");
		return -EINVAL;
	}

	ret = sysfs_create_file(efs_kobj, &efs_buyer_code_attr.attr);
	if (ret)
		goto out;

	ret = sysfs_create_file(efs_kobj, &efs_bt_addr_attr.attr);
	if (ret)
		goto out;

	ret = 0;

out:
	return ret;
}
device_initcall(efs_init);
