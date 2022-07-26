/*=============================================================================================
 * CLASSIFICATION: Unclassified
 * Government Purpose Rights
 * Contract No: 12-C-0158
 * Contractor Name: Perspecta
 * Contractor Address: 15052 Conference Center Dr. Chantilly, VA 20151
 * Expiration Date: 31 Aug. 2023
 * The Government's rights to use, modify, reproduce, release, perform, display, or disclose these
 * technical data and computer software are restricted by paragraph (b)(1) of clause N52.227-002,
 * Rights in Technical Data and Computer Software: Noncommercial Items, contained in the contract
 * identified above. No restrictions apply after the expiration date shown above. Any reproduction of
 * technical data or computer software, or portions thereof marked with this legend,must also reproduce the markings.
 *
 *
 * EigenValueVector.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : EigenValueVector.cpp

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : EigenValueVector

//  AUTHOR(s) : R. Harrison

//  UPDATES   : V4.1-001  06/01/2006   RMH  Initial Release

//  NOTES :		Finds and sorts the eigen values & vectors of symmentric matrix

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Contents derived from SE and SST Ver 1.0.

//****************************************************************************//
#include "EigenValueVector.h"

namespace A3
{

//--------------------------------------------------------------------------
EigenValueVector::~EigenValueVector()
{;}
//--------------------------------------------------------------------------
void EigenValueVector::tred2(double** a, int n, double* d, double*e)
{
    register int l,k,j,i;
    double g;
    double f;

    for(i = n - 1; i > 0; i--) {
        double scale;
        double h;
        l = i-1;
        h = scale = 0.0;

        if(l > 0) {
            for(k = 0; k <= l; k++) {
                scale += fabs(a[i][k]);
            }

            if(scale == 0.0) {             // skip transformation
                e[i] = a[i][l];
            } else {
                double hh;

                for(k = 0; k <= l; k++) {
                    a[i][k] /= scale;          // used scaled a's for transformation
                    h += a[i][k]*a[i][k];
                }

                f = a[i][l];
                g = (f >= 0.0 ? -sqrt(h) : sqrt(h));
                e[i]= scale*g;
                h -= f * g;
                a[i][l] = f - g;
                f = 0.0;

                for(j = 0; j <= l; j++) {
                    a[j][i] = a[i][j]/h;       // can be omitted if eigenvector not wanted
                    g = 0.0;

                    for(k = 0; k <= j; k++) {
                        g += a[j][k]*a[i][k];
                    }

                    for(k = j+1; k <= l; k++) {
                        g += a[k][j]*a[i][k];
                    }

                    e[j]=g/h;
                    f += e[j]*a[i][j];
                }

                hh=f/(h+h);

                for(j = 0; j <= l; j++) {
                    f = a[i][j];
                    e[j]=g=e[j]-hh*f;

                    for(k = 0; k <= j; k++) {
                        a[j][k] -= (f*e[k]+g*a[i][k]);
                    }
                }
            }  // end k-loop
        }  // end if-loop for l > 1
        else {
            e[i]=a[i][l];
        }

        d[i]=h;
    }  // end i-loop

    d[0]  = 0.0;
    e[0]  = 0.0;

    //   Contents of this loop can be omitted if eigenvectors not
    // wanted except for statement d[i]=a[i][i];
    for(i = 0; i < n; i++) {
        l = i-1;

        if(d[i]) {
            for(j = 0; j <= l; j++) {
                g= 0.0;

                for(k = 0; k <= l; k++) {
                    g += a[i][k] * a[k][j];
                }

                for (k = 0; k <= l; k++) {
                    a[k][j] -= g * a[k][i];
                }
            }
        }

        d[i] = a[i][i];
        a[i][i] = 1.0;

        for(j = 0; j <= l; j++)  {
            a[j][i]=a[i][j] = 0.0;
        }
    }
} // End: function tred2(), (C) Copr. 1986-92 Numerical Recipes Software )

//-------------------------------------------------------------------------
void EigenValueVector::tqli(double* d, double* e, int n, double** z)
{
    register int m;
    register int l;
    register int i;
    register int k;
    double         s,r,p,g,f,dd,c,b;

    for(i = 1; i < n; i++) {
        e[i-1] = e[i];
    }

    e[n-1] = 0.0;

    for(l = 0; l < n; l++) {
        register int iter;
        iter = 0;

        do {
            for(m = l; m < n-1; m++) {
                dd = fabs(d[m]) + fabs(d[m+1]);

                if((double)(fabs(e[m])+dd) == dd) {
                    break;
                }
            }

            if(m != l) {
//            if(iter++ == 30) // NEED TO UPDATE ERROR CHECK!!!
//
//                  {
//
//                     std::cout<<"\n\nToo many iterations in tqli.\n";
//
//               exit(1);
//
//            }
                g = (d[l+1] - d[l])/(2.0 * e[l]);
                r = pythag(g,1.0);
                g = d[m]-d[l]+e[l]/(g+sign(r,g));
                s = c = 1.0;
                p = 0.0;

                for(i = m-1; i >= l; i--) {
                    f = s * e[i];
                    b = c*e[i];
                    e[i+1] = (r=pythag(f,g));

                    if(r == 0.0) {
                        d[i+1]-= p;
                        e[m]= 0.0;
                        break;
                    }

                    s = f/r;
                    c = g/r;
                    g = d[i+1] - p;
                    r = (d[i] - g) * s + 2.0 * c * b;
                    d[i+1] = g + (p = s * r);
                    g = c * r - b;

                    for(k = 0; k < n; k++) {
                        f = z[k][i+1];
                        z[k][i+1] = s * z[k][i] + c * f;
                        z[k][i]   = c * z[k][i] - s * f;
                    } // end k-loop
                } // end i-loop */

                if(r == 0.0 && i >= l) {
                    continue;
                }

                d[l] -= p;
                e[l] = g;
                e[m]  = 0.0;
            } // end if-loop for m != 1
        } while(m != l);
    } // end l-loop
} // End: function tqli(), (C) Copr. 1986-92 Numerical Recipes Software )%.

//--------------------------------------------------------------------------
void EigenValueVector::eigsrt(double* d, double** v, int n)
{
    int j;
    int i;

    for (i=0; i<n-1; i++) {
        int k;
        double p;
        p=d[k=i];

        for (j=i+1; j<n; j++)
            if (d[j] >= p) {
                p=d[k=j];
            }

        if (k != i) {
            d[k]=d[i];
            d[i]=p;

            for (j=0; j<n; j++) {
                p=v[j][i];
                v[j][i]=v[j][k];
                v[j][k]=p;
            }
        }
    }
}

//--------------------------------------------------------------------------
} // End namespace







