#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "riemann.h"
#include <stdbool.h>

/*
  Translated from a C++ Riemann solver written by Richard
  J. Gonsalves,  which in turn is rewrite of a Riemann solver
  found in Laney's textbook Computational Gasdynamics (Cambridge
  University Press.)     
*/ 

/* Este metodo es referenciado en la pagina: http://www.physics.buffalo.edu/phy411-506/topic7/riemann-solver.hpp
   Se realiza la traduccion de C++ a c

*/

inline double fg(double x) {
  const double gamma = 1.4;
  const double g2 = (gamma + 1) / (2 * gamma);
  return (x-1) / sqrt(g2 * (x - 1) + 1);
}

void Riemann(double *U1, double *U4, double *F) {
  int i;
  
  const double gamma = 1.4;
  const double g1 = (gamma - 1) / (2 * gamma);
  const double g2 = (gamma + 1) / (2 * gamma);
  const double g3 = (gamma + 1) / (gamma - 1);
  const double tol = 1e-15;
  
  // Computando variables primitivas
  double rho1 = U1[0];
  double u1 = U1[1] / rho1;
  double p1 = (U1[2] - rho1 * u1 * u1 / 2) * (gamma - 1);
  double rho4 = U4[0];
  double u4 = U4[1] / rho4;
  double p4 = (U4[2] - rho4 * u4 * u4 / 2) * (gamma - 1);
  
  // switch states if necessary so high pressure is on left
  int revflag = FALSE;
  if (p4 < p1) {
    double swap = p1; p1 = p4; p4 = swap;
    swap = u1; u1 = -u4; u4 = -swap;
    swap = rho1; rho1 = rho4; rho4 = swap;
    revflag = TRUE;
  }
  double a1 = sqrt(gamma * p1 / rho1);
  double a4 = sqrt(gamma * p4 / rho4);
  double p = pow(p4/p1, g1);
  double du = u4 - u1;
  
  // Aplicando el metodo de las secantes
  
  double x = 50  * p4 / p1;
  double y = 50 * p4 / p1;
  
  double fx = p - pow(x, g1) / (1 + g1 * (gamma * du - a1 * fg(x)) / a4);
  double fy = p - pow(y, g1) / (1 + g1 * (gamma * du - a1 * fg(y)) / a4);
  bool converge = false;
  
  for (i = 1; i <= 20; i++) {
    
    double z = y - fy * (y - x) / (fy - fx);
    double fz = p - pow(z, g1) / (1 + g1 * (gamma * du - a1 * fg(z)) / a4);
    
    if (abs(fz) < tol && abs(z - y) < tol) {
      converge = true;
      break;
    }
    
    x = y;
    fx = fy;
    y = z;
    fy = fz;
  }
  
  if (!converge)
    printf("Warning: secant failed to converge in Riemann");
  
  // Compute shock
  double p2 = p1 * x;
  double u2 = u1 + a1 * fg(x) / gamma;
  //     u2 = u4 + 2.*a4*(1.-(x**g1)/p)/(gamma-1.)
  double a2 = a1 * sqrt(x * (g3 + x) / (1 + g3 * x));
  double rho2 = gamma * p2 / (a2 * a2);
  double s1 = u1 + a1 * sqrt(g2 *(x - 1) + 1);
  //     s1 = (rho1*u1 - rho2*u2)/(rho1-rho2);
  
  // Contacto
  double p3 = p2;
  double u3 = u2;
  double a3 = a4 + 0.5 * (gamma - 1) * (u4 - u3);
  double s2 = u2;
  double rho3 = gamma * p3/(a3 * a3);
  
  // Expansion
  double s3 = u3 - a3;
  double s4 = u4 - a4;
  
  // Computando flujos
  double f1, f2, f3, a, u, rho;

  if (revflag) {
    if (s4 > 0) {
      
      f1 = -rho4 * u4;
      f2 = rho4 * u4 * u4 + p4;
      f3 = -0.5 * rho4 * u4 * u4 * u4;
      - rho4 * a4 * a4 * u4 / (gamma - 1);
      
    }
    
    else if (s3 > 0) {
      u = (-(gamma-1.)*u4+2.*a4)/(gamma+1.);
      a = u;
      p = p4*pow(a/a4, 2.*gamma/(gamma-1.));
      if (a < 0 || p < 0) {
	printf("Negative a or p in Riemann");
      }
      
      rho = gamma*p/(a*a);
      f1 = -rho*u;
      f2 = rho*u*u + p ;
      f3 = -.5*rho*u*u*u - rho*a*a*u/(gamma-1.);
    }
    
    else if (s2 > 0) {
      f1 = -rho3*u3;
      f2 = rho3*u3*u3 + p3;
      f3 =  -.5*rho3*u3*u3*u3 - rho3*a3*a3*u3/(gamma-1.);
    }
    
    else if (s1 > 0) {
      f1 = -rho2*u2;
      f2 = rho2*u2*u2 + p2;
      f3 = -.5*rho2*u2*u2*u2 - rho2*a2*a2*u2/(gamma-1.);
    }
    
    else {
	f1 = -rho1*u1;
	f2 = rho1*u1*u1 + p1;
	f3 = -.5*rho1*u1*u1*u1 - rho1*a1*a1*u1/(gamma-1.);
    }
    
  }  //end rev flag
  
  else {
    
    if(s4 > 0) {
      f1 = rho4*u4;
      f2 = rho4*u4*u4 + p4;
      f3 = .5*rho4*u4*u4*u4 + rho4*a4*a4*u4/(gamma-1.);
    }
    
    else if (s3 > 0) {
      u = ((gamma-1.)*u4+2.*a4)/(gamma+1.);
      a = u;
      p = p4*pow(a/a4, 2.*gamma/(gamma-1.));
      if (a < 0 || p < 0) {
	printf("Negative a or p in Riemann");
      }
      rho = gamma*p/(a*a);
      f1 = rho*u;
      f2 = rho*u*u + p;
      f3 = .5*rho*u*u*u + rho*a*a*u/(gamma-1.);
    }
    else if (s2 > 0) {
      f1 = rho3*u3;
      f2 = rho3*u3*u3 + p3;
      f3 =  .5*rho3*u3*u3*u3 + rho3*a3*a3*u3/(gamma-1.);
    }
    
    else if (s1 > 0) {
      f1 = rho2*u2;
      f2 = rho2*u2*u2 + p2;
      f3 = .5*rho2*u2*u2*u2 + rho2*a2*a2*u2/(gamma-1.);
    }
    
    else {
      f1 = rho1*u1;
      f2 = rho1*u1*u1 + p1;
      f3 = .5*rho1*u1*u1*u1 + rho1*a1*a1*u1/(gamma-1.);
    }
      
  } // end principal else
  
  F[0] = f1;
  F[1] = f2;
  F[2] = f3;
  
}     // Termina void Riemann

