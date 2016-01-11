#define __ARIKKEI_TYPES_C__

/*
 * Basic cross-platform functionality
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "arikkei-strlib.h"
#include "arikkei-interface.h"
#include "arikkei-property.h"
#include "arikkei-reference.h"
#include "arikkei-string.h"
#include "arikkei-value.h"
#include "arikkei-utils.h"
#include "arikkei-function-object.h"

#include "arikkei-types.h"

struct _ClassDef {
	unsigned int type;
	unsigned int parent_type;
	const char *name;
	unsigned int instance_size;
};

const struct _ClassDef defs[] = {
	{ ARIKKEI_TYPE_NONE, ARIKKEI_TYPE_NONE, "none", 0 },
	{ ARIKKEI_TYPE_ANY, ARIKKEI_TYPE_NONE , "any", 0 },
	/* True primitives */
	{ ARIKKEI_TYPE_BOOLEAN, ARIKKEI_TYPE_ANY, "bool", sizeof (unsigned int) },
	{ ARIKKEI_TYPE_INT8, ARIKKEI_TYPE_ANY , "int8", 1 },
	{ ARIKKEI_TYPE_UINT8, ARIKKEI_TYPE_ANY, "uint8", 1 },
	{ ARIKKEI_TYPE_INT16, ARIKKEI_TYPE_ANY , "int16", 2 },
	{ ARIKKEI_TYPE_UINT16, ARIKKEI_TYPE_ANY , "uint16", 2 },
	{ ARIKKEI_TYPE_INT32, ARIKKEI_TYPE_ANY , "int32", 4 },
	{ ARIKKEI_TYPE_UINT32, ARIKKEI_TYPE_ANY , "uint32", 4 },
	{ ARIKKEI_TYPE_INT64, ARIKKEI_TYPE_ANY , "int64", 8 },
	{ ARIKKEI_TYPE_UINT64, ARIKKEI_TYPE_ANY , "uint64", 8 },
	{ ARIKKEI_TYPE_FLOAT, ARIKKEI_TYPE_ANY, "float", 4 },
	{ ARIKKEI_TYPE_DOUBLE, ARIKKEI_TYPE_ANY, "double", 8 },
	{ ARIKKEI_TYPE_POINTER, ARIKKEI_TYPE_ANY, "pointer", sizeof (void *) },
	{ ARIKKEI_TYPE_STRUCT, ARIKKEI_TYPE_ANY, "struct", 0 },

	{ ARIKKEI_TYPE_CLASS, ARIKKEI_TYPE_STRUCT, "class", sizeof (ArikkeiClass) },
	{ ARIKKEI_TYPE_INTERFACE, ARIKKEI_TYPE_STRUCT, "interface", 0 },
	{ ARIKKEI_TYPE_ARRAY, ARIKKEI_TYPE_STRUCT, "array", 0 },
	{ ARIKKEI_TYPE_REFERENCE, ARIKKEI_TYPE_STRUCT, "reference", sizeof (ArikkeiReference) },

	{ ARIKKEI_TYPE_STRING, ARIKKEI_TYPE_REFERENCE, "String", sizeof (ArikkeiString) },
	{ ARIKKEI_TYPE_STRING, ARIKKEI_TYPE_STRUCT, "Value", sizeof (ArikkeiValue) }
};

static unsigned int classes_size = 0;
static unsigned int nclasses = 0;
static ArikkeiClass **classes = NULL;

static unsigned int
arikkei_any_to_string (ArikkeiClass *klass, void *instance, unsigned char *buf, unsigned int len)
{
	char c[32];
	unsigned int tlen, nlen, alen;
	unsigned int l;
	/* Instance of NAME 0xADDRESS */
	tlen = 12;
	nlen = (unsigned int) strlen ((const char *) klass->name);
	sprintf (c, "%p", instance);
	alen = (unsigned int) strlen(c);
	l = (tlen < len) ? tlen : len;
	if (l > 0) memcpy (buf, "Instance of ", l);
	buf += l;
	len -= l;
	l = (nlen < len) ? nlen : len;
	if (l > 0) memcpy (buf, klass->name, l);
	buf += l;
	len -= l;
	if (len > 0) {
		*buf = ' ';
		buf += 1;
		len -= 1;
	}
	l = (alen < len) ? alen : len;
	if (l > 0) memcpy (buf, c, l);
	buf += l;
	len -= l;
	if (len > 0) {
		*buf = 0;
	}
	return tlen + nlen + 1 + alen;
}

