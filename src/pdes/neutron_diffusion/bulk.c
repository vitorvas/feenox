/*------------ -------------- -------- --- ----- ---   --       -            -
 *  feenox's routines for neutron diffusion FEM: bulk elements
 *
 *  Copyright (C) 2021--2023 jeremy theler
 *
 *  This file is part of FeenoX <https://www.seamplex.com/feenox>.
 *
 *  feenox is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  FeenoX is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FeenoX.  If not, see <http://www.gnu.org/licenses/>.
 *------------------- ------------  ----    --------  --     -       -         -
 */
#include "feenox.h"
#include "neutron_diffusion.h"
#include "pdes/neutron_transport/neutron_transport.h"

int feenox_problem_build_allocate_aux_neutron_diffusion(unsigned int n_nodes) {
  
  neutron_diffusion.n_nodes = n_nodes;
  int size = neutron_diffusion.n_nodes * neutron_diffusion.groups;
    
  if (neutron_diffusion.Ki != NULL) {
    gsl_matrix_free(neutron_diffusion.Ki);
    gsl_matrix_free(neutron_diffusion.Ai);
    gsl_matrix_free(neutron_diffusion.Xi);
    gsl_matrix_free(neutron_diffusion.DB);
    gsl_matrix_free(neutron_diffusion.AH);
    gsl_matrix_free(neutron_diffusion.Xi);
    if (neutron_diffusion.has_fission) {
      gsl_matrix_free(neutron_diffusion.XH);
    }
  }
    
  feenox_check_alloc(neutron_diffusion.Ki = gsl_matrix_calloc(size, size));
  feenox_check_alloc(neutron_diffusion.Ai = gsl_matrix_calloc(size, size));
  feenox_check_alloc(neutron_diffusion.Xi = gsl_matrix_calloc(size, size));
  feenox_check_alloc(neutron_diffusion.DB = gsl_matrix_calloc(neutron_diffusion.groups * feenox.pde.dim, size));
  feenox_check_alloc(neutron_diffusion.AH = gsl_matrix_calloc(neutron_diffusion.groups, size));
  if (neutron_diffusion.has_fission) {
    feenox_check_alloc(neutron_diffusion.XH = gsl_matrix_calloc(neutron_diffusion.groups, size));
  }

  return FEENOX_OK;
}



