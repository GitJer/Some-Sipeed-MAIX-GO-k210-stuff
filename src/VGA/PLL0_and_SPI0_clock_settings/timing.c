#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Although the MaixGo has a 24MHz crystal, the base clock used in the calculations below is defined as:
#define SYSCTRL_CLOCK_FREQ_IN0 (26000000UL)

/* this function is adapted from "sysctl.c" which is released under the Apache License Version 2.0: */
/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
static uint32_t sysctl_pll_source_set_freq(uint32_t freq)
{
    uint32_t freq_in = 0;

    /*
     * Check input clock source
     */
    freq_in = SYSCTRL_CLOCK_FREQ_IN0;
    /*
     * Check input clock freq
     */

    /*
     * Begin calculate PLL registers' value
     */

    /* constants */
    const double vco_min = 3.5e+08;
    const double vco_max = 1.75e+09;
    const double ref_min = 1.36719e+07;
    const double ref_max = 1.75e+09;
    const int nr_min = 1;
    const int nr_max = 16;
    const int nf_min = 1;
    const int nf_max = 64;
    const int no_min = 1;
    const int no_max = 16;
    const int nb_min = 1;
    const int nb_max = 64;
    const int max_vco = 1;
    const int ref_rng = 1;

    /* variables */
    int nr = 0;
    int nrx = 0;
    int nf = 0;
    int nfi = 0;
    int no = 0;
    int noe = 0;
    int not = 0;
    int nor = 0;
    int nore = 0;
    int nb = 0;
    int first = 0;
    int firstx = 0;
    int found = 0;

    long long nfx = 0;
    double fin = 0, fout = 0, fvco = 0;
    double val = 0, nval = 0, err = 0, merr = 0, terr = 0;
    int x_nrx = 0, x_no = 0, x_nb = 0;
    long long x_nfx = 0;
    double x_fvco = 0, x_err = 0;

    fin = freq_in;
    fout = freq;
    val = fout / fin;
    terr = 0.5 / ((double)(nf_max / 2));
    first = firstx = 1;
    if (terr != -2)
    {
        first = 0;
        if (terr == 0)
            terr = 1e-16;
        merr = fabs(terr);
    }
    found = 0;
    for (nfi = val; nfi < nf_max; ++nfi)
    {
        nr = rint(((double)nfi) / val);
        if (nr == 0)
            continue;
        if ((ref_rng) && (nr < nr_min))
            continue;
        if (fin / ((double)nr) > ref_max)
            continue;
        nrx = nr;
        nf = nfx = nfi;
        nval = ((double)nfx) / ((double)nr);
        if (nf == 0)
            nf = 1;
        err = 1 - nval / val;

        if ((first) || (fabs(err) < merr * (1 + 1e-6)) || (fabs(err) < 1e-16))
        {
            not = floor(vco_max / fout);
            for (no = (not > no_max) ? no_max : not; no > no_min; --no)
            {
                if ((ref_rng) && ((nr / no) < nr_min))
                    continue;
                if ((nr % no) == 0)
                    break;
            }
            if ((nr % no) != 0)
                continue;
            nor = ((not > no_max) ? no_max : not) / no;
            nore = nf_max / nf;
            if (nor > nore)
                nor = nore;
            noe = ceil(vco_min / fout);
            if (!max_vco)
            {
                nore = (noe - 1) / no + 1;
                nor = nore;
                not = 0; /* force next if to fail */
            }
            if ((((no * nor) < (not >> 1)) || ((no * nor) < noe)) && ((no * nor) < (nf_max / nf)))
            {
                no = nf_max / nf;
                if (no > no_max)
                    no = no_max;
                if (no > not)
                    no = not;
                nfx *= no;
                nf *= no;
                if ((no > 1) && (!firstx))
                    continue;
                /* wait for larger nf in later iterations */
            }
            else
            {
                nrx /= no;
                nfx *= nor;
                nf *= nor;
                no *= nor;
                if (no > no_max)
                    continue;
                if ((nor > 1) && (!firstx))
                    continue;
                /* wait for larger nf in later iterations */
            }

            nb = nfx;
            if (nb < nb_min)
                nb = nb_min;
            if (nb > nb_max)
                continue;

            fvco = fin / ((double)nrx) * ((double)nfx);
            if (fvco < vco_min)
                continue;
            if (fvco > vco_max)
                continue;
            if (nf < nf_min)
                continue;
            if ((ref_rng) && (fin / ((double)nrx) < ref_min))
                continue;
            if ((ref_rng) && (nrx > nr_max))
                continue;
            if (!(((firstx) && (terr < 0)) || (fabs(err) < merr * (1 - 1e-6)) || ((max_vco) && (no > x_no))))
                continue;
            if ((!firstx) && (terr >= 0) && (nrx > x_nrx))
                continue;

            found = 1;
            x_no = no;
            x_nrx = nrx;
            x_nfx = nfx;
            x_nb = nb;
            x_fvco = fvco;
            x_err = err;
            first = firstx = 0;
            merr = fabs(err);
            if (terr != -1)
                continue;
        }
    }
    if (!found)
    {
        return 0;
    }

    nrx = x_nrx;
    nfx = x_nfx;
    no = x_no;
    nb = x_nb;
    fvco = x_fvco;
    err = x_err;
    if ((terr != -2) && (fabs(err) >= terr * (1 - 1e-6)))
    {
        return 0;
    }

    freq_in = SYSCTRL_CLOCK_FREQ_IN0;

    uint32_t freq_out = ((double)freq_in * (double)nfx) / ((double)no * (double)nrx);

    return freq_out;
}

