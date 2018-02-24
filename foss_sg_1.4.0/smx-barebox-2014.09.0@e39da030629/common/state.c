/*
 * Copyright (C) 2012 Jan Luebbe <j.luebbe@pengutronix.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <environment.h>
#include <common.h>
#include <malloc.h>
#include <xfuncs.h>
#include <errno.h>
#include <fcntl.h>
#include <ioctl.h>
#include <libbb.h>
#include <state.h>
#include <init.h>
#include <net.h>
#include <libfile.h>
#include <fs.h>

#include <linux/mtd/mtd-abi.h>
#include <linux/list.h>
#include <linux/err.h>

#include <asm/unaligned.h>

/* list of all registered state instances */
static LIST_HEAD(state_list);

/* instance of a single variable */
struct state_variable {
	enum state_variable_type type;
	struct list_head list;
	char *name;
	void *raw;
	int raw_size;
	int index;
};

/* A variable type (uint32, enum32) */
struct variable_type {
	enum state_variable_type type;
	const char *type_name;
	struct list_head list;
	int (*export)(struct state_variable *, struct device_node *);
	int (*init)(struct state_variable *, struct device_node *);
	struct state_variable *(*create)(struct state *state,
			const char *name, struct device_node *);
};

static int state_set_dirty(struct param_d *p, void *priv)
{
	struct state *state = priv;

	state->dirty = 1;

	return 0;
}

/*
 *  uint32
 */
struct state_uint32 {
	struct state_variable var;
	struct param_d *param;
	uint32_t value;
	uint32_t value_default;
};

static int state_var_compare(struct list_head *a, struct list_head *b)
{
	struct state_variable *va = list_entry(a, struct state_variable, list);
	struct state_variable *vb = list_entry(b, struct state_variable, list);

	if (va->index == vb->index)
		return 0;

	return va->index < vb->index ? -1 : 1;
}

static int state_add_variable(struct state *state, struct state_variable *var)
{
	list_add_sort(&var->list, &state->variables, state_var_compare);

	return 0;
}

static inline struct state_uint32 *to_state_uint32(struct state_variable *s)
{
	return container_of(s, struct state_uint32, var);
}

static int state_uint32_export(struct state_variable *var,
		struct device_node *node)
{
	struct state_uint32 *su32 = to_state_uint32(var);
	int ret;

	ret = of_property_write_u32(node, "default", su32->value_default);
	if (ret)
		return ret;

	return of_property_write_u32(node, "value", su32->value);
}

static int state_uint32_init(struct state_variable *sv,
		struct device_node *node)
{
	int len;
	const __be32 *value, *value_default;
	struct state_uint32 *su32 = to_state_uint32(sv);

	value_default = of_get_property(node, "default", &len);
	if (value_default && len != sizeof(uint32_t))
		return -EINVAL;

	value = of_get_property(node, "value", &len);
	if (value && len != sizeof(uint32_t))
		return -EINVAL;

	if (value_default)
		su32->value_default = be32_to_cpu(*value_default);
	if (value)
		su32->value = be32_to_cpu(*value);
	else
		su32->value = su32->value_default;

	return 0;
}

static struct state_variable *state_uint32_create(struct state *state,
		const char *name, struct device_node *node)
{
	struct state_uint32 *su32;

	su32 = xzalloc(sizeof(*su32));

	su32->param = dev_add_param_int(&state->dev, name, state_set_dirty,
			NULL, &su32->value, "%d", state);
	if (!su32->param) {
		free(su32);
		return ERR_PTR(-EINVAL);
	}

	su32->var.raw_size = sizeof(uint32_t);
	su32->var.raw = &su32->value;

	return &su32->var;
}

/*
 *  enum32
 */
struct state_enum32 {
	struct state_variable var;
	struct param_d *param;
	uint32_t value;
	uint32_t value_default;
	const char **names;
	int num_names;
};

static inline struct state_enum32 *to_state_enum32(struct state_variable *s)
{
	return container_of(s, struct state_enum32, var);
}

static int state_enum32_export(struct state_variable *var,
		struct device_node *node)
{
	struct state_enum32 *enum32 = to_state_enum32(var);
	int ret, i, len;
	char *prop, *str;

	ret = of_property_write_u32(node, "default", enum32->value_default);
	if (ret)
		return ret;

