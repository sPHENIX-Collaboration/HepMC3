// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
#ifndef HEPMC_HEPEVT_WRAPPER_H
#define HEPMC_HEPEVT_WRAPPER_H
/**
 *  @file HEPEVT_Wrapper.h
 *  @brief Definition of \b class HEPEVT_Wrapper
 *
 *  @class HepMC::HEPEVT_Wrapper
 *  @brief An interface to HEPEVT common block
 *
 *  @note This header file does not include HEPEVT definition, only declaration.
 *        Including this wrapper requires that HEPEVT is defined somewhere
 *        in the project (most likely as FORTRAN common block).
 *
 *  @note Make sure that HEPEVT definition in project matches this definition
 *        (NMXHEP, double precision, etc.) Change this definition if necessary.
 *
 *  @todo Do we just make write_event and fill_next_event instead?
 */

#ifndef HEPMC_HEPEVT_NMXHEP
/** Default number of particles in the HEPEVT structure */
#define HEPMC_HEPEVT_NMXHEP 10000
#endif

#ifndef HEPMC_HEPEVT_PRECISION
/** Default precision of the 4-momentum, time-space position and mass */
#define HEPMC_HEPEVT_PRECISION double
#endif

/* This definition of HEPEVT corresponds to FORTRAN definition:

      PARAMETER (NMXHEP=10000)
      COMMON /HEPEVT/  NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     &                 JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
      INTEGER          NEVHEP,NHEP,ISTHEP,IDHEP,JMOHEP,JDAHEP
      DOUBLE PRECISION PHEP,VHEP
*/

static const int NMXHEP = HEPMC_HEPEVT_NMXHEP; //!< Number of particles in the HEPEVT structure
typedef HEPMC_HEPEVT_PRECISION momentum_t;     //!< Precision of the 4-momentum, time-space position and mass

/** @struct HEPEVT
 *  @brief  C structure representing Fortran common block HEPEVT
 * T. Sjöstrand et al., "A proposed standard event record",
 *  in `Z physics at LEP 1', eds. G. Altarelli, R. Kleiss and C. Verzegnassi,
 * Geneva, Switzerland, September 4-5, 1989, CERN 89-08 (Geneva, 1989), Vol. 3, p. 327
 * Disk representation is given by Fortran WRITE/READ format.
 */
struct HEPEVT
{
    int        nevhep;             //!< Event number
    int        nhep;               //!< Number of entries in the event
    int        isthep[NMXHEP];     //!< Status code
    int        idhep [NMXHEP];     //!< PDG ID
    int        jmohep[NMXHEP][2];  //!< Pointer to position of 1st and 2nd (or last!) mother
    int        jdahep[NMXHEP][2];  //!< Pointer to position of 1nd and 2nd (or last!) daughter
    momentum_t phep  [NMXHEP][5];  //!< Momentum: px, py, pz, e, m
    momentum_t vhep  [NMXHEP][4];  //!< Time-space position: x, y, z, t
};                               //!< Fortran common block HEPEVT



#include <iostream>
#include <cstdio>
#include <cstring> // memset
#ifndef HEPEVT_WRAPPER_HEADER_ONLY
#include "HepMC/GenEvent.h"
#endif
using std::endl;

namespace HepMC
{
extern struct HEPEVT*  hepevtptr;

class HEPEVT_Wrapper
{

//
// Functions
//
public:
    /** @brief Print information from HEPEVT common block */
    static void print_hepevt( std::ostream& ostr = std::cout );

    /** @brief Print particle information */
    static void print_hepevt_particle( int index, std::ostream& ostr = std::cout );

    /** @brief Check for problems with HEPEVT common block */
    static bool check_hepevt_consistency( std::ostream& ostr = std::cout );

