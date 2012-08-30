/**
 * Copyright (c) 2012, OpenGeoSys Community (http://www.opengeosys.net)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.net/LICENSE.txt
 *
 * @file MeshSearchTest.cpp
 *
 *  Created on  Aug 30, 2012 by Thomas Fischer
 */

// BaseLib
#include "MemWatch.h"
#include "RunTime.h"
#include "tclap/CmdLine.h"

// BaseLib/logog
#include "logog.hpp"

// GeoLib
#include "Grid.h"

// MeshLib
#include "Node.h"
#include "Elements/Element.h"
#include "Mesh.h"
#include "Legacy/MeshIO.h"


void testMeshGridAlgorithm(MeshLib::Mesh const*const mesh,
				std::vector<GeoLib::Point*>& pnts_for_search,
				std::vector<size_t> &idx_found_nodes)
{
	// constructing Grid
	INFO ("[MeshGridAlgorithm] constructing mesh grid object ...");
	clock_t start_grid_construction = clock();
	GeoLib::Grid<MeshLib::Node*> mesh_grid(mesh->getNodes(), 512);
	clock_t end_grid_construction = clock();
	INFO("\tdone, %f seconds", (end_grid_construction-start_grid_construction)/(double)(CLOCKS_PER_SEC));

	const size_t n_pnts_for_search(pnts_for_search.size());
	INFO ("[MeshGridAlgorithm] searching %d points ...", pnts_for_search.size());
	clock_t start = clock();
	for (size_t k(0); k<n_pnts_for_search; k++) {
		MeshLib::Node const* node(mesh_grid.getNearestPoint(pnts_for_search[k]->getCoords()));
		idx_found_nodes.push_back(node->getID());
	}
	clock_t stop = clock();
	INFO("\tdone, %f seconds", (stop-start)/(double)(CLOCKS_PER_SEC));
}

int main(int argc, char *argv[])
{
	LOGOG_INITIALIZE();
	logog::Cout* logogCout = new logog::Cout;

	TCLAP::CmdLine cmd("Simple mesh search test", ' ', "0.1");

	// Define a value argument and add it to the command line.
	// A value arg defines a flag and a type of value that it expects,
	// such as "-m meshfile".
	TCLAP::ValueArg<std::string> mesh_arg("m","mesh","input mesh file",true,"test.msh","string");

	// Add the argument mesh_arg to the CmdLine object. The CmdLine object
	// uses this Arg to parse the command line.
	cmd.add( mesh_arg );

	TCLAP::ValueArg<unsigned> number_arg("n","number-of-test-points","the number of test points",true,10000,"positive number");
	cmd.add( number_arg );

	cmd.parse( argc, argv );

	std::string fname (mesh_arg.getValue());

	FileIO::MeshIO mesh_io;
#ifndef WIN32
	BaseLib::MemWatch mem_watch;
	unsigned long mem_without_mesh (mem_watch.getVirtMemUsage());
#endif
	BaseLib::RunTime run_time;
	run_time.start();
	MeshLib::Mesh* mesh (mesh_io.loadMeshFromFile(fname));
#ifndef WIN32
	unsigned long mem_with_mesh (mem_watch.getVirtMemUsage());
	INFO ("mem for mesh: %i MB", (mem_with_mesh - mem_without_mesh)/(1024*1024));
#endif
	run_time.stop();
	INFO ("time for reading: %f s", run_time.elapsed());

	// *** preparing test data
	std::vector<MeshLib::Node*> const& nodes(mesh->getNodes());
	std::vector<GeoLib::Point*> pnts_for_search;
	unsigned n(number_arg.getValue());
	for (size_t k(0); k<n; k++) {
		pnts_for_search.push_back(new GeoLib::Point(nodes[k]->getCoords()));
	}

	delete mesh;
	delete logogCout;
	LOGOG_SHUTDOWN();
}
