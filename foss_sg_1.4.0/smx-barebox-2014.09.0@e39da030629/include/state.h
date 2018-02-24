#ifndef __STATE_H
#define __STATE_H

enum state_variable_type {
	STATE_TYPE_INVALID = 0,
	STATE_TYPE_ENUM,
	STATE_TYPE_U32,
	STATE_TYPE_MAC,
};

struct state_backend;

struct state {
	struct device_d dev;
	struct list_head variables;
	const char *name;
	struct list_head list;
	struct state_backend *backend;
	uint32_t magic;
	unsigned int dirty;
};

struct state_backend {
	int (*load)(struct state_backend *backend, struct state *state);
	int (*save)(struct state_backend *backend, struct state *state);
	const char *name;
	char *path;
};

int state_backend_dtb_file(struct state *state, const char *path);
int state_backend_raw_file(struct state *state, const char *path,
		off_t offset, size_t size);

struct state *state_new_from_fdt(const char *name, void *fdt);
struct state *state_new_from_node(const char *name, struct device_node *node);

struct state *state_by_name(const char *name);
int state_load(struct state *state);
int state_save(struct state *state);
void state_info(void);

#endif /* __STATE_H */
