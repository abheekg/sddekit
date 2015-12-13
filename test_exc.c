/* Apache 2.0 INS-AMU 2015 */

#include <stdio.h>

#include "sk_test.h"
#include "sk_solv.h"
#include "sk_sys.h"
#include "sk_scheme.h"

typedef struct {
	int crossed;
	double tf;
	FILE *fd;
} out_data;

static SK_DEFOUT(test_out)
{
	out_data *d = data;
	fprintf(d->fd, "%f\t%f\t%f\n", t, x[0], x[1]);
	if (x[0] < 0.0)
		d->crossed = 1;
	return t < d->tf;
}

static void test_hist_filler()
{
}

static double x0[2] = {1.010403, 0.030870};

static int for_scheme(sk_sch sch, void *schd, char *name)
{
	sk_sys_exc_dat sysd;
	out_data outd;
	struct sk_solv solv;
	char dat_name[100];

	/* init solver */
	sk_solv_init(&solv, &sk_sys_exc, &sysd,
		sch, schd, &test_out, &outd,
		&test_hist_filler, 42, 2, x0, 0, NULL, NULL,
		0.0, 0.05);

	/* fill in data */
	outd.tf = 20.0;
	sprintf(dat_name, "test_exc_%s.dat", name);
	outd.fd = fopen(dat_name, "w");
	sysd.a = 1.01;
	sysd.tau = 3.0;

	/* deterministic sub-thresh, no crossing */
	outd.crossed = 0;
	sysd.D = 0.0;
	sk_solv_cont(&solv);
	sk_test_true(!outd.crossed);

	/* stochastic sub-thresh, crossing */
	outd.crossed = 0;
	outd.tf = 40.0;
	sysd.D = 0.05;
	sk_solv_cont(&solv);
	sk_test_true(outd.crossed);

	/* clean up */
	fclose(outd.fd);
	sk_solv_free(&solv);

	return 0;
}


int test_exc()
{
	sk_sch_em_data emd;
	sk_sch_heun_data heund;

	sk_sch_em_init(&emd, 2);
	for_scheme(sk_sch_em, &emd, "em");
	sk_sch_em_free(&emd);
	
	sk_sch_heun_init(&heund, 2);
	for_scheme(sk_sch_heun, &heund, "heun");
	sk_sch_heun_free(&heund);

	return 0;
}