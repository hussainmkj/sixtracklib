//SixTrackLib
//
//Authors: R. De Maria, G. Iadarola, D. Pellegrini, H. Jasim
//
//Copyright 2017 CERN. This software is distributed under the terms of the GNU
//Lesser General Public License version 2.1, copied verbatim in the file
//`COPYING''.
//
//In applying this licence, CERN does not waive the privileges and immunities
//granted to it by virtue of its status as an Intergovernmental Organization or
//submit itself to any jurisdiction.


/* === Element Records ===
 * Elements are specified as follows:
 * Element(
 *     [name],
 *     (
 *         [fields]
 *     ),
 *     ({
 *         [track function body]
 *     })
 * )
 * 
 * [name] is the element's identification label;
 * for each element, there will be created:
 *     - an associated structure ST_[name]
 *     - an enum entry [name]ID in an enum type_t of all element labels
 *     - a track dispatch function [name]_track
 *     - a dispatch call in Block_track
 * 
 * [fields] is the set of entries in the element's struct ST_[name], corresponding to elements in the data buffer.
 * Individual elements are separated by semicolons, and can either be:
 *     - _([var_type], [var_label]);
 *     _M([var_type], [var_label]);
 * _(t, l) and _M(t, l) are macros that expand into variable declarations during struct construction,
 * and control initialization behaviour during function construction (see below).
 * 
 * [track function body] is a statement block (enclosed by block delimiters {}) that is used to create the element's track function, declared as:
 *   int [name]_track(CLGLOBAL Particle *p, CLGLOBAL ST_[name] *el);
 * It must be enclosed in parentheses to prevent commas from being parsed as additional macro arguments.
 * Any element fields using the _(t, l) declaration will be initialized at the beginning of the function to the corresponding value in the passed-in struct, and can be accessed using their label.
 * Fields declared with _M(t, l) will have to be manually initialized.
 *
*/


Element(
    Drift,
    (
        _(double, length);
    ),
    ({
        double xp, yp;
        xp = p->px * p->rpp;
        yp = p->py * p->rpp;
        p->x += xp * length;
        p->y += yp * length;
        p->sigma += length * (1 - p->rvv*( 1 + (xp*xp+yp*yp)/2 ) );
        p->s+=length;
        return 1;
    })
)

Element(
    DriftExact,
    (
        _(double, length);
    ),
    ({
        double lpzi, lbzi, px, py, opd;
        opd=1+p->delta;
        px=p->px; py=p->py;
        lpzi= length/sqrt(opd*opd-px*px-py*py);
        lbzi=(p->beta0*p->beta0*p->psigma+1)*lpzi;
        p->x += px*lpzi ;
        p->y += py*lpzi ;
        p->sigma += length - lbzi;
        p->s += length ;
        return 1;
    })
)

Element(
    Multipole,
    (
        _(long int, order);
        _(double, l);
        _(double, hxl);
        _(double, hyl);
        _M(double, bal[1]);
    ),
    ({
        double x,y,chi,dpx,dpy,zre,zim,b1l,a1l,hxx,hyy;
        CLGLOBAL double *bal = el->bal;
        dpx=bal[order*2];
        dpy=bal[order*2+1];
        x=p->x; y=p->y; chi=p->chi;
        for(int ii=order-1;ii>=0;ii--) {
            zre=(dpx*x-dpy*y);
            zim=(dpx*y+dpy*x);
            dpx=bal[ii*2]+zre;
            dpy=bal[ii*2+1]+zim;
        }
        dpx=-chi*dpx ;
        dpy=chi*dpy ;
        if(l>0) {
            b1l=chi*bal[0]; a1l=chi*bal[1];
            hxx=hxl/l*x; hyy=hyl/l*y;
            dpx+=hxl + hxl*p->delta - b1l*hxx;
            dpy-=hyl + hyl*p->delta - a1l*hyy;
            p->sigma-=chi*(hxx-hyy)*l*p->rvv;
        }
        p->px+=dpx ;  p->py+=dpy ;
        return 1 ;
    })
)

Element(
    Cavity,
    (
        _(double, volt);
        _(double, freq);
        _(double, lag);
    ),
    ({
        double phase, pt, opd;
        phase=lag-2*M_PI/CLIGHT*freq*p->sigma/p->beta0;
        //printf("ggg00 %e %e\n",p->psigma,p->psigma+p->chi*volt/(p->p0c));
        p->psigma+=p->chi*volt*sin(phase)/(p->p0c*p->beta0);
        pt=p->psigma * p->beta0;
        opd=sqrt( pt*pt+ 2*p->psigma + 1 );
        p->delta=opd - 1;
        p->beta=opd/(1/p->beta0+pt);
        //p->gamma=1/sqrt(1-p->beta*p->beta);
        p->gamma=(pt*p->beta0+1)*p->gamma0;
        p->rpp=1/opd;
        p->rvv=p->beta0/p->beta;
        //printf("ggg2 %e %e %e\n",pt,opd,p->delta);
        return 1;
    })
)

Element(
    Align,
    (
        _(double, cz);
        _(double, sz);
        _(double, dx);
        _(double, dy);
    ),
    ({
        double xn,yn;
        xn= cz*p->x-sz*p->y - dx;
        yn= sz*p->x+cz*p->y - dy;
        p->x=xn;
        p->y=yn;
        xn= cz*p->px+sz*p->py;
        yn=-sz*p->px+cz*p->py;
        p->px=xn;
        p->py=yn;
        return 1;
    })
)