	ret = of_property_write_u32(node, "value", enum32->value);
	if (ret)
		return ret;

	len = 0;

	for (i = 0; i < enum32->num_names; i++)
		len += strlen(enum32->names[i]) + 1;

	prop = xzalloc(len);
	str = prop;

	for (i = 0; i < enum32->num_names; i++)
		str += sprintf(str, "%s", enum32->names[i]) + 1;

	ret = of_set_property(node, "names", prop, len, 1);

	free(prop);

	return ret;
}

static int state_enum32_init(struct state_variable *sv, struct device_node *node)
{
	struct state_enum32 *enum32 = to_state_enum32(sv);
	int len;
	const __be32 *value, *value_default;

	value = of_get_property(node, "value", &len);
	if (value && len != sizeof(uint32_t))
		return -EINVAL;

	value_default = of_get_property(node, "default", &len);
	if (value_default && len != sizeof(uint32_t))
		return -EINVAL;

	if (value_default)
		enum32->value_default = be32_to_cpu(*value_default);
	if (value)
		enum32->value = be32_to_cpu(*value);
	else
		enum32->value = enum32->value_default;

	return 0;
}

static struct state_variable *state_enum32_create(struct state *state,
		const char *name, struct device_node *node)
{
	struct state_enum32 *enum32;
	int ret, i, num_names;

	enum32 = xzalloc(sizeof(*enum32));

	num_names = of_property_count_strings(node, "names");

	enum32->names = xzalloc(sizeof(char *) * num_names);
	enum32->num_names = num_names;
	enum32->var.raw_size = sizeof(uint32_t);
	enum32->var.raw = &enum32->value;

	for (i = 0; i < num_names; i++) {
		const char *name;

		ret = of_property_read_string_index(node, "names", i, &name);
		if (ret)
			goto out;
		enum32->names[i] = xstrdup(name);
	}

	enum32->param = dev_add_param_enum(&state->dev, name, state_set_dirty, NULL,
			&enum32->value, enum32->names, num_names, state);
	if (!enum32->param) {
		ret = -EINVAL;
		goto out;
	}

	return &enum32->var;
out:
	free(enum32->names);
	free(enum32);
	return ERR_PTR(ret);
}

/*
 *  MAC address
 */
struct state_mac {
	struct state_variable var;
	struct param_d *param;
	uint8_t value[6];
	uint8_t value_default[6];
};

static inline struct state_mac *to_state_mac(struct state_variable *s)
{
	return container_of(s, struct state_mac, var);
}

static int state_mac_export(struct state_variable *var,
		struct device_node *node)
{
	struct state_mac *mac = to_state_mac(var);
	int ret;

	ret = of_property_write_u8_array(node, "default", mac->value_default, 6);
	if (ret)
		return ret;

	return of_property_write_u8_array(node, "value", mac->value, 6);
}

static int state_mac_init(struct state_variable *sv, struct device_node *node)
{
	struct state_mac *mac = to_state_mac(sv);
	u8 value[6] = {};
	u8 value_default[6] = {};

	of_property_read_u8_array(node, "default", value_default, 6);
	memcpy(mac->value_default, value_default, 6);

	if (!of_property_read_u8_array(node, "value", value, 6))
		memcpy(mac->value, value, 6);
	else
		memcpy(mac->value, value_default, 6);

	return 0;
}

static struct state_variable *state_mac_create(struct state *state,
		const char *name, struct device_node *node)
{
	struct state_mac *mac;
	int ret;

	mac = xzalloc(sizeof(*mac));

	mac->var.raw_size = 6;
	mac->var.raw = mac->value;

	mac->param = dev_add_param_mac(&state->dev, name, state_set_dirty,
			NULL, mac->value, state);
	if (!mac->param) {
		ret = -EINVAL;
		goto out;
	}

	return &mac->var;
out:
	free(mac);
	return ERR_PTR(ret);
}

static struct variable_type types[] =  {
	{
		.type = STATE_TYPE_U32,
		.type_name = "uint32",
		.export = state_uint32_export,
		.init = state_uint32_init,
		.create = state_uint32_create,
	}, {
		.type = STATE_TYPE_ENUM,
		.type_name = "enum32",
		.export = state_enum32_export,
		.init = state_enum32_init,
		.create = state_enum32_create,
	}, {
		.type = STATE_TYPE_MAC,
		.type_name = "mac",
		.export = state_mac_export,
		.init = state_mac_init,
		.create = state_mac_create,
	},
};