int feenox_problem_build_volumetric_gauss_point_neutron_diffusion(element_t *e, unsigned int v) {

#ifdef HAVE_PETSC
  
  feenox_call(feenox_mesh_compute_wHB_at_gauss(e, v));
  double *x = feenox_mesh_compute_x_if_needed(e, v, neutron_diffusion.space_XS);
  material_t *material = feenox_mesh_get_material(e);
  
  gsl_matrix_set_zero(neutron_diffusion.diff);
  gsl_matrix_set_zero(neutron_diffusion.removal);
  if (neutron_transport.has_fission) {
    gsl_matrix_set_zero(neutron_diffusion.nufission);
  }
  
  for (int g = 0; g < neutron_diffusion.groups; g++) {
    // independent sources
    if (neutron_diffusion.has_sources) {
      // TODO: macro to make it nicer
      gsl_vector_set(neutron_diffusion.src, g, neutron_diffusion.S[g].eval(&neutron_diffusion.S[g], x, material));
    }
    
    // scattering and fission
    for (int g_prime = 0; g_prime < neutron_diffusion.groups; g_prime++) {
      gsl_matrix_set(neutron_diffusion.removal, g, g_prime, -neutron_diffusion.Sigma_s[g_prime][g].eval(&neutron_diffusion.Sigma_s[g_prime][g], x, material));
    
      if (neutron_diffusion.has_fission) {
        gsl_matrix_set(neutron_diffusion.nufission, g, g_prime, neutron_diffusion.chi[g]*neutron_diffusion.nu_Sigma_f[g_prime].eval(&neutron_diffusion.nu_Sigma_f[g_prime], x, material));
      }  
    }
    
    // absorption
    double xi = gsl_matrix_get(neutron_diffusion.removal, g, g);
    if (neutron_diffusion.Sigma_t[g].defined) {
      xi += neutron_diffusion.Sigma_t[g].eval(&neutron_diffusion.Sigma_t[g], x, material);
    } else {
      xi += neutron_diffusion.Sigma_a[g].eval(&neutron_diffusion.Sigma_a[g], x, material);
      for (int g_prime = 0; g_prime < neutron_diffusion.groups; g_prime++) {
        xi += neutron_diffusion.Sigma_s[g][g_prime].eval(&neutron_diffusion.Sigma_s[g][g_prime], x, material);
      }
    }
    gsl_matrix_set(neutron_diffusion.removal, g, g, xi);
    
    // leaks
    for (int m = 0; m < feenox.pde.dim; m++) {
      if (neutron_diffusion.D[g].defined) {
        xi = neutron_diffusion.D[g].eval(&neutron_diffusion.D[g], x, material);
      } else if (neutron_diffusion.Sigma_t[g].defined) {
        xi = 1.0/(3.0*neutron_diffusion.Sigma_t[g].eval(&neutron_diffusion.Sigma_t[g], x, material));
      } else {
        feenox_push_error_message("neither D nor Sigma_t given for group %d", g);
        return FEENOX_ERROR;
      }

      unsigned int index = m*neutron_diffusion.groups + g;
      gsl_matrix_set(neutron_diffusion.diff, index, index, xi);
    }
  }

  if (neutron_diffusion.n_nodes != e->type->nodes) {
    feenox_call(feenox_problem_build_allocate_aux_neutron_diffusion(e->type->nodes));
  }

  gsl_matrix_set_zero(neutron_diffusion.Ki);
  gsl_matrix_set_zero(neutron_diffusion.Ai);
  gsl_matrix_set_zero(neutron_diffusion.Xi);
  
  // elemental stiffness for the diffusion term B'*D*B
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, neutron_diffusion.diff, e->B[v], 0.0, neutron_diffusion.DB));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[v], e->B[v], neutron_diffusion.DB, 1.0, neutron_diffusion.Ki));

  // elemental scattering H'*A*H
  // TODO: can we mix Ai and Ki?
  feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, neutron_diffusion.removal, e->H[v], 0.0, neutron_diffusion.AH));
  feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[v], e->H[v], neutron_diffusion.AH, 1.0, neutron_diffusion.Ai));
  
  // elemental fission matrix
  if (neutron_diffusion.has_fission) {
    feenox_call(gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, neutron_diffusion.nufission, e->H[v], 0, neutron_diffusion.XH));
    feenox_call(gsl_blas_dgemm(CblasTrans, CblasNoTrans, e->w[v], e->H[v], neutron_diffusion.XH, 1.0, neutron_diffusion.Xi));
  }
  
  
  if (neutron_diffusion.has_sources) {
    feenox_call(gsl_blas_dgemv(CblasTrans, e->w[v], e->H[v], neutron_diffusion.src, 1.0, feenox.pde.bi));
  }
  
  // for source-driven problems
  //   K = Ki + Ai - Xi
  // for criticallity problems
  //   K = Ki + Ai
  //   M = Xi
  gsl_matrix_add(neutron_diffusion.Ki, neutron_diffusion.Ai);
  if (neutron_diffusion.has_fission) {
    if (neutron_diffusion.has_sources) {
      gsl_matrix_scale(neutron_diffusion.Xi, -1.0);
      gsl_matrix_add(neutron_diffusion.Ki, neutron_diffusion.Xi);
    } else {
      gsl_matrix_add(feenox.pde.Mi, neutron_diffusion.Xi);
    }  
  }
  gsl_matrix_add(feenox.pde.Ki, neutron_diffusion.Ki);
  
 
#endif
  
  return FEENOX_OK;
  
}

