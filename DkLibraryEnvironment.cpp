#include <DkLibraryEnvironment.hpp>
#include <MyJEGAOptimizer.hpp>
#include <IoData.hpp>
#include <cassert>

void
DkLibraryEnvironment::reset_top_iterator()
{
	// get the model attached to top method
	Dakota::Model &modelWithTopMethod = this->topLevelIterator.iterated_model();
	// get the communicator attached to environment
	Dakota::ParLevLIter w_pl_iter = parallelLib.w_parallel_level_iterator();
	// create our iterator
	std::shared_ptr<Dakota::Iterator> iter_rep = 
		std::make_shared<MyJEGAOptimizer>(this->probDescDB, modelWithTopMethod);
	// assign to top method
	this->topLevelIterator.assign_rep(iter_rep);	
	this->topLevelIterator.init_communicators(w_pl_iter);
}

// This function is passed to LibraryEnvironment after it creates data containers based
// on parsed input. In our case it should create default containers. We use set methods
// to then update the containers with valued parsed in our input format.
static void setup_data_containers(Dakota::ProblemDescDB* pdb, void* iod_ptr)
{

	IoData &iod = *(IoData*)iod_ptr;

		

}