static unsigned int
arikkei_none_to_string (ArikkeiClass *klass, void *instance, unsigned char *buf, unsigned int len)
{
	unsigned int i;
	static const char *t = "None";
	for (i = 0; i < len; i++) {
		buf[i] = t[i];
		if (!t[i]) break;
	}
	return i;
}

static unsigned int
arikkei_i32_to_string (ArikkeiClass *klass, void *instance, unsigned char *buf, unsigned int len)
{
	unsigned int i;
	char c[32];
	itoa (*((int *) instance), c, 10);
	for (i = 0; i < len; i++) {
		buf[i] = c[i];
		if (!buf[i]) return i;
	}
	return len;
}

static unsigned int
arikkei_u32_to_string (ArikkeiClass *klass, void *instance, unsigned char *buf, unsigned int len)
{
	unsigned int i;
	char c[32];
	itoa (*((int *) instance), c, 10);
	for (i = 0; i < len; i++) {
		buf[i] = c[i];
		if (!buf[i]) return i;
	}
	return len;
}

static unsigned int
arikkei_float_to_string (ArikkeiClass *klass, void *instance, unsigned char *buf, unsigned int len)
{
	unsigned int pos = 0;
	pos += arikkei_dtoa_exp (buf + pos, len - pos, *((float *) instance), 6, 0);
	if (pos < len) buf[pos] = 0;
	return pos;
}

static unsigned int
arikkei_double_to_string (ArikkeiClass *klass, void *instance, unsigned char *buf, unsigned int len)
{
	unsigned int pos = 0;
	pos += arikkei_dtoa_exp (buf + pos, len - pos, *((double *) instance), 6, 0);
	if (pos < len) buf[pos] = 0;
	return pos;
}

static unsigned int
arikkei_pointer_to_string (ArikkeiClass *klass, void *instance, unsigned char *buf, unsigned int len)
{
	unsigned int i;
	const char *t;
	char b[32];
	if (instance) {
		static const char *c = "0123456789abcdef";
		unsigned long long v = (unsigned long long) instance;
		unsigned int l = 2 * sizeof (void *);
		for (i = 0; i < l; i++) {
			b[l - 1 - i] = c[v & 0xf];
			v = v >> 4;
		}
		b[i] = 0;
		t = b;
	} else {
		t = "Null";
	}
	for (i = 0; i < len; i++) {
		buf[i] = t[i];
		if (!t[i]) break;
	}
	return i;
}

static unsigned int
arikkei_string_to_string (ArikkeiClass *klass, void *instance, unsigned char *buf, unsigned int len)
{
	unsigned int pos = 0;
	if (instance) {
		ArikkeiString *str = (ArikkeiString *) instance;
		unsigned int slen = (str->length > len) ? len : str->length;
		memcpy (buf + pos, str->str, slen);
		pos += slen;
	}
	if (pos < len) buf[pos] = 0;
	return pos;
}

static void
arikkei_reference_instance_init (void *instance)
{
	ArikkeiReference *ref = ARIKKEI_REFERENCE(instance);
	ref->refcount = 1;
}

