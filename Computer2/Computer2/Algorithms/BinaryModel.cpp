#include "BinaryModel.h"
#include "svm.h"
#include <time.h>

BinaryModel::BinaryModel(int numInputs)
	: Model(numInputs)
{

}

BinaryModel::~BinaryModel()
{
	svm_free_and_destroy_model(&this->_model);
}

int BinaryModel::Train(std::vector<unsigned int**> data, std::vector<double> output, void* info)
{
	int i, j, k, n;
	BinaryModelParams bmParams;
	struct svm_parameter param;
	struct svm_problem prob;

	if(data.size() != output.size())
		return R_DATA_LENGTHS_NOT_MATCHING;

	LOG_DEBUG("Length of training data: %d", data.size());

	//set default param values
	param.svm_type = C_SVC;
	param.kernel_type = LINEAR;
	param.degree = 3;
	param.gamma = 0.1;	
	param.coef0 = 0;
	
	param.nu = 0.5;
	param.cache_size = 100;
	param.C = 1;
	param.eps = 1e-3;
	param.p = 0.1;
	param.shrinking = 1;
	param.probability = 1;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;

	//parse the provided info and set weight values accordingly
	if(info != NULL)
	{
		bmParams = *((BinaryModelParams *) info);
		LOG_DEBUG("Binary model using provided params");
		return 10;	//fail - need to set params in here
	}
	else
	{
		LOG_DEBUG("Binary model using default params");
	}

	//set up the svm problem
	prob.l = output.size();
	prob.x = (struct svm_node**) malloc(prob.l * sizeof(struct svm_node*));
	prob.y = (double*) malloc(prob.l * sizeof(double));

	for(i=0;i<prob.l;i++)
	{
		prob.y[i] = output[i];
		prob.x[i] = (struct svm_node*) malloc(this->_numInputs * DATA_LENGTH * sizeof(struct svm_node));
		for(j=0;j<this->_numInputs;j++)
		{
			for(k=0;k<DATA_LENGTH;k++)
			{
				n = j*DATA_LENGTH + k;
				prob.x[i][n].index = n;
				prob.x[i][n].value = (double) (data[i][j][k]);
				//prob.x[i][n].value = (double) output[i];
			}
		}
		prob.x[i][this->_numInputs*DATA_LENGTH-1].index = -1;
	}


	/*
	param.nr_weight = 2;
	param.weight_label = (int*) malloc(2*sizeof(int));
	param.weight = (double*) malloc(2*sizeof(double));
	param.weight_label[0] = 0;
	param.weight[0] = 0.9;
	param.weight_label[1] = 1;
	param.weight[1] = 0.1;
	*/
	
#ifdef _DEBUG 
	//this->EvalCrossValidation(&prob, &param);
#endif
	

	LOG_DEBUG("Before svm_train");
	time_t start = time(NULL);
	this->_model = svm_train(&prob, &param);
	
	
	LOG_DEBUG("After svm train, training time: %d seconds", (time(NULL) - start));

	
	svm_destroy_param(&param);


	return R_SUCCESS;
}

double BinaryModel::Predict(unsigned int** data)
{
	int i, k, n;
	double result;
	struct svm_node* node = (struct svm_node*) malloc(this->_numInputs * DATA_LENGTH * sizeof(struct svm_node));
	
	for(i=0;i<this->_numInputs;i++)
	{
		for(k=0;k<DATA_LENGTH;k++)
		{
			n = i*DATA_LENGTH + k;
			node[n].index = n;
			node[n].value = data[i][k];
		}
	}
	double* probs = (double*)malloc(2 * sizeof(double));
	result = svm_predict_probability(this->_model, node, probs);
	result = probs[1];
	free(probs);
	//result = svm_predict(this->_model, node);
	return result;
}

void BinaryModel::EvalCrossValidation(struct svm_problem* prob, struct svm_parameter* param)
{
	int nFolds = 10;
	LOG_DEBUG("Starting %d - fold cross validation", nFolds);
	double* target = (double*) malloc(prob->l * sizeof(double));
	
	//do the cross validation
	svm_cross_validation(prob, param, nFolds, target);

	//build a confusion matrix
	double confusionMatrix[2][2] = {{0, 0}, {0, 0}};
	int i;
	for(i=0;i<prob->l;i++)
		confusionMatrix[(int)prob->y[i]][(int)target[i]]++;

	//log matrix and overall accuracy
	double accuracy = (confusionMatrix[0][0] + confusionMatrix[1][1]) / ((double)prob->l);
	LOG_DEBUG("Overall accuracy: %f%%", 100.0 * accuracy);

	LOG_DEBUG(	"\t\t\t\t\tPredicted class");
	LOG_DEBUG(	"\t\t\t\t\t0\t1");
	LOG_DEBUG(	"Actual\t\t0\t\t%d\t%d", (int)confusionMatrix[0][0], (int)confusionMatrix[0][1]);
	LOG_DEBUG(	"\t\t\t1\t\t%d\t%d", (int)confusionMatrix[1][0], (int)confusionMatrix[1][1]);

	LOG_DEBUG("Done cross validation");

	free(target);
}