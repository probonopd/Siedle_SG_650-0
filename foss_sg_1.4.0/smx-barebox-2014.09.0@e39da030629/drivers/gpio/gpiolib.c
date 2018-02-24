#define pr_fmt(fmt)	"gpiolib: " fmt

#include <init.h>
#include <common.h>
#include <command.h>
#include <complete.h>
#include <gpio.h>
#include <errno.h>
#include <malloc.h>

static LIST_HEAD(chip_list);

struct gpio_info {
	struct gpio_chip *chip;
	bool requested;
	char *label;
};

static struct gpio_info *gpio_desc;

static int gpio_desc_alloc(void)
{
	gpio_desc = xzalloc(sizeof(struct gpio_info) * ARCH_NR_GPIOS);

	return 0;
}
pure_initcall(gpio_desc_alloc);

static int gpio_ensure_requested(struct gpio_info *gi, int gpio)
{
	if (gi->requested)
		return 0;

	return gpio_request(gpio, "gpio");
}

static struct gpio_info *gpio_to_desc(unsigned gpio)
{
	if (gpio_is_valid(gpio))
		if (gpio_desc[gpio].chip)
			return &gpio_desc[gpio];

	pr_warning("invalid GPIO %d\n", gpio);

	return NULL;
}

int gpio_request(unsigned gpio, const char *label)
{
	struct gpio_info *gi = gpio_to_desc(gpio);
	int ret;

	if (!gi) {
		ret = -ENODEV;
		goto done;
	}

	if (gi->requested) {
		ret = -EBUSY;
		goto done;
	}

	ret = 0;

	if (gi->chip->ops->request) {
		ret = gi->chip->ops->request(gi->chip, gpio - gi->chip->base);
		if (ret)
			goto done;
	}

	gi->requested = true;
	gi->label = xstrdup(label);

done:
	if (ret)
		pr_err("_gpio_request: gpio-%d (%s) status %d\n",
			 gpio, label ? : "?", ret);

	return ret;
}

void gpio_free(unsigned gpio)
{
	struct gpio_info *gi = gpio_to_desc(gpio);

	if (!gi)
		return;

	if (!gi->requested)
		return;

	if (gi->chip->ops->free)
		gi->chip->ops->free(gi->chip, gpio - gi->chip->base);

	gi->requested = false;
	free(gi->label);
	gi->label = NULL;
}

/**
 * gpio_request_one - request a single GPIO with initial configuration
 * @gpio:	the GPIO number
 * @flags:	GPIO configuration as specified by GPIOF_*
 * @label:	a literal description string of this GPIO
 */
int gpio_request_one(unsigned gpio, unsigned long flags, const char *label)
{
	int err;

	err = gpio_request(gpio, label);
	if (err)
		return err;

	if (flags & GPIOF_DIR_IN)
		err = gpio_direction_input(gpio);
	else
		err = gpio_direction_output(gpio,
				(flags & GPIOF_INIT_HIGH) ? 1 : 0);

	if (err)
		goto free_gpio;

	return 0;

 free_gpio:
	gpio_free(gpio);
	return err;
}
EXPORT_SYMBOL_GPL(gpio_request_one);

/**
 * gpio_request_array - request multiple GPIOs in a single call
 * @array:	array of the 'struct gpio'
 * @num:	how many GPIOs in the array
 */
int gpio_request_array(const struct gpio *array, size_t num)
{
	int i, err;

	for (i = 0; i < num; i++, array++) {
		err = gpio_request_one(array->gpio, array->flags, array->label);
		if (err)
			goto err_free;
	}
	return 0;

err_free:
	while (i--)
		gpio_free((--array)->gpio);
	return err;
}
EXPORT_SYMBOL_GPL(gpio_request_array);

/**
 * gpio_free_array - release multiple GPIOs in a single call
 * @array:	array of the 'struct gpio'
 * @num:	how many GPIOs in the array
 */
void gpio_free_array(const struct gpio *array, size_t num)
{
	while (num--)
		gpio_free((array++)->gpio);
}
EXPORT_SYMBOL_GPL(gpio_free_array);

void gpio_set_value(unsigned gpio, int value)
{
	struct gpio_info *gi = gpio_to_desc(gpio);

	if (!gi)
		return;

	if (gpio_ensure_requested(gi, gpio))
		return;

	if (gi->chip->ops->set)
		gi->chip->ops->set(gi->chip, gpio - gi->chip->base, value);
}
EXPORT_SYMBOL(gpio_set_value);