void arikkei_types_init (void)
{
	int i;
	if (classes) return;
	classes_size = 32;
	classes = (ArikkeiClass **) malloc (classes_size * sizeof (ArikkeiClass *));
	/* NONE */
	for (i = 0; i < ARIKKEI_TYPE_NUM_PRIMITIVES; i++) {
		if (i == ARIKKEI_TYPE_INTERFACE) {
			classes[i] = (ArikkeiClass *) malloc (sizeof (ArikkeiInterfaceClass));
			memset (classes[i], 0, sizeof (ArikkeiInterfaceClass));
		} else {
			classes[i] = (ArikkeiClass *) malloc (sizeof (ArikkeiClass));
			memset (classes[i], 0, sizeof (ArikkeiClass));
		}
		if (defs[i].parent_type) {
			memcpy (classes[i], classes[defs[i].parent_type], classes[defs[i].parent_type]->class_size);
			classes[i]->parent = classes[defs[i].parent_type];
		}
		classes[i]->implementation.type = defs[i].type;
		classes[i]->name = (const unsigned char *) defs[i].name;
		classes[i]->class_size = sizeof (ArikkeiClass);
		classes[i]->instance_size = defs[i].instance_size;
		classes[i]->element_size = classes[i]->instance_size;
		classes[i]->zero_memory = 0;

		if (i == ARIKKEI_TYPE_NONE) classes[i]->to_string = arikkei_none_to_string;
		if (i == ARIKKEI_TYPE_ANY) classes[i]->to_string = arikkei_any_to_string;
		if (i == ARIKKEI_TYPE_INT32) {
			classes[i]->to_string = arikkei_i32_to_string;
		}
		if (i == ARIKKEI_TYPE_UINT32) {
			classes[i]->to_string = arikkei_u32_to_string;
		}
		if (i == ARIKKEI_TYPE_FLOAT) {
			classes[i]->to_string = arikkei_float_to_string;
		}
		if (i == ARIKKEI_TYPE_DOUBLE) {
			classes[i]->to_string = arikkei_double_to_string;
		}
		if (i == ARIKKEI_TYPE_POINTER) classes[i]->to_string = arikkei_pointer_to_string;
		if (i == ARIKKEI_TYPE_STRING) classes[i]->to_string = arikkei_string_to_string;
		if (i == ARIKKEI_TYPE_REFERENCE) classes[i]->instance_init = arikkei_reference_instance_init;
		if (i == ARIKKEI_TYPE_INTERFACE) {
			((ArikkeiInterfaceClass *) classes[i])->implementation_size = sizeof (ArikkeiInterfaceImplementation);
		}
		if (i == ARIKKEI_TYPE_VALUE) {
			classes[i]->zero_memory = 1;
		}
		nclasses += 1;
	}
}

static void *
allocate_default (ArikkeiClass *klass)
{
	return malloc (klass->instance_size);
}

static void *
allocate_array_default (ArikkeiClass *klass, unsigned int nelements)
{
	return malloc (nelements * klass->element_size);
}

static void
free_default (ArikkeiClass *klass, void *location)
{
	free (location);
}

static void 
free_array_default (ArikkeiClass *klass, void *location, unsigned int nelements)
{
	free (location);
}

static void
duplicate_default (ArikkeiClass *klass, void *destination, void *instance)
{
	memcpy (destination, instance, klass->instance_size);
}

static void
assign_default (ArikkeiClass *klass, void *destination, void *instance)
{
	klass->instance_finalize (destination);
	memcpy (destination, instance, klass->instance_size);
}

void
arikkei_register_class (ArikkeiClass *klass, unsigned int *type, unsigned int parent, const unsigned char *name, unsigned int class_size, unsigned int instance_size,
						void (* class_init) (ArikkeiClass *), void (* instance_init) (void *), void (* instance_finalize) (void *))
{
	if (!classes) arikkei_types_init ();
	arikkei_return_if_fail (klass != NULL);
	arikkei_return_if_fail ((parent == ARIKKEI_TYPE_NONE) || (class_size >= classes[parent]->class_size));
	arikkei_return_if_fail ((parent == ARIKKEI_TYPE_NONE) || (instance_size >= classes[parent]->instance_size));
	if (nclasses >= classes_size) {
		classes_size += 32;
		classes = (ArikkeiClass **) realloc (classes, classes_size * sizeof (ArikkeiClass *));
	}
	*type = nclasses;
	nclasses += 1;

	classes[*type] = klass;
	if (parent == ARIKKEI_TYPE_NONE) {
		/* Default methods */
		/* Memory management */
		klass->allocate = allocate_default;
		klass->allocate_array = allocate_array_default;
		klass->free = free_default;
		klass->free_array = free_array_default;
	} else {
		memcpy (klass, classes[parent], classes[parent]->class_size);
		klass->parent = classes[parent];
		klass->firstinterface = klass->parent->firstinterface + klass->parent->ninterfaces;
		klass->ninterfaces = 0;
		klass->implementations = NULL;
		klass->firstproperty = klass->parent->firstproperty + klass->parent->nproperties;
		klass->nproperties = 0;
		klass->properties = NULL;
	}
	klass->implementation.type = *type;
	klass->name = (unsigned char *) strdup ((const char *) name);
	/* Sizes */
	klass->class_size = class_size;
	klass->instance_size = instance_size;
	klass->element_size = instance_size;
	/* Constructors and destructors */
	klass->instance_init = instance_init;
	klass->instance_finalize = instance_finalize;

	if (class_init) class_init (klass);
}