    /** @brief Set all entries in HEPEVT to zero */
    static void zero_everything();
#ifndef HEPEVT_WRAPPER_HEADER_ONLY
    /** @brief Convert GenEvent to HEPEVT*/
    static bool GenEvent_to_HEPEVT( const GenEvent* evt );
    /** @brief Convert HEPEVT to GenEvent*/
    static bool HEPEVT_to_GenEvent( GenEvent* evt );
#endif
    /** @brief Tries to fix list of daughters */
    static bool fix_daughters();
//
// Accessors
//
public:
    static void set_hepevt_address(char *c) { hepevtptr=(struct HEPEVT*)c;          } //!< Set Fortran block address
    static const int max_number_entries()   { return int(NMXHEP);                   } //!< Block size
    static int    event_number()            { return hepevtptr->nevhep;             } //!< Get event number
    static int    number_entries()          { return hepevtptr->nhep;               } //!< Get number of entries
    static int    status( int index )       { return hepevtptr->isthep[index-1];    } //!< Get status code
    static int    id( int index )           { return hepevtptr->idhep[index-1];     } //!< Get PDG particle id
    static int    first_parent( int index ) { return hepevtptr->jmohep[index-1][0]; } //!< Get index of 1st mother
    static int    last_parent( int index )  { return hepevtptr->jmohep[index-1][1]; } //!< Get index of last mother
    static int    first_child( int index )  { return hepevtptr->jdahep[index-1][0]; } //!< Get index of 1st daughter
    static int    last_child( int index )   { return hepevtptr->jdahep[index-1][1]; } //!< Get index of last daughter
    static double px( int index )           { return hepevtptr->phep[index-1][0];   } //!< Get X momentum
    static double py( int index )           { return hepevtptr->phep[index-1][1];   } //!< Get Y momentum
    static double pz( int index )           { return hepevtptr->phep[index-1][2];   } //!< Get Z momentum
    static double e( int index )            { return hepevtptr->phep[index-1][3];   } //!< Get Energy
    static double m( int index )            { return hepevtptr->phep[index-1][4];   } //!< Get generated mass
    static double x( int index )            { return hepevtptr->vhep[index-1][0];   } //!< Get X Production vertex
    static double y( int index )            { return hepevtptr->vhep[index-1][1];   } //!< Get Y Production vertex
    static double z( int index )            { return hepevtptr->vhep[index-1][2];   } //!< Get Z Production vertex
    static double t( int index )            { return hepevtptr->vhep[index-1][3];   } //!< Get production time
    static int    number_parents( int index );                                   //!< Get number of parents
    static int    number_children( int index );                                  //!< Get number of children from the range of daughters
    static int    number_children_exact( int index );                            //!< Get number of children by counting

