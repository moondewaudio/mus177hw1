/* code for the "obj3" pd class.  This adds an outlet and a state variable.  */

#define PD_LONGINTTYPE long long
#include "m_pd.h"

static t_class *moonfold_class;

typedef struct _moonfold {
	t_object x_obj;
	t_float foldsig, cliplim;
	t_outlet *sig_out;
} t_moonfold;

void moonfold_fold(t_moonfold *x)
{
	t_float sig = x->foldsig;
	t_float cl = x->cliplim;
	t_float temp;
	t_float newval = sig;

	while ((newval > cl) || (newval < 0)) {		// fold input if its outside of [0, clip lim]
		if (newval > cl) {
			temp = newval - cl;
			newval = cl - temp;
		}
		else {
			newval = x->foldsig;
		}
		if (newval < 0) {						// fold input back up if first fold dropped it below 0
			newval = 0 - newval;
		}
	}
	outlet_float(x->sig_out, newval);
}

void moonfold_fin(t_moonfold *x, t_floatarg f)
{
	x->foldsig = f;
	moonfold_fold(x);
}

void moonfold_clim(t_moonfold *x, t_floatarg f)
{
	if (f < 1) f = 1.0f;						// set minimum clip lim level for folding
	x->cliplim = f;
	moonfold_fold(x);
}

void *moonfold_new(t_floatarg sig, t_floatarg clip)
{
	t_moonfold *x = (t_moonfold *)pd_new(moonfold_class);

	inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("float"), gensym("clim"));

	x->sig_out = outlet_new(&x->x_obj, &s_float);

	x->cliplim = 1.0f;							// default clip lim is set to 1

	return (void *)x;
}

void moonfold_setup(void) {
	moonfold_class = class_new(gensym("moonfold"),
		(t_newmethod)moonfold_new,
		0, sizeof(t_moonfold),
		CLASS_DEFAULT,
		A_DEFFLOAT, A_DEFFLOAT, 0);

	class_addfloat(moonfold_class, (t_method)moonfold_fin);
	class_addmethod(moonfold_class, (t_method)moonfold_clim, gensym("clim"), A_FLOAT, 0);
}