ArikkeiClass *
arikkei_register_type (unsigned int *type, unsigned int parent, const unsigned char *name, unsigned int class_size, unsigned int instance_size,
				       void (* class_init) (ArikkeiClass *),
					   void (* instance_init) (void *),
					   void (* instance_finalize) (void *))
{
	ArikkeiClass *klass;
	if (!classes) arikkei_types_init ();
	arikkei_return_val_if_fail ((parent == ARIKKEI_TYPE_NONE) || (class_size >= classes[parent]->class_size), NULL);
	arikkei_return_val_if_fail ((parent == ARIKKEI_TYPE_NONE) || (instance_size >= classes[parent]->instance_size), NULL);
	klass = (ArikkeiClass *) malloc (class_size);
	memset (klass, 0, class_size);
	arikkei_register_class (klass, type, parent, name, class_size, instance_size, class_init, instance_init, instance_finalize);
	return klass;
}

ArikkeiClass *
arikkei_type_get_class (unsigned int type)
{
	return classes[type];
}

unsigned int
arikkei_type_is_a (unsigned int type, unsigned int test)
{
	ArikkeiClass *klass;
	arikkei_return_val_if_fail (type < nclasses, 0);
	arikkei_return_val_if_fail (test < nclasses, 0);
	if (type == test) return 1;
	klass = classes[type]->parent;
	while (klass) {
		if (klass->implementation.type == test) return 1;
		klass = klass->parent;
	}
	return 0;
}

unsigned int
arikkei_type_implements_a (unsigned int type, unsigned int test)
{
	arikkei_return_val_if_fail (type < nclasses, 0);
	arikkei_return_val_if_fail (test < nclasses, 0);
	return arikkei_implementation_get_interface (&classes[type]->implementation, test) != NULL;
}

unsigned int
arikkei_type_is_assignable_to (unsigned int type, unsigned int test)
{
	if (arikkei_type_is_a (test, ARIKKEI_TYPE_INTERFACE)) {
		unsigned int val = arikkei_type_implements_a (type, test);
		return val;
	} else {
		return arikkei_type_is_a (type, test);
	}
}

unsigned int
arikkei_type_get_parent_primitive (unsigned int type)
{
	ArikkeiClass *klass;
	arikkei_return_val_if_fail (type < nclasses, 0);
	if (type < ARIKKEI_TYPE_NUM_PRIMITIVES) return type;
	klass = classes[type]->parent;
	while (klass) {
		if (klass->implementation.type < ARIKKEI_TYPE_NUM_PRIMITIVES) return klass->implementation.type;
		klass = klass->parent;
	}
	return 0;
}

unsigned int
arikkei_class_is_of_type (ArikkeiClass *klass, unsigned int type)
{
	arikkei_return_val_if_fail (klass != NULL, 0);
	while (klass) {
		if (klass->implementation.type == type) return 1;
		klass = klass->parent;
	}
	return 0;
}

static void
arikkei_class_tree_interface_invoke_init (ArikkeiClass *klass, ArikkeiInterfaceImplementation *implementation, void *instance)
{
	unsigned int i;
	/* Every interface has to be subclass of ArikkeiInterface */
	if (klass->parent && (klass->parent->implementation.type >= ARIKKEI_TYPE_INTERFACE)) {
		arikkei_class_tree_interface_invoke_init (klass->parent, implementation, instance);
	}
	/* Interfaces */
	for (i = 0; i < klass->ninterfaces; i++) {
		ArikkeiInterfaceImplementation *impl = (ArikkeiInterfaceImplementation *) ((char *) klass + klass->implementations[i]);
		ArikkeiClass *ifclass = arikkei_type_get_class (impl->type);
		if (ifclass->zero_memory) {
			memset ((char *) instance + impl->instance_offset, 0, ifclass->instance_size);
		}
		arikkei_class_tree_interface_invoke_init (ifclass, impl, (char *) instance + impl->instance_offset);
	}
	/* Instance itself */
	if (klass->instance_init) klass->instance_init (instance);
	if (((ArikkeiInterfaceClass *) klass)->instance_init) ((ArikkeiInterfaceClass *) klass)->instance_init (implementation, instance);
}

static void
arikkei_class_tree_instance_invoke_init (ArikkeiClass *klass, void *instance)
{
	unsigned int i;
	/* Initialize parent instances */
	if (klass->parent && (klass->parent->implementation.type != ARIKKEI_TYPE_NONE)) {
		arikkei_class_tree_instance_invoke_init (klass->parent, instance);
	}
	/* Interfaces */
	for (i = 0; i < klass->ninterfaces; i++) {
		ArikkeiInterfaceImplementation *impl = (ArikkeiInterfaceImplementation *) ((char *) klass + klass->implementations[i]);
		ArikkeiClass *ifclass = arikkei_type_get_class (impl->type);
		if (ifclass->zero_memory) {
			memset ((char *) instance + impl->instance_offset, 0, ifclass->instance_size);
		}
		arikkei_class_tree_interface_invoke_init (ifclass, impl, (char *) instance + impl->instance_offset);
	}
	/* Instance itself */
	if (klass->instance_init) klass->instance_init (instance);
}