static struct variable_type *state_find_type(enum state_variable_type type)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(types); i++) {
		if (type == types[i].type) {
			return &types[i];
		}
	}

	return NULL;
}

static struct variable_type *state_find_type_by_name(const char *name)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(types); i++) {
		if (!strcmp(name, types[i].type_name)) {
			return &types[i];
		}
	}

	return NULL;
}

/*
 * Generic state functions
 */

static struct state *state_new(const char *name)
{
	struct state *state;
	int ret;

	state = xzalloc(sizeof(*state));
	safe_strncpy(state->dev.name, name, MAX_DRIVER_NAME);
	state->name = state->dev.name;
	state->dev.id = DEVICE_ID_SINGLE;
	INIT_LIST_HEAD(&state->variables);

	ret = register_device(&state->dev);
	if (ret)
		return ERR_PTR(ret);

	state->dirty = 1;
	dev_add_param_bool(&state->dev, "dirty", NULL, NULL, &state->dirty, NULL);

	list_add_tail(&state->list, &state_list);

	return state;
}

static void state_release(struct state *state)
{
	unregister_device(&state->dev);
	free(state);
}

static struct device_node *state_to_node(struct state *state)
{
	struct device_node *root, *node;
	struct state_variable *sv;
	int ret;

	root = of_new_node(NULL, NULL);

	list_for_each_entry(sv, &state->variables, list) {
		struct variable_type *vtype;
		char *name;

		name = asprintf("%s@%d", sv->name, sv->index);
		node = of_new_node(root, name);
		free(name);

		vtype = state_find_type(sv->type);
		if (!vtype) {
			ret = -ENOENT;
			goto out;
		}

		of_set_property(node, "type", vtype->type_name,
				strlen(vtype->type_name) + 1, 1);

		ret = vtype->export(sv, node);
		if (ret)
			goto out;
	}

	of_property_write_u32(root, "magic", state->magic);

	return root;
out:
	of_delete_node(root);
	return ERR_PTR(ret);
}

static struct state_variable *state_find_var(struct state *state, const char *name)
{
	struct state_variable *sv;

	list_for_each_entry(sv, &state->variables, list) {
		if (!strcmp(sv->name, name))
			return sv;
	}

	return NULL;
}

static int state_variable_from_node(struct state *state, struct device_node *node,
		bool create)
{
	struct variable_type *vtype;
	struct state_variable *sv;
	char *name, *indexs;
	int index = 0;
	const char *type_name = NULL;

	of_property_read_string(node, "type", &type_name);
	if (!type_name)
		return -EINVAL;

	vtype = state_find_type_by_name(type_name);
	if (!vtype)
		return -ENOENT;

	name = xstrdup(node->name);
	indexs = strchr(name, '@');
	if (indexs) {
		*indexs++ = 0;
		index = simple_strtoul(indexs, NULL, 10);
	}

	if (create) {
		sv = vtype->create(state, name, node);
		if (IS_ERR(sv)) {
			int ret = PTR_ERR(sv);
			pr_err("failed to create %s: %s\n", name, strerror(-ret));
			return ret;
		}
		sv->name = name;
		sv->type = vtype->type;
		sv->index = index;
		state_add_variable(state, sv);
	} else {
		sv = state_find_var(state, name);
		if (IS_ERR(sv)) {
			int ret = PTR_ERR(sv);
			pr_err("no such variable: %s: %s\n", name, strerror(-ret));
			return ret;
		}
	}

	vtype->init(sv, node);

	return 0;
}

int state_from_node(struct state *state, struct device_node *node, bool create)
{
	struct device_node *child;
	int ret;
	uint32_t magic;

	of_property_read_u32(node, "magic", &magic);

	if (create) {
		state->magic = magic;
	} else {
		if (state->magic && state->magic != magic) {
			dev_err(&state->dev,
					"invalid magic 0x%08x, should be 0x%08x\n",
					magic, state->magic);
			return -EINVAL;
		}
	}

	for_each_child_of_node(node, child) {
		ret = state_variable_from_node(state, child, create);
		if (ret)
			return ret;
	}

	return 0;
}

