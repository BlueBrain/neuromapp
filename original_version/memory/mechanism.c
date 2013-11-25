#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "include/section.h"

#define gamma _p[0]
#define decay _p[1]
#define depth _p[2]
#define minCai _p[3]
#define ica _p[4]
#define cai _p[5]
#define Dcai _p[6]
#define v _p[7]
#define _g _p[8]
//#define _ion_ica	*_ppvar[0]._pval
//#define _ion_cai	*_ppvar[1]._pval
//#define _style_ca	*((int*)_ppvar[2]._pvoid)
 
void nrn_alloc(Prop *_prop, int iMech, int iNumVars)
{
  Prop *prop_ion, *need_memb();
  double *_p; Datum *_ppvar;

  /// Allocate the double array
  _p = nrn_prop_data_alloc(iMech, iNumVars, _prop);
  assert(_p != NULL);
  /*initialize range parameters*/
//  gamma = 0.05;

  _prop->param = _p;
  _prop->param_size = iNumVars;

  _ppvar = nrn_prop_datum_alloc(iMech, 4, _prop);
  assert(_ppvar != NULL);
  _prop->dparam = _ppvar;

//  _ppvar[0].pval = &prop_ion->param[3]; /* ica */
//  _ppvar[1].pval = &prop_ion->param[1]; /* cai */
//  _ppvar[2]._pvoid = (void*)(&(prop_ion->dparam[0].i)); /* iontype for ca */
}