static void
arikkei_class_tree_interface_invoke_finalize (ArikkeiClass *klass, ArikkeiInterfaceImplementation *implementation, void *instance)
{
	unsigned int i;
	if (((ArikkeiInterfaceClass *) klass)->instance_finalize) ((ArikkeiInterfaceClass *) klass)->instance_finalize (implementation, instance);
	if (klass->instance_finalize) klass->instance_finalize (instance);
	for (i = 0; i < klass->ninterfaces; i++) {
		ArikkeiInterfaceImplementation *impl = (ArikkeiInterfaceImplementation *) ((char *) klass + klass->implementations[i]);
		ArikkeiClass *ifclass = arikkei_type_get_class (impl->type);
		arikkei_class_tree_interface_invoke_finalize (ifclass, impl, (char *) instance + impl->instance_offset);
	}
	if (klass->parent && (klass->parent->implementation.type >= ARIKKEI_TYPE_INTERFACE)) {
		arikkei_class_tree_interface_invoke_finalize (klass->parent, implementation, instance);
	}
}

static void
arikkei_class_tree_instance_invoke_finalize (ArikkeiClass *klass, void *instance)
{
	unsigned int i;
	if (klass->instance_finalize) klass->instance_finalize (instance);
	for (i = 0; i < klass->ninterfaces; i++) {
		ArikkeiInterfaceImplementation *impl = (ArikkeiInterfaceImplementation *) ((char *) klass + klass->implementations[i]);
		ArikkeiClass *ifclass = arikkei_type_get_class(impl->type);
		arikkei_class_tree_interface_invoke_finalize (ifclass, impl, (char *) instance + impl->instance_offset);
	}
	if (klass->parent && (klass->parent->implementation.type != ARIKKEI_TYPE_NONE)) {
		arikkei_class_tree_instance_invoke_finalize (klass->parent, instance);
	}
}

static void
arikkei_implementation_init_recursive (ArikkeiInterfaceClass *interface_class, ArikkeiInterfaceImplementation *implementation)
{
	ArikkeiClass *klass = (ArikkeiClass *) interface_class;
	if (klass->parent && (klass->parent->implementation.type >= ARIKKEI_TYPE_INTERFACE)) {
		arikkei_implementation_init_recursive ((ArikkeiInterfaceClass *) klass->parent, implementation);
	}
	if (interface_class->implementation_init) interface_class->implementation_init (implementation);
}

void *
arikkei_instance_new (unsigned int type)
{
	void *instance;
	ArikkeiClass *klass = arikkei_type_get_class (type);
	arikkei_return_val_if_fail (klass != NULL, NULL);
	if (klass->allocate) {
		instance = klass->allocate (klass);
	} else {
		instance = malloc (klass->instance_size);
	}
	if (klass->zero_memory) memset (instance, 0, klass->instance_size);
	arikkei_class_tree_instance_invoke_init (klass, instance);
	return instance;
}

void *
arikkei_instance_new_array (unsigned int type, unsigned int nelements)
{
	void *elements;
	unsigned int i;
	ArikkeiClass *klass = arikkei_type_get_class (type);
	arikkei_return_val_if_fail (klass != NULL, NULL);
	if (klass->allocate_array) {
		elements = klass->allocate_array (klass, nelements);
	} else {
		elements = malloc (nelements * klass->element_size);
	}
	if (klass->zero_memory) memset (elements, 0, nelements * klass->element_size);
	for (i = 0; i < nelements; i++) {
		void *instance = (char *) elements + i * klass->element_size;
		arikkei_class_tree_instance_invoke_init (klass, instance);
	}
	return elements;
}

void
arikkei_instance_delete (unsigned int type, void *instance)
{
	ArikkeiClass *klass = arikkei_type_get_class (type);
	arikkei_return_if_fail (klass != NULL);
	arikkei_class_tree_instance_invoke_finalize (klass, instance);
	if (klass->free) {
		klass->free (klass, instance);
	} else {
		free (instance);
	}
}

