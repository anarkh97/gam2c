#include <DkLibraryEnvironment.hpp>

#ifndef DAKOTA_HAVE_MPI
#define MPI_COMM_WORLD 0
#endif // not DAKOTA_HAVE_MPI

#include <version.h>
#include <MyJEGAOptimizer.hpp>
#include <IoData.hpp>
#include <cassert>

int main(int argc, char* argv[])
{

	// whether dakota running in parallel (e.g. mpiexec dakota -i dakota.in)
	bool parallel = Dakota::MPIManager::detect_parallel_launch(argc, argv);
	
	// Define MPI_DEBUG in dakota_global_defs.cpp to cause a hold here
	Dakota::mpi_debug_hold();

#ifdef DAKOTA_HAVE_MPI
	//AN: Stop execution in case someone tries mpiexec gam2c.
	if (parallel) {
		//MPI_Init(&argc, &argv); // initialize MPI
		Cerr << "***Error: GAM2C is not configured to run with"
				 << " dakota's coarse grained parallelism." << std::endl;
		Dakota::abort_handler(Dakota::OTHER_ERROR);
	}
#endif // DAKOTA_HAVE_MPI

	// No dakota input file set --> no parsing.  
	// We parse our own inputs and create respective dakota Data[] objects
	// Could set other command line options such as restart in opts:
	Dakota::ProgramOptions opts;
	
	// delay validation/sync of the Dakota database and iterator
	// construction to allow update after all data is populated
	bool check_bcast_construct = false;

	// Parse inputs from out input file.
	IoData iod(argc, argv);

	// builds default methods and set values from our inputs.
	DkLibraryEnvironment env(MPI_COMM_WORLD, opts, check_bcast_construct);

	// setup dakota with our inputs
	env.update_db(iod);

	// dakota exit mode
	env.exit_mode("throw");

	// execute dakota
	env.execute();

/*
  // Note: Dakota objects created in above function calls need to go
  // out of scope prior to MPI_Finalize so that MPI code in
  // destructors works properly in library mode.
#ifdef DAKOTA_HAVE_MPI
	//if (parallel)
		//MPI_Finalize(); // finalize MPI
#endif // DAKOTA_HAVE_MPI
*/
	return 0;
}