/*
 * state_new_from_node - create a new state instance from a device_node
 *
 * @name	The name of the new state instance
 * @node	The device_node describing the new state instance
 */
struct state *state_new_from_node(const char *name, struct device_node *node)
{
	struct state *state;
	int ret;

	state = state_new(name);
	if (!state)
		return ERR_PTR(-EINVAL);

	ret = state_from_node(state, node, 1);
	if (ret) {
		state_release(state);
		return ERR_PTR(ret);
	}

	return state;
}

/*
 * state_new_from_fdt - create a new state instance from a fdt binary blob
 *
 * @name	The name of the new state instance
 * @fdt		The fdt binary blob describing the new state instance
 */
struct state *state_new_from_fdt(const char *name, void *fdt)
{
	struct state *state;
	struct device_node *root;

	root = of_unflatten_dtb(fdt);
	if (!root)
		return ERR_PTR(-EINVAL);

	state = state_new_from_node(name, root);

	of_delete_node(root);

	return state;
}

/*
 * state_by_name - find a state instance by name
 *
 * @name	The name of the state instance
 */
struct state *state_by_name(const char *name)
{
	struct state *s;

	list_for_each_entry(s, &state_list, list) {
		if (!strcmp(name, s->name))
			return s;
	}

	return NULL;
}

/*
 * state_load - load a state from the backing store
 *
 * @state	The state instance to load
 */
int state_load(struct state *state)
{
	int ret;

	if (!state->backend)
		return -ENOSYS;

	ret = state->backend->load(state->backend, state);
	if (ret)
		state->dirty = 1;
	else
		state->dirty = 0;

	return ret;
}

/*
 * state_save - save a state to the backing store
 *
 * @state	The state instance to save
 */
int state_save(struct state *state)
{
	int ret;

	if (!state->dirty)
		return 0;

	if (!state->backend)
		return -ENOSYS;

	ret = state->backend->save(state->backend, state);
	if (ret)
		return ret;

	state->dirty = 0;

	return 0;
}

void state_info(void)
{
	struct state *s;

	printf("registered state instances:\n");

	list_for_each_entry(s, &state_list, list) {
		printf("%-20s ", s->name);
		if (s->backend)
			printf("(backend: %s, path: %s)\n", s->backend->name, s->backend->path);
		else
			printf("(no backend)\n");
	}
}

static int get_meminfo(const char *path, struct mtd_info_user *meminfo)
{
	int fd, ret;

	fd = open(path, O_RDWR);
	if (fd < 0)
		return fd;

	ret = ioctl(fd, MEMGETINFO, meminfo);

	close(fd);

	if (ret)
		return ret;

	return 0;
}

/*
 * DTB backend implementation
 */
struct state_backend_dtb {
	struct state_backend backend;
	int need_erase;
};

static int state_backend_dtb_load(struct state_backend *backend, struct state *state)
{
	struct device_node *root;
	void *fdt;
	int len, ret;

	fdt = read_file(backend->path, &len);
	if (!fdt) {
		dev_err(&state->dev, "cannot read %s\n", backend->path);
		return -EINVAL;
	}

	root = of_unflatten_dtb(fdt);

	free(fdt);

	if (IS_ERR(root))
		return PTR_ERR(root);

	ret = state_from_node(state, root, 0);

	return ret;
}

static int state_backend_dtb_save(struct state_backend *backend, struct state *state)
{
	struct state_backend_dtb *backend_dtb = container_of(backend,
			struct state_backend_dtb, backend);
	int ret, fd;
	struct device_node *root;
	struct fdt_header *fdt;

	root = state_to_node(state);
	if (IS_ERR(root))
		return PTR_ERR(root);

	fdt = of_flatten_dtb(root);
	if (!fdt)
		return -EINVAL;

	fd = open(backend->path, O_WRONLY);
	if (fd < 0) {
		ret = fd;
		goto out;
	}

	if (backend_dtb->need_erase) {
		ret = erase(fd, fdt32_to_cpu(fdt->totalsize), 0);
		if (ret) {
			close(fd);
			goto out;
		}
	}

	ret = write(fd, fdt, fdt32_to_cpu(fdt->totalsize));

	close(fd);

	if (ret < 0)
		goto out;

	ret = 0;
out:
	free(fdt);
	of_delete_node(root);

	return ret;
}

