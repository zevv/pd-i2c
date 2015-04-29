#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pd/m_pd.h>

#include "i2c-dev.h"

static t_class *i2c_class;


struct i2c {
	t_object x_obj;
	t_outlet *f_out;
	int fd;
	int bus;
	int addr;
};



void i2c_get(struct i2c *x, t_float reg)
{
	if(x->fd == -1) {
		post("i2c get error: bus is not opened");
		return;
	}

	int val = i2c_smbus_read_byte_data(x->fd, reg);
	if(val == -1) {
		post("i2c get error: %s", strerror(errno));
		return;
	}
}


void i2c_set(struct i2c *x, t_float reg, t_float val)
{
	if(x->fd == -1) {
		post("i2c set error: bus is not opened");
		return;
	}

	int r = i2c_smbus_write_byte_data(x->fd, reg, val);
	if(r == -1) {
		post("i2c get error: %s", strerror(errno));
		return;
	}
}


void *i2c_new(t_symbol *s, int argc, t_atom *argv)
{
	struct i2c *x = (struct i2c *)pd_new(i2c_class);

	if(argc == 2) {
		x->bus  = atom_getintarg(0, argc, argv);
		x->addr = atom_getintarg(1, argc, argv);


		char fname[32];
		snprintf(fname, sizeof(fname), "/dev/i2c-%d", x->bus);

		x->fd = open(fname, O_RDWR);
		if(x->fd == -1) {
			post("Error opening %s: %s", fname, strerror(errno));
		} else {
			int r = ioctl(x->fd, I2C_SLAVE, x->addr);
			if(r == 0) {
				post("I2C bus %d slave address %d opened", x->bus, x->addr);
			} else {
				post("Error setting slave address: %s", strerror(errno));
			}
		}

	} else {
		post("Missing bus and address argument");
	}

	x->f_out = outlet_new(&x->x_obj, &s_float);

	return (void *)x;
}


void i2c_del(struct i2c *x)
{
	if(x->fd != -1) {
		close(x->fd);
	}
}


void i2c_setup(void) 
{
	i2c_class = class_new(gensym("i2c"), 
			(t_newmethod)i2c_new, 
			(t_method)i2c_del, 
			sizeof(struct i2c), CLASS_DEFAULT, A_GIMME, 0);

	class_addmethod(i2c_class, (t_method)i2c_get, gensym("get"), A_DEFFLOAT, 0);
	class_addmethod(i2c_class, (t_method)i2c_set, gensym("set"), A_DEFFLOAT, A_DEFFLOAT, 0);

	class_sethelpsymbol(i2c_class, gensym("help-i2c"));
}

/*
 * End
 */

