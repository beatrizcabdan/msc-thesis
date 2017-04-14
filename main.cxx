
#include <RandomWorld.hxx>
#include <RandomWorldConfig.hxx>
#include <Exception.hxx>
#include <Config.hxx>
#include <eigen3/Eigen/Dense>

#include <iostream>
#include <cstdlib>

Eigen::IOFormat OctaveFmt(3, 0, ", ", ";\n", "", "", "[", "]");

void printFloatMatrix(std::vector<Eigen::Triplet<float>> matrix, int width, int height, std::string name="M")
{
    SparseMatrixType sparseM(width, height);
    sparseM.setFromTriplets(matrix.begin(), matrix.end());
    std::cout << "\033[1;35m\n" << name << " matrix:" << "\033[0m" << std::endl;
    std::cout << Eigen::MatrixXf(sparseM).format(OctaveFmt) << std::endl;
}

void printIntMatrix(SparseMatrixType matrix, std::string name="M")
{   
    std::cout << "\033[1;35m\n" << name << " matrix:" << "\033[0m" << std::endl;
    std::cout << Eigen::MatrixXf(matrix).format(OctaveFmt) << std::endl;
}

int calculatePtheta(std::vector<SparseMatrixType> states_rolls, std::vector<SparseMatrixType> reward_rolls, bool debug) 
{
    std::cout << "Updating p_theta" << std::endl;
}

int updatePolicy(int pstar, int state) 
{
	std::cout << "Updating policy for state " << state << std::endl;
	return 0;
}

int main(int argc, char *argv[])
{
    int maxIt=2;
    int maxRolls=2;

    try
    {
        if(argc>2)
        {
            throw Engine::Exception("USAGE: randomWalkers [config file]");
        }

        std::string fileName("config.xml");
        if(argc!=1)
        {
            fileName = argv[1];
        }

        Examples::RandomWorldConfig * randomConfig =  new Examples::RandomWorldConfig(fileName);

        randomConfig->loadFile();
        int maxAgents = randomConfig->_numAgents;
        int maxSteps = randomConfig->getNumSteps();
        int numBasis = randomConfig->_numBasis;

        for(int i=0; i<maxIt; i++) 
        {
            std::cout << "\033[1;33m\n" << "ITERATION " << i << "\033[0m" << std::endl;
            
            std::vector<SparseMatrixType> state_rolls;
            std::vector<SparseMatrixType> reward_rolls;

            // Execute tau rollouts
            for(int tau=0; tau<maxRolls; tau++) 
            {
                std::cout << "\033[1;34m\n" << "ROLL " << i << "\033[0m" << std::endl;
                Examples::RandomWorld world(new Examples::RandomWorldConfig(fileName), world.useOpenMPSingleNode());

                world.initialize(argc, argv);
                world.initQ();      //uncontrolled dynamics
                world.initBasis();  //center of RBF
                world.run();
              
                SparseMatrixType states(maxAgents,maxSteps);
                states.setFromTriplets(world._pos_spr_coeff.begin(), world._pos_spr_coeff.end());
                state_rolls.push_back(states);
                
                SparseMatrixType rewards(maxAgents,maxSteps);
                rewards.setFromTriplets(world._rwd_spr_coeff.begin(), world._rwd_spr_coeff.end());
                reward_rolls.push_back(rewards);

                //WARNING: prints up to 3 decimals but _phi contains floats
                /*std::cout << "\033[1;35m\n" << "Phi matrix:" << "\033[0m" << std::endl;
                for(size_t j = 0; j < numBasis*numBasis; j++)
                {
                    for(size_t i = 0; i < world.getCurrentTimeStep(); i++)
                        std::cout << (int)(world._phi[i][j]*1000)/1000.0 << "\t";
                    std::cout << std::endl;
                }
                
                printIntMatrix(states, "State");                
                printIntMatrix(rewards, "Reward");*/
            }

            //compute Z_theta (normalization term)
            //compute p_theta
            //update omega weights for current rollout
            //update Theta_{t+1}
        }
    }
    catch( std::exception & exceptionThrown )
    {
        std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
    }
    return 0;
}