/*
 * state_backend_dtb_file - create a dtb backend store for a state instance
 *
 * @state	The state instance to work on
 * @path	The path where the state will be stored to
 */
int state_backend_dtb_file(struct state *state, const char *path)
{
	struct state_backend_dtb *backend_dtb;
	struct state_backend *backend;
	struct mtd_info_user meminfo;
	int ret;

	if (state->backend)
		return -EBUSY;

	backend_dtb = xzalloc(sizeof(*backend_dtb));
	backend = &backend_dtb->backend;

	backend->load = state_backend_dtb_load;
	backend->save = state_backend_dtb_save;
	backend->path = xstrdup(path);
	backend->name = "dtb";

	state->backend = backend;

	ret = get_meminfo(backend->path, &meminfo);
	if (!ret)
		backend_dtb->need_erase = 1;

	return 0;
}

/*
 * Raw backend implementation
 */
struct state_backend_raw {
	struct state_backend backend;
	struct state *state;
	unsigned long size_data; /* The raw data size (without magic and crc) */
	unsigned long size_full;
	unsigned long step; /* The step in bytes between two copies */
	off_t offset; /* offset in the storage file */
	size_t size; /* size of the storage area */
	int need_erase;
	int num_copy_read; /* The first successfully read copy */
};

struct backend_raw_header {
	uint32_t magic;
	uint16_t reserved;
	uint16_t data_len;
	uint32_t data_crc;
	uint32_t header_crc;
};

static int backend_raw_load_one(struct state_backend_raw *backend_raw,
		int fd, off_t offset)
{
	struct state *state = backend_raw->state;
	uint32_t crc;
	void *off;
	struct state_variable *sv;
	struct backend_raw_header header = {};
	int ret, len;
	void *buf;

	ret = lseek(fd, offset, SEEK_SET);
	if (ret < 0)
		return ret;

	ret = read(fd, &header, sizeof(header));
	if (ret < 0)
		return ret;
	if (ret < sizeof(header))
		return -EINVAL;

	crc = crc32(0, &header, sizeof(header) - sizeof(uint32_t));
	if (crc != header.header_crc) {
		dev_err(&state->dev, "invalid header crc, calculated 0x%08x, found 0x%08x\n",
				crc, header.header_crc);
		return -EINVAL;
	}

	if (state->magic && state->magic != header.magic) {
		dev_err(&state->dev, "invalid magic 0x%08x, should be 0x%08x\n",
				header.magic, state->magic);
		return -EINVAL;
	}

	buf = xzalloc(header.data_len);

	ret = read(fd, buf, header.data_len);
	if (ret < 0)
		return ret;
	if (ret < header.data_len)
		return -EINVAL;

	crc = crc32(0, buf, header.data_len);
	if (crc != header.data_crc) {
		dev_err(&state->dev, "invalid crc, calculated 0x%08x, found 0x%08x\n",
				crc, header.data_crc);
		return -EINVAL;
	}

	off = buf;
	len = header.data_len;

	list_for_each_entry(sv, &state->variables, list) {
		if (len < sv->raw_size) {
			break;
		}
		memcpy(sv->raw, off, sv->raw_size);
		off += sv->raw_size;
		len -= sv->raw_size;
	}

	free(buf);

	return 0;
}

static int state_backend_raw_load(struct state_backend *backend, struct state *state)
{
	struct state_backend_raw *backend_raw = container_of(backend,
			struct state_backend_raw, backend);
	int ret = 0, fd, i;

	fd = open(backend->path, O_RDONLY);
	if (fd < 0)
		return fd;

	for (i = 0; i < 2; i++) {
		off_t offset = backend_raw->offset + i * backend_raw->step;

		ret = backend_raw_load_one(backend_raw, fd, offset);
		if (!ret) {
			backend_raw->num_copy_read = i;
			dev_dbg(&state->dev, "copy %d successfully loaded\n", i);
			break;
		}
	}

	close(fd);

	return ret;
}

