#include <iostream>
#include <cstdlib>
#include <ctime>
#include <Windows.h>

#define OBJ_COUNT 7 //��Ʒ����
#define POPULATION_SIZE 1000 //��Ⱥ��С
#define MAX_GENERATIONS 1000 //��������
#define P_CROSS 0.1 //�������
#define P_MUTATION 0.01 //�������

using namespace std;

int capacity = 150; //��������
int weight[OBJ_COUNT] = { 35,30,60,50,40,10,25 }; //ÿ����Ʒ������
int value[OBJ_COUNT] = { 10,40,30,50,35,40,30 }; //ÿ����Ʒ�ļ�ֵ

//����Ķ���
struct Individual
{
	int gene[OBJ_COUNT] = { 0 }; //����
	int fitness; //������Ӧ��
	double selectProbability; //ѡ�����
	double accumulateProbability; //�ۼƸ���

	//Ĭ�Ϲ��캯��
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

//�������и������Ӧ�ȣ����������и�����Ӧ��֮��
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

//����ѡ����ʺ��ۼƸ���
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

//���̶�ģ���㷨
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

//���������������������Ļ���λ����
void ExchangeOver(Individual* population, int first, int second)
{
	int cnt = rand() % OBJ_COUNT + 1; //��������
	for (int i = 0; i < cnt; ++i)
	{
		int index = rand() % OBJ_COUNT; //�����Ļ���λ
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

//ѡ��
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

//�Խ����������ָ���������������岢���н���
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
				ExchangeOver(population, first, i); //������������н���
				first = -1;
			}
		}
	}
}

//����������Ļ���λ���б���
void ReverseGene(Individual* population, int index)
{
	int cnt = rand() % OBJ_COUNT + 1;
	for (int i = 0; i < cnt; ++i)
	{
		int index = rand() % OBJ_COUNT;
		population[index].gene[index] = 1 - population[index].gene[index];
	}
}

//�Ա������ѡ�������岢���б���
void Mutation(Individual* population)
{
	for (int i = 0; i < POPULATION_SIZE; ++i)
	{
		double p = (double)rand() / (RAND_MAX + 1);
		if (p < P_MUTATION)
		{
			ReverseGene(population, i); //��ѡ�и�����б���
		}
	}
}

//��ʼ����Ⱥ
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
	Individual population[POPULATION_SIZE]; //��Ⱥ

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
	srand((unsigned)time(NULL)); //��ʼ�����������

	Individual population[POPULATION_SIZE]; //��Ⱥ

	Initialize(population); //��ʼ����Ⱥ
	for (int i = 0; i < MAX_GENERATIONS; ++i)
	{
		Select(population); //ѡ��
		Cross(population); //����
		Mutation(population); //����
	}

	Individual best; //��ø���
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