void
arikkei_instance_delete_array (unsigned int type, void *elements, unsigned int nelements)
{
	unsigned int i;
	ArikkeiClass *klass = arikkei_type_get_class (type);
	arikkei_return_if_fail (klass != NULL);
	for (i = 0; i < nelements; i++) {
		void *instance = (char *) elements + i * klass->element_size;
		arikkei_class_tree_instance_invoke_finalize (klass, instance);
	}
	if (klass->free_array) {
		klass->free_array (klass, elements, nelements);
	} else {
		free (elements);
	}
}

void
arikkei_instance_setup (void *instance, unsigned int type)
{
	ArikkeiClass *klass = arikkei_type_get_class (type);
	arikkei_return_if_fail (klass != NULL);
	if (klass->zero_memory) memset (instance, 0, klass->instance_size);
	arikkei_class_tree_instance_invoke_init (klass, instance);
}

void
arikkei_instance_release (void *instance, unsigned int type)
{
	ArikkeiClass *klass;
	klass = arikkei_type_get_class (type);
	arikkei_return_if_fail (klass != NULL);
	arikkei_class_tree_instance_invoke_finalize (klass, instance);
}

const unsigned char *
arikkei_type_get_name (unsigned int type)
{
	return classes[type]->name;
}

unsigned int
arikkei_instance_to_string (ArikkeiClass *klass, void *instance, unsigned char *buf, unsigned int len)
{
	if (klass->to_string) {
		return klass->to_string (klass, instance, buf, len);
	}
	return 0;
}

ArikkeiImplementation *
arikkei_implementation_get_interface (ArikkeiImplementation *impl, unsigned int type)
{
	arikkei_return_val_if_fail (impl != NULL, NULL);
	/* Get our class */
	ArikkeiClass *klass = classes[impl->type];
	while (klass) {
		unsigned int i;
		/* Iterate over interfaces */
		for (i = 0; i < klass->ninterfaces; i++) {
			ArikkeiImplementation *sub_impl = (ArikkeiImplementation *) ((char *) impl + klass->implementations[i]);
			/* If this interface is of requested type we are done */
			if (arikkei_type_is_a (sub_impl->type, type)) return sub_impl;
			/* Check sub-interfaces of this interface */
			sub_impl = arikkei_implementation_get_interface (sub_impl, type);
			if (sub_impl) return sub_impl;
		}
		klass = klass->parent;
	}
	return NULL;
}

ArikkeiProperty *
arikkei_class_lookup_property (ArikkeiClass *klass, const unsigned char *key)
{
	unsigned int i;
	arikkei_return_val_if_fail (klass != NULL, NULL);
	arikkei_return_val_if_fail (key != NULL, NULL);
	for (i = 0; i < klass->nproperties; i++) {
		if (!strcmp ((const char *) key, (const char *) klass->properties[i].key->str)) return &klass->properties[i];
	}
	if (klass->parent) {
		return arikkei_class_lookup_property (klass->parent, key);
	}
	return NULL;
}

unsigned int
arikkei_instance_set_property (ArikkeiClass *klass, void *instance, const unsigned char *key, const ArikkeiValue *val)
{
	ArikkeiProperty *prop;
	arikkei_return_val_if_fail (klass != NULL, 0);
	arikkei_return_val_if_fail (key != NULL, 0);
	arikkei_return_val_if_fail (val != NULL, 0);
	prop = arikkei_class_lookup_property (klass, key);
	if (!prop) return 0;
	return arikkei_instance_set_property_by_id (klass, instance, prop->id, val);
}

unsigned int
arikkei_instance_set_property_by_id (ArikkeiClass *klass, void *instance, unsigned int id, const ArikkeiValue *val)
{
	arikkei_return_val_if_fail (klass != NULL, 0);
	arikkei_return_val_if_fail (val != NULL, 0);
	if (id >= klass->firstproperty) {
		unsigned int idx = id - klass->firstproperty;
		if (!klass->properties[idx].can_write) return 0;
		if (klass->properties[idx].is_final) return 0;
		if (val && !(arikkei_type_is_a (val->type, klass->properties[idx].type) || arikkei_type_implements_a (val->type, klass->properties[idx].type))) return 0;
		if (klass->set_property) {
			return klass->set_property (klass, instance, idx, val);
		}
		return 0;
	} else {
		if (klass->parent) {
			return arikkei_instance_set_property_by_id (klass->parent, instance, id, val);
		}
		return 0;
	}
}