static int backend_raw_write_one(struct state_backend_raw *backend_raw,
		int fd, int num, void *buf, size_t size)
{
	int ret;
	off_t offset = backend_raw->offset + num * backend_raw->step;

	dev_dbg(&backend_raw->state->dev, "%s: 0x%08lx 0x%08x\n",
			__func__, offset, size);

	ret = lseek(fd, offset, SEEK_SET);
	if (ret < 0)
		return ret;

	if (backend_raw->need_erase) {
		ret = erase(fd, backend_raw->size_full, offset);
		if (ret)
			return ret;
	}

	ret = write(fd, buf, size);
	if (ret < 0)
		return ret;

	return 0;
}

static int state_backend_raw_save(struct state_backend *backend, struct state *state)
{
	struct state_backend_raw *backend_raw = container_of(backend,
			struct state_backend_raw, backend);
	int ret = 0, size, fd;
	void *buf, *freep, *off, *data, *tmp;
	struct backend_raw_header *header;
	struct state_variable *sv;

	size = backend_raw->size_data + sizeof(struct backend_raw_header);

	freep = off = buf = xzalloc(size);

	header = buf;
	data = buf + sizeof(*header);

	tmp = data;
	list_for_each_entry(sv, &state->variables, list) {
		memcpy(tmp, sv->raw, sv->raw_size);
		tmp += sv->raw_size;
	}

	header->magic = state->magic;
	header->data_len = backend_raw->size_data;
	header->data_crc = crc32(0, data, backend_raw->size_data);
	header->header_crc = crc32(0, header, sizeof(*header) - sizeof(uint32_t));

	fd = open(backend->path, O_WRONLY);
	if (fd < 0)
		return fd;

	ret = backend_raw_write_one(backend_raw, fd, !backend_raw->num_copy_read, buf, size);
	if (ret)
		goto out;

	ret = backend_raw_write_one(backend_raw, fd, backend_raw->num_copy_read, buf, size);
	if (ret)
		goto out;

	dev_dbg(&state->dev, "wrote state to %s\n", backend->path);
out:
	close(fd);
	free(buf);

	return ret;
}

/*
 * state_backend_raw_file - create a raw file backend store for a state instance
 *
 * @state	The state instance to work on
 * @path	The path where the state will be stored to
 * @offset	The offset in the storage file
 * @size	The maximum size to use in the storage file
 *
 * This backend stores raw binary data from a state instance. The binary data is
 * protected with a magic value which has to match and a crc32 that must be valid.
 * Up to four copies are stored if there is sufficient space available.
 * @path can be a path to a device or a regular file. When it's a device @size may
 * be 0. The four copies a spread to different eraseblocks if approriate for this
 * device.
 */
int state_backend_raw_file(struct state *state, const char *path, off_t offset,
		size_t size)
{
	struct state_backend_raw *backend_raw;
	struct state_backend *backend;
	struct state_variable *sv;
	int ret;
	struct stat s;
	struct mtd_info_user meminfo;

	if (state->backend)
		return -EBUSY;

	ret = stat(path, &s);
	if (!ret && S_ISCHR(s.st_mode)) {
		if (size == 0)
			size = s.st_size;
		else if (offset + size > s.st_size)
			return -EINVAL;
	}

	backend_raw = xzalloc(sizeof(*backend_raw));
	backend = &backend_raw->backend;

	backend->load = state_backend_raw_load;
	backend->save = state_backend_raw_save;
	backend->path = xstrdup(path);
	backend->name = "raw";

	list_for_each_entry(sv, &state->variables, list)
		backend_raw->size_data += sv->raw_size;

	backend_raw->state = state;
	backend_raw->offset = offset;
	backend_raw->size = size;
	backend_raw->size_full = backend_raw->size_data + sizeof(struct backend_raw_header);

	state->backend = backend;

	ret = get_meminfo(backend->path, &meminfo);
	if (!ret) {
		backend_raw->need_erase = 1;
		backend_raw->step = ALIGN(backend_raw->size_full, meminfo.erasesize);
		dev_dbg(&state->dev, "is a mtd, adjust stepsize to %ld\n", backend_raw->step);
	} else {
		backend_raw->step = backend_raw->size_full;
	}

	if (backend_raw->size / backend_raw->step < 2) {
		dev_err(&state->dev, "not enough space for two copies\n");
		ret = -ENOSPC;
		goto err;
	}

	return 0;
err:
	free(backend_raw);
	return ret;
}
