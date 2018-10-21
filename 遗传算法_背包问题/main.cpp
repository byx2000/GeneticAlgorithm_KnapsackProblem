#include <iostream>
#include <cstdlib>
#include <ctime>
#include <Windows.h>

#define OBJ_COUNT 7 //物品数量
#define POPULATION_SIZE 1000 //种群大小
#define MAX_GENERATIONS 1000 //进化代数
#define P_CROSS 0.1 //交叉概率
#define P_MUTATION 0.01 //变异概率

using namespace std;

int capacity = 150; //背包容量
int weight[OBJ_COUNT] = { 35,30,60,50,40,10,25 }; //每件物品的重量
int value[OBJ_COUNT] = { 10,40,30,50,35,40,30 }; //每件物品的价值

//个体的定义
struct Individual
{
	int gene[OBJ_COUNT] = { 0 }; //基因
	int fitness; //个体适应度
	double selectProbability; //选择概率
	double accumulateProbability; //累计概率

	//默认构造函数
	Individual()
	{
		fitness = 0;
		selectProbability = 0.0;
		accumulateProbability = 0.0;
	}
};

void PrintPopulation(Individual* population)
{
	cout << "fitness:" << endl;
	for (int i = 0; i < POPULATION_SIZE; ++i)
	{
		cout << population[i].fitness << " ";
	}
	cout << endl;
	cout << "sp:" << endl;
	for (int i = 0; i < POPULATION_SIZE; ++i)
	{
		cout << population[i].selectProbability << " ";
	}
	cout << endl;
	cout << "ap:" << endl;
	for (int i = 0; i < POPULATION_SIZE; ++i)
	{
		cout << population[i].accumulateProbability << " ";
	}
	cout << endl << endl;
}

//评估所有个体的适应度，并返回所有个体适应度之和
int EvaluateFitness(Individual* population)
{
	int totalFitness = 0;
	for (int i = 0; i < POPULATION_SIZE; ++i)
	{
		int totalWeight = 0;
		population[i].fitness = 0;
		for (int j = 0; j < OBJ_COUNT; ++j)
		{
			if (population[i].gene[j] == 1)
			{
				totalWeight += weight[j];
				population[i].fitness += value[j];
			}
		}
		if (totalWeight > capacity)
		{
			population[i].fitness = 1;
		}
		totalFitness += population[i].fitness;
	}

	return totalFitness;
}

//计算选择概率和累计概率
void CalculateProbability(Individual* population)
{
	double lastAccumulateProbability = 0;
	int totalFitness = EvaluateFitness(population);
	for (int i = 0; i < POPULATION_SIZE; ++i)
	{
		population[i].selectProbability = (double)population[i].fitness / totalFitness;
		population[i].accumulateProbability = lastAccumulateProbability + population[i].selectProbability;
		lastAccumulateProbability = population[i].accumulateProbability;
	}
}

//轮盘赌模拟算法
void Roulette(Individual* population, Individual* newPopulation, Individual best)
{
	for (int i = 0; i < POPULATION_SIZE; ++i)
	{
		double p = (double)rand() / (RAND_MAX + 1);
		if (p < population[0].accumulateProbability)
		{
			newPopulation[i] = best;
			//newPopulation[i] = population[0];
		}
		else
		{
			for (int j = 0; j < POPULATION_SIZE - 1; ++j)
			{
				if (p >= population[j].accumulateProbability && p < population[j + 1].accumulateProbability)
				{
					newPopulation[i] = population[j + 1];
				}
			}
		}
	}
}

//对两个个体进行随机次数的基因位交换
void ExchangeOver(Individual* population, int first, int second)
{
	int cnt = rand() % OBJ_COUNT + 1; //交换次数
	for (int i = 0; i < cnt; ++i)
	{
		int index = rand() % OBJ_COUNT; //交换的基因位
		int t = population[first].gene[index];
		population[first].gene[index] = population[second].gene[index];
		population[second].gene[index] = t;
	}
}

Individual GetBestPopulation(Individual* population)
{
	Individual best;
	for (int i = 0; i < POPULATION_SIZE; ++i)
	{
		if (population[i].fitness > best.fitness)
		{
			best = population[i];
		}
	}
	return best;
}

//选择
void Select(Individual* population)
{
	Individual newPopulation[POPULATION_SIZE];
	Individual best = GetBestPopulation(population);

	CalculateProbability(population);
	Roulette(population, newPopulation, best);

	for (int i = 0; i < POPULATION_SIZE; ++i)
	{
		population[i] = (newPopulation[i].fitness == 1) ? best : newPopulation[i];
	}
}

//以交换概率随机指定交换的两个个体并进行交换
void Cross(Individual* population)
{
	int first = -1;
	for (int i = 0; i < POPULATION_SIZE; ++i)
	{
		double p = (double)rand() / (RAND_MAX + 1);
		if (p < P_CROSS)
		{
			if (first < 0)
			{
				first = i;
			}
			else
			{
				ExchangeOver(population, first, i); //对两个个体进行交换
				first = -1;
			}
		}
	}
}

//对随机个数的基因位进行变异
void ReverseGene(Individual* population, int index)
{
	int cnt = rand() % OBJ_COUNT + 1;
	for (int i = 0; i < cnt; ++i)
	{
		int index = rand() % OBJ_COUNT;
		population[index].gene[index] = 1 - population[index].gene[index];
	}
}

//以变异概率选择变异个体并进行变异
void Mutation(Individual* population)
{
	for (int i = 0; i < POPULATION_SIZE; ++i)
	{
		double p = (double)rand() / (RAND_MAX + 1);
		if (p < P_MUTATION)
		{
			ReverseGene(population, i); //对选中个体进行变异
		}
	}
}

//初始化种群
void Initialize(Individual* population)
{
	for (int i = 0; i < POPULATION_SIZE; ++i)
	{
		for (int j = 0; j < OBJ_COUNT; ++j)
		{
			population[i].gene[j] = rand() % 2;
		}
	}
}

int test()
{
	Individual population[POPULATION_SIZE]; //种群

	Initialize(population);
	for (int i = 0; i < MAX_GENERATIONS; ++i)
	{
		Select(population);
		Cross(population);
		Mutation(population);
	}

	Individual best = GetBestPopulation(population);
	return best.fitness;
}

int main()
{
	srand((unsigned)time(NULL)); //初始化随机数种子

	Individual population[POPULATION_SIZE]; //种群

	Initialize(population); //初始化种群
	for (int i = 0; i < MAX_GENERATIONS; ++i)
	{
		Select(population); //选择
		Cross(population); //交换
		Mutation(population); //变异
	}

	Individual best; //最好个体
	best = GetBestPopulation(population);
	cout << best.fitness << endl;

	/*int cnt = 0;
	for (int i = 0; i < 500; ++i)
	{
		if (test() == 170)
		{
			cnt++;
		}
	}

	cout << cnt << endl;*/

	system("pause");
	return 0;
}