    static void set_event_number( int evtno )       { hepevtptr->nevhep = evtno;         } //!< Set event number
    static void set_number_entries( int noentries ) { hepevtptr->nhep = noentries;       } //!< Set number of entries
    static void set_status( int index, int status ) { hepevtptr->isthep[index-1] = status; } //!< Set status code
    static void set_id( int index, int id )         { hepevtptr->idhep[index-1] = id;      } //!< Set PDG particle id
    static void set_parents( int index, int firstparent, int lastparent );              //!< Set parents
    static void set_children( int index, int firstchild, int lastchild );               //!< Set children
    static void set_momentum( int index, double px, double py, double pz, double e );   //!< Set 4-momentum
    static void set_mass( int index, double mass );                                     //!< Set mass
    static void set_position( int index, double x, double y, double z, double t );      //!< Set position in time-space
};

//
// inline definitions
//
inline void HEPEVT_Wrapper::print_hepevt( std::ostream& ostr )
{
    ostr << " Event No.: " << hepevtptr->nevhep << endl;
    ostr<< "  Nr   Type   Parent(s)  Daughter(s)      Px       Py       Pz       E    Inv. M." << endl;
    for( int i=1; i<=hepevtptr->nhep; ++i )
        {
            HEPEVT_Wrapper::print_hepevt_particle( i, ostr );
        }
}

inline void HEPEVT_Wrapper::print_hepevt_particle( int index, std::ostream& ostr )
{
    char buf[255];

    sprintf(buf,"%5i %6i",index,hepevtptr->idhep[index-1]);
    ostr << buf;
    sprintf(buf,"%4i - %4i  ",hepevtptr->jmohep[index-1][0],hepevtptr->jmohep[index-1][1]);
    ostr << buf;
    sprintf(buf,"%4i - %4i ",hepevtptr->jdahep[index-1][0],hepevtptr->jdahep[index-1][1]);
    ostr << buf;
    // print the rest of particle info
    sprintf(buf,"%8.2f %8.2f %8.2f %8.2f %8.2f",hepevtptr->phep[index-1][0],hepevtptr->phep[index-1][1],hepevtptr->phep[index-1][2],hepevtptr->phep[index-1][3],hepevtptr->phep[index-1][4]);
    ostr << buf << endl;
}


inline bool HEPEVT_Wrapper::check_hepevt_consistency( std::ostream& ostr )
{
    //!< @todo HEPEVT_Wrapper::check_hepevt_consistency unimplemented!
    printf("HEPEVT_Wrapper::check_hepevt_consistency unimplemented!\n");
    return true;
}

inline void HEPEVT_Wrapper::zero_everything()
{
    memset(hepevtptr,0,sizeof(struct HEPEVT));
}

inline int HEPEVT_Wrapper::number_parents( int index )
{
    return (hepevtptr->jmohep[index-1][0]) ? (hepevtptr->jmohep[index-1][1]) ? hepevtptr->jmohep[index-1][1]-hepevtptr->jmohep[index-1][0] : 1 : 0;
}

inline int HEPEVT_Wrapper::number_children( int index )
{
    return (hepevtptr->jdahep[index-1][0]) ? (hepevtptr->jdahep[index-1][1]) ? hepevtptr->jdahep[index-1][1]-hepevtptr->jdahep[index-1][0] : 1 : 0;
}

inline int HEPEVT_Wrapper::number_children_exact( int index )
{
    int nc=0;
    for( int i=1; i<=hepevtptr->nhep; ++i )
        if (((hepevtptr->jmohep[i-1][0]<=index&&hepevtptr->jmohep[i-1][1]>=index))||(hepevtptr->jmohep[i-1][0]==index)||(hepevtptr->jmohep[i-1][1]==index)) nc++;
    return nc;
}



inline void HEPEVT_Wrapper::set_parents( int index, int firstparent, int lastparent )
{
    hepevtptr->jmohep[index-1][0] = firstparent;
    hepevtptr->jmohep[index-1][1] = lastparent;
}

inline void HEPEVT_Wrapper::set_children( int index, int firstchild, int lastchild )
{
    hepevtptr->jdahep[index-1][0] = firstchild;
    hepevtptr->jdahep[index-1][1] = lastchild;
}

inline void HEPEVT_Wrapper::set_momentum( int index, double px, double py, double pz, double e )
{
    hepevtptr->phep[index-1][0] = px;
    hepevtptr->phep[index-1][1] = py;
    hepevtptr->phep[index-1][2] = pz;
    hepevtptr->phep[index-1][3] = e;
}

inline void HEPEVT_Wrapper::set_mass( int index, double mass )
{
    hepevtptr->phep[index-1][4] = mass;
}

inline void HEPEVT_Wrapper::set_position( int index, double x, double y, double z, double t )
{
    hepevtptr->vhep[index-1][0] = x;
    hepevtptr->vhep[index-1][1] = y;
    hepevtptr->vhep[index-1][2] = z;
    hepevtptr->vhep[index-1][3] = t;
}



inline bool HEPEVT_Wrapper::fix_daughters()
{
    /*AV The function should be called  for a record that has correct particle ordering and mother ids.
    As a result it produces a record with ranges where the daughters can be found.
    Not every particle in the range will be a daughter. It is true only for proper events.
    The return tells if the record was fixed succesfully.
    */

    for( int i=1; i<=HEPEVT_Wrapper::number_entries(); i++ )
        for( int k=1; k<=HEPEVT_Wrapper::number_entries(); k++ ) if (i!=k)
                if ((HEPEVT_Wrapper::first_parent(k)<=i)&&(i<=HEPEVT_Wrapper::last_parent(k)))
                    HEPEVT_Wrapper::set_children(i,(HEPEVT_Wrapper::first_child(i)==0?k:std::min(HEPEVT_Wrapper::first_child(i),k)),(HEPEVT_Wrapper::last_child(i)==0?k:std::max(HEPEVT_Wrapper::last_child(i),k)));
    bool is_fixed=true;
    for( int i=1; i<=HEPEVT_Wrapper::number_entries(); i++ )
        is_fixed=(is_fixed&&(HEPEVT_Wrapper::number_children_exact(i)==HEPEVT_Wrapper::number_children(i)));
    return is_fixed;

    return true;
}







} // namespace HepMC

#endif
