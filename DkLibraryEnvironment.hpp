#include <ParallelLibrary.hpp>
#include <ProblemDescDB.hpp>
#include <LibraryEnvironment.hpp>
#include <PluginParallelDirectApplicInterface.hpp> 

#ifndef DAKOTA_HAVE_MPI
#define MPI_COMM_WORLD 0
#endif // not DAKOTA_HAVE_MPI

#include <IoData.hpp>

class DkLibraryEnvironment : public Dakota::LibraryEnvironment
{
public:
	DkLibraryEnvironment(MPI_Comm dakota_mpi_comm,
                       Dakota::ProgramOptions prog_opts = Dakota::ProgramOptions(),
                       bool check_bcast_construct = true,
                       Dakota::DbCallbackFunctionPtr callback = NULL,
                       void* callback_data = NULL)
											: LibraryEnvironment(dakota_mpi_comm, 
																					 prog_opts, check_bcast_construct, NULL, NULL)
											{ };
  //LibraryEnvironment destructor handles deleting pointers
	~DkLibraryEnvironment() { };

	void update_db(const IoData &iod);
};