unsigned int
arikkei_instance_get_property (ArikkeiClass *klass, void *instance, const unsigned char *key, ArikkeiValue *val)
{
	ArikkeiProperty *prop;
	arikkei_return_val_if_fail (klass != NULL, 0);
	arikkei_return_val_if_fail (key != NULL, 0);
	arikkei_return_val_if_fail (val != NULL, 0);
	prop = arikkei_class_lookup_property (klass, key);
	if (!prop) return 0;
	return arikkei_instance_get_property_by_id (klass, instance, prop->id, val);
}

unsigned int
arikkei_instance_get_property_by_id (ArikkeiClass *klass, void *instance, unsigned int id, ArikkeiValue *val)
{
	arikkei_return_val_if_fail (klass != NULL, 0);
	arikkei_return_val_if_fail (val != NULL, 0);
	if (id >= klass->firstproperty) {
		unsigned int idx = id - klass->firstproperty;
		if (!klass->properties[idx].can_read) return 0;
		if (klass->properties[idx].is_value) {
			arikkei_value_copy (val, &klass->properties[idx].value);
			return 1;
		} else if (klass->get_property) {
			return klass->get_property (klass, instance, idx, val);
		}
		return 0;
	} else {
		if (klass->parent) {
			return arikkei_instance_get_property_by_id (klass->parent, instance, id, val);
		}
		return 0;
	}
}

void *
arikkei_get_instance_from_containing_instance (ArikkeiImplementation *impl, void *containing_instance)
{
	arikkei_return_val_if_fail (impl != NULL, NULL);
	arikkei_return_val_if_fail (containing_instance != NULL, NULL);
	return (char *) containing_instance + impl->instance_offset;
}

void *
arikkei_get_instance_from_outmost_instance (ArikkeiImplementation *impl, void *outmost_instance)
{
	unsigned int offset = 0;
	arikkei_return_val_if_fail (impl != NULL, NULL);
	arikkei_return_val_if_fail (outmost_instance != NULL, NULL);
	while (impl->parent_offset != 0) {
		offset += impl->instance_offset;
		impl = (ArikkeiImplementation *) ((char *) impl - impl->parent_offset);
	}
	return (char *) outmost_instance + offset;
}

ArikkeiImplementation *
arikkei_get_containing_implementation (ArikkeiImplementation *impl)
{
	arikkei_return_val_if_fail (impl != NULL, NULL);
	return (ArikkeiImplementation *) ((char *) impl - impl->parent_offset);
}

ArikkeiImplementation *
arikkei_get_outmost_implementation (ArikkeiImplementation *impl)
{
	arikkei_return_val_if_fail (impl != NULL, NULL);
	while (impl->parent_offset != 0) {
		impl = (ArikkeiImplementation *) ((char *) impl - impl->parent_offset);
	}
	return impl;
}

void *
arikkei_get_containing_instance (ArikkeiImplementation *impl, void *inst)
{
	arikkei_return_val_if_fail (impl != NULL, NULL);
	arikkei_return_val_if_fail (inst != NULL, NULL);
	return (char *) inst - impl->instance_offset;
}

void *
arikkei_get_outmost_instance (ArikkeiImplementation *impl, void *inst)
{
	arikkei_return_val_if_fail (impl != NULL, NULL);
	arikkei_return_val_if_fail (inst != NULL, NULL);
	while (impl->parent_offset != 0) {
		inst = (char *) inst - impl->instance_offset;
		impl = (ArikkeiImplementation *) ((char *) impl - impl->parent_offset);
	}
	return inst;
}

void *
arikkei_instance_get_interface (void *containing_instance, unsigned int containing_type, unsigned int interface_type, ArikkeiInterfaceImplementation **interface_implementation)
{
	ArikkeiClass *klass;
	arikkei_return_val_if_fail (containing_instance != NULL, NULL);
	arikkei_return_val_if_fail (containing_type != 0, NULL);
	arikkei_return_val_if_fail (containing_type < nclasses, NULL);
	arikkei_return_val_if_fail (interface_type != 0, NULL);
	arikkei_return_val_if_fail (interface_type < nclasses, NULL);
	arikkei_return_val_if_fail (interface_implementation != NULL, NULL);
	klass = classes[containing_type];
	*interface_implementation = arikkei_implementation_get_interface (&klass->implementation, interface_type);
	if (!*interface_implementation) return NULL;
	return arikkei_get_instance_from_containing_instance (*interface_implementation, containing_instance);
}

