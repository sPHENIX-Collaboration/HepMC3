// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
/// @brief Basic example of building HepMC3 tree by hand
///
///  Based on HepMC2/examples/example_BuildEventFromScratch.cc

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "HepMC/Search/FindParticles.h"
#include "HepMC/Print.h"

// Useful definition of a FOREACH loop that either uses BOOST_FOREACH
// or c++11 'foreach' loop depending on the HepMC installation options
#include "HepMC/foreach.h"

#include <iostream>
using namespace HepMC;
using std::cout;
using std::endl;

/** Main program */
int main() {
    //
    // In this example we will place the following event into HepMC "by hand"
    //
    //     name status pdg_id  parent Px       Py    Pz       Energy      Mass
    //  1  !p+!    3   2212    0,0    0.000    0.000 7000.000 7000.000    0.938
    //  2  !p+!    3   2212    0,0    0.000    0.000-7000.000 7000.000    0.938
    //=========================================================================
    //  3  !d!     3      1    1,1    0.750   -1.569   32.191   32.238    0.000
    //  4  !u~!    3     -2    2,2   -3.047  -19.000  -54.629   57.920    0.000
    //  5  !W-!    3    -24    1,2    1.517   -20.68  -20.605   85.925   80.799
    //  6  !gamma! 1     22    1,2   -3.813    0.113   -1.833    4.233    0.000
    //  7  !d!     1      1    5,5   -2.445   28.816    6.082   29.552    0.010
    //  8  !u~!    1     -2    5,5    3.962  -49.498  -26.687   56.373    0.006

    // now we build the graph, which will looks like
    //                       p7                         #
    // p1                   /                           #
    //   \v1__p3      p5---v4                           #
    //         \_v3_/       \                           #
    //         /    \        p8                         #
    //    v2__p4     \                                  #
    //   /            p6                                #
    // p2                                               #
    //                                                  #
    GenEvent evt(Units::GEV,Units::MM);

    //                                                               px      py        pz       e     pdgid status
    GenParticlePtr p1 = make_shared<HepMC::GenParticle>( FourVector( 0.0,    0.0,   7000.0,  7000.0  ),2212,  3 );
    GenParticlePtr p2 = make_shared<HepMC::GenParticle>( FourVector( 0.0,    0.0,  -7000.0,  7000.0  ),2212,  3 );
    GenParticlePtr p3 = make_shared<HepMC::GenParticle>( FourVector( 0.750, -1.569,   32.191,  32.238),   1,  3 );
    GenParticlePtr p4 = make_shared<HepMC::GenParticle>( FourVector(-3.047,-19.0,    -54.629,  57.920),  -2,  3 );

    GenVertexPtr v1 = make_shared<HepMC::GenVertex>();
    v1->add_particle_in (p1);
    v1->add_particle_out(p3);
    evt.add_vertex(v1);

    GenVertexPtr v2 = make_shared<HepMC::GenVertex>();
    v2->add_particle_in (p2);
    v2->add_particle_out(p4);
    evt.add_vertex(v2);
    evt.set_beam_particles(p1,p2); // declare that p1 and p2 are beam particles

    GenVertexPtr v3 = make_shared<HepMC::GenVertex>();
    v3->add_particle_in(p3);
    v3->add_particle_in(p4);
    evt.add_vertex(v3);

    GenParticlePtr p5 = make_shared<HepMC::GenParticle>( FourVector(-3.813,  0.113, -1.833, 4.233),  22, 1 );
    GenParticlePtr p6 = make_shared<HepMC::GenParticle>( FourVector( 1.517,-20.68, -20.605,85.925), -24, 3 );

    v3->add_particle_out(p5);
    v3->add_particle_out(p6);

    GenVertexPtr v4 = make_shared<HepMC::GenVertex>();
    v4->add_particle_in (p6);
    evt.add_vertex(v4);

    GenParticlePtr p7 = make_shared<HepMC::GenParticle>( FourVector(-2.445, 28.816,  6.082,29.552),  1, 1 );
    GenParticlePtr p8 = make_shared<HepMC::GenParticle>( FourVector( 3.962,-49.498,-26.687,56.373), -2, 1 );

    v4->add_particle_out(p7);
    v4->add_particle_out(p8);

    //
    // Example of use of the search engine
    //

    // 1)
    cout << endl << "Find all stable particles: " << endl;

    FindParticles search(evt, FIND_ALL, STATUS == 1 && STATUS_SUBCODE == 0);

    FOREACH( const GenParticlePtr &p, search.results() ) {
        Print::line(p);
    }

    // 3)
    cout << endl << "Find all ancestors of particle with id " << p5->id() << ": " << endl;

    FindParticles search2(p5, FIND_ALL_ANCESTORS);

    FOREACH( const GenParticlePtr &p, search2.results() ) {
        Print::line(p);
    }

    // 3)
    cout << endl << "Find stable descendants of particle with id " << p4->id() << ": " << endl;
    cout<<"(we check both for status == 1 and no end vertex just to be safe)" << endl;

    FindParticles search3(p4, FIND_ALL_DESCENDANTS, STATUS == 1 && !HAS_END_VERTEX);

    FOREACH( const GenParticlePtr &p, search3.results() ) {
        Print::line(p);
    }

    // 3b)
    cout << endl << "Narrow down results of previous search to quarks only: " << endl;

    search3.narrow_down( PDG_ID >= -6 && PDG_ID <= 6 );

    FOREACH( const GenParticlePtr &p, search3.results() ) {
        Print::line(p);
    }

    //
    // Example of adding event attributes
    //
    shared_ptr<GenPdfInfo> pdf_info = make_shared<GenPdfInfo>();
    evt.add_attribute("GenPdfInfo",pdf_info);

    pdf_info->set(1,2,3.4,5.6,7.8,9.0,1.2,3,4);

    shared_ptr<GenHeavyIon> heavy_ion = make_shared<GenHeavyIon>();
    evt.add_attribute("GenHeavyIon",heavy_ion);

    heavy_ion->set( 1,2,3,4,5,6,7,8,9,0.1,2.3,4.5,6.7);

    shared_ptr<GenCrossSection> cross_section = make_shared<GenCrossSection>();
    evt.add_attribute("GenCrossSection",cross_section);

    cross_section->set_cross_section(1.2,3.4);

    //
    // Example of manipulating the attributes
    //

    cout << endl << " Manipulating attributes:" << endl;

    // get attribute
    shared_ptr<GenCrossSection> cs = evt.attribute<GenCrossSection>("GenCrossSection");

    // if attribute exists - do something with it
    if(cs) {
        cs->set_cross_section(-1.0,0.0);
        Print::line(cs);
    }
    else cout << "Problem accessing attribute!" << endl;

    // remove attribute
    evt.remove_attribute("GenCrossSection");
    evt.remove_attribute("GenCrossSection");

    // now this should be null
    cs = evt.attribute<GenCrossSection>("GenCrossSection");

    if(!cs) cout << "Successfully removed attribute" << endl;
    else    cout << "Problem removing attribute!" << endl;

    Print::listing(evt);

    cout << endl << "Printing full content of the GenEvent object " << endl
                 << "(including particles and vertices in one-line format):" << endl << endl;

    Print::content(evt);

    cout << endl << "Now: removing particle with id 6 and printing again:" << endl << endl;
    evt.remove_particle(p6);

    Print::listing(evt);
    Print::content(evt);

    return 0;
}