int gpio_get_value(unsigned gpio)
{
	struct gpio_info *gi = gpio_to_desc(gpio);
	int ret;

	if (!gi)
		return -ENODEV;

	ret = gpio_ensure_requested(gi, gpio);
	if (ret)
		return ret;

	if (!gi->chip->ops->get)
		return -ENOSYS;
	return gi->chip->ops->get(gi->chip, gpio - gi->chip->base);
}
EXPORT_SYMBOL(gpio_get_value);

int gpio_direction_output(unsigned gpio, int value)
{
	struct gpio_info *gi = gpio_to_desc(gpio);
	int ret;

	if (!gi)
		return -ENODEV;

	ret = gpio_ensure_requested(gi, gpio);
	if (ret)
		return ret;

	if (!gi->chip->ops->direction_output)
		return -ENOSYS;
	return gi->chip->ops->direction_output(gi->chip, gpio - gi->chip->base,
					       value);
}
EXPORT_SYMBOL(gpio_direction_output);

int gpio_direction_input(unsigned gpio)
{
	struct gpio_info *gi = gpio_to_desc(gpio);
	int ret;

	if (!gi)
		return -ENODEV;

	ret = gpio_ensure_requested(gi, gpio);
	if (ret)
		return ret;

	if (!gi->chip->ops->direction_input)
		return -ENOSYS;
	return gi->chip->ops->direction_input(gi->chip, gpio - gi->chip->base);
}
EXPORT_SYMBOL(gpio_direction_input);

static int gpiochip_find_base(int start, int ngpio)
{
	int i;
	int spare = 0;
	int base = -ENOSPC;

	if (start < 0)
		start = 0;

	for (i = start; i < ARCH_NR_GPIOS; i++) {
		struct gpio_chip *chip = gpio_desc[i].chip;

		if (!chip) {
			spare++;
			if (spare == ngpio) {
				base = i + 1 - ngpio;
				break;
			}
		} else {
			spare = 0;
			i += chip->ngpio - 1;
		}
	}

	if (gpio_is_valid(base))
		debug("%s: found new base at %d\n", __func__, base);
	return base;
}

int gpiochip_add(struct gpio_chip *chip)
{
	int base, i;

	base = gpiochip_find_base(chip->base, chip->ngpio);
	if (base < 0)
		return base;

	if (chip->base >= 0 && chip->base != base)
		return -EBUSY;

	chip->base = base;

	list_add_tail(&chip->list, &chip_list);

	for (i = chip->base; i < chip->base + chip->ngpio; i++)
		gpio_desc[i].chip = chip;

	return 0;
}

void gpiochip_remove(struct gpio_chip *chip)
{
	list_del(&chip->list);
}

int gpio_get_num(struct device_d *dev, int gpio)
{
	struct gpio_chip *chip;

	list_for_each_entry(chip, &chip_list, list) {
		if (chip->dev == dev)
			return chip->base + gpio;
	}

	return -ENODEV;
}

#ifdef CONFIG_CMD_GPIO
static int do_gpiolib(int argc, char *argv[])
{
	int i;

	for (i = 0; i < ARCH_NR_GPIOS; i++) {
		struct gpio_info *gi = &gpio_desc[i];
		int val = -1, dir = -1;

		if (!gi->chip)
			continue;

		/* print chip information and header on first gpio */
		if (gi->chip->base == i) {
			printf("\nGPIOs %u-%u, chip %s:\n",
				gi->chip->base,
				gi->chip->base + gi->chip->ngpio - 1,
				gi->chip->dev->name);
			printf("%*cdir val requested  label\n", 13, ' ');
		}

		if (gi->chip->ops->get_direction)
			dir = gi->chip->ops->get_direction(gi->chip,
						i - gi->chip->base);
		if (gi->chip->ops->get)
			val = gi->chip->ops->get(gi->chip,
						i - gi->chip->base);

		printf("  GPIO %*d: %*s %*s %*s  %s\n", 4, i,
			3, (dir < 0) ? "unk" : ((dir == GPIOF_DIR_IN) ? "in" : "out"),
			3, (val < 0) ? "unk" : ((val == 0) ? "lo" : "hi"),
			9, gi->requested ? "true" : "false",
			(gi->requested && gi->label) ? gi->label : "");
	}

	return 0;
}

BAREBOX_CMD_START(gpioinfo)
	.cmd		= do_gpiolib,
	BAREBOX_CMD_DESC("list registered GPIOs")
	BAREBOX_CMD_GROUP(CMD_GRP_INFO)
	BAREBOX_CMD_COMPLETE(empty_complete)
BAREBOX_CMD_END
#endif