void
arikkei_class_set_num_interfaces (ArikkeiClass *klass, unsigned int ninterfaces)
{
	if (klass->parent) klass->firstinterface = klass->parent->firstinterface + klass->parent->ninterfaces;
	klass->ninterfaces = ninterfaces;
	klass->implementations = (unsigned int *) malloc (ninterfaces * sizeof (unsigned int));
	memset (klass->implementations, 0, ninterfaces * sizeof (unsigned int));
}

void
arikkei_interface_implementation_setup (ArikkeiClass *klass, unsigned int idx, unsigned int type, unsigned int class_offset, unsigned int instance_offset)
{
	ArikkeiInterfaceImplementation *impl;
	ArikkeiInterfaceClass *ifclass;
	arikkei_return_if_fail(klass != NULL);
	arikkei_return_if_fail (idx < klass->ninterfaces);
	arikkei_return_if_fail (arikkei_type_is_a (type, ARIKKEI_TYPE_INTERFACE));
	klass->implementations[idx] = class_offset;
	impl = (ArikkeiInterfaceImplementation *) ((char *) klass + class_offset);
	impl->type = type;
	impl->parent_offset = class_offset;
	impl->instance_offset = instance_offset;
	ifclass = (ArikkeiInterfaceClass *)arikkei_type_get_class (type);
	arikkei_implementation_init_recursive (ifclass, impl);
}

void
arikkei_class_set_num_properties (ArikkeiClass *klass, unsigned int nproperties)
{
	if (klass->parent) klass->firstproperty = klass->parent->firstproperty + klass->parent->nproperties;
	klass->nproperties = nproperties;
	klass->properties = (ArikkeiProperty *) malloc (nproperties * sizeof (ArikkeiProperty));
	memset (klass->properties, 0, nproperties * sizeof (ArikkeiProperty));
}

void
arikkei_class_property_setup (ArikkeiClass *klass, unsigned int idx, const unsigned char *key, unsigned int type,
							  unsigned int is_static, unsigned int can_read, unsigned int can_write, unsigned int is_final, unsigned int is_value,
							  unsigned int value_type, void *value)
{
	arikkei_return_if_fail (klass != NULL);
	arikkei_return_if_fail (idx < klass->nproperties);
	arikkei_return_if_fail (key != NULL);
	arikkei_return_if_fail (type != ARIKKEI_TYPE_NONE);
	arikkei_return_if_fail (!(can_write && is_final));
	arikkei_return_if_fail (!is_value || is_static);
	if (!((value_type == ARIKKEI_TYPE_NONE) || (arikkei_type_is_assignable_to (value_type, type)))) {
		return;
	}
	arikkei_return_if_fail ((value_type == ARIKKEI_TYPE_NONE) || (arikkei_type_is_assignable_to (value_type, type)));

	arikkei_property_setup (klass->properties + idx, key, type, klass->firstproperty + idx, is_static, can_read, can_write, is_final, is_value, value_type, value);
}

void
arikkei_class_method_setup (ArikkeiClass *klass, unsigned int idx, const unsigned char *key,
							unsigned int rettype, unsigned int nargs, const unsigned int argtypes[],
							unsigned int (*call) (ArikkeiValue *, ArikkeiValue *, ArikkeiValue *))
{
	ArikkeiFunctionObject *fobj;
	fobj = arikkei_function_object_new (klass->implementation.type, rettype, nargs, argtypes, call);
	/* Property is static although function is not static */
	arikkei_class_property_setup (klass, idx, key, ARIKKEI_TYPE_FUNCTION, 1, 1, 0, 1, 1, ARIKKEI_TYPE_FUNCTION_OBJECT, fobj);
	arikkei_object_unref ((ArikkeiObject *) fobj);
}

void
arikkei_class_static_method_setup (ArikkeiClass *klass, unsigned int idx, const unsigned char *key,
								   unsigned int rettype, unsigned int nargs, const unsigned int argtypes[],
								   unsigned int (*call) (ArikkeiValue *, ArikkeiValue *, ArikkeiValue *))
{
	ArikkeiFunctionObject *fobj;
	fobj = arikkei_function_object_new (ARIKKEI_TYPE_NONE, rettype, nargs, argtypes, call);
	arikkei_class_property_setup (klass, idx, key, ARIKKEI_TYPE_FUNCTION, 1, 1, 0, 1, 1, ARIKKEI_TYPE_FUNCTION_OBJECT, fobj);
	arikkei_object_unref ((ArikkeiObject *) fobj);
}