int main()
{
    // starting frequency for pll0
    uint32_t min_f_pll0 = 200000000;
    // end frequency for pll0
    uint32_t max_f_pll0 = 800000000;
    // the spi frequency that is requested and should be approached as closely as possible
    //uint32_t requested_f_spi = 25175000;
    uint32_t requested_f_spi = 65000000;

    // step size to go through the pll0 frequency range
    uint32_t f_pll0_step = 1000000;

    //closest_diff, diff;
    uint32_t f_pll0_req, f_pll0_act, f_spi0_req, f_spi0_act;

    // variables to store the nearest matches above and below the requested spi frequency
    uint32_t low_closest_pll0=-1, low_closest_spi0=-1, low_closest_value=-1;
    uint32_t low_diff = 1e9;
    uint32_t high_closest_pll0=-1, high_closest_spi0=-1, high_closest_value=-1;
    uint32_t high_diff = 1e9;

    printf("The requested SPI0 frequency is %d\n", requested_f_spi);

    // sweep the range from min to max pll0 frequencies to test
    for (f_pll0_req=min_f_pll0; f_pll0_req<=max_f_pll0; f_pll0_req+=f_pll0_step) {
        // determine the freq. the system would get based on the requested freq.
        f_pll0_act = sysctl_pll_source_set_freq(f_pll0_req);

        // the calculation of the spi freq uses half of the pll0 frequency
        uint32_t clock_div = f_pll0_act>>1;

        // sweep the range of trusthworthy baudr factors (you can ask for a
        // baudr from 2 to 65534, but 2-5 are unstable for longer data packages
        for (uint32_t spi_baudr=6; spi_baudr<65535; spi_baudr++) {
            uint32_t f_spi0_act = clock_div / spi_baudr;
            // NOTE: normally you can ask for a SPI frequency, but you may not get it!
            //       Here, however, we have calculated the requested spi0 frequency in the same
            //       way as the actual code calculates it, so we do get wat we request!
            // printf("baud=%d actual pll0=%d (req=%d) spi0=%d\n", spi_baudr, f_pll0_act, f_pll0_req, f_spi0_req);


            int32_t diff = f_spi0_act - requested_f_spi;
            if (diff < 0) {
                // the actual frequency is below the requested freq.
                diff = abs(diff);
                if (diff < low_diff) {
                    low_diff = diff;
                    low_closest_pll0 = f_pll0_act;
                    low_closest_spi0 = f_spi0_act;
                } 
            } else {
                // the actual frequency is above the requested freq.
                if (diff < high_diff) {
                    high_diff = diff;
                    high_closest_pll0 = f_pll0_act;
                    high_closest_spi0 = f_spi0_act;
                }
            }
        }
    }
    printf("below: pll0=%d spi0=%d (diff=%d)\n", low_closest_pll0, low_closest_spi0, low_diff);
    printf("above: pll0=%d spi0=%d (diff=%d)\n", high_closest_pll0, high_closest_spi0, high_diff);
}
