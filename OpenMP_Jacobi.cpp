#include <stdio.h>
#include <omp.h>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <vector>
#include <time.h>
#include <windows.h>
using namespace std;
#define size 4
#define NUM_THREADS 4
#define episinon DBL_MIN

int main()
{
	vector<vector<int> > A(size, vector<int>(size));
	vector<int> b(size);
	vector<double> x(size);
	vector<double> old_x(size, 0);
	vector<double> delta(size);
	int avg = NUM_THREADS / size;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			cin >> A[i][j];
		}
		cin >> b[i];
	}
	omp_set_num_threads(NUM_THREADS);
	//这部分看为初始化的操作，没有计入运行时间
	#pragma omp parallel
	{
		int tid;
		tid = omp_get_thread_num();
		x[tid] = b[tid] / A[tid][tid];
		delta[tid] = abs(x[tid]);
	}
	double run_time;
	_LARGE_INTEGER time_start;	//开始时间
	_LARGE_INTEGER time_over;	//结束时间
	double dqFreq;		//计时器频率
	LARGE_INTEGER f;	//计时器频率
	QueryPerformanceFrequency(&f);
	dqFreq = (double)f.QuadPart;
	QueryPerformanceCounter(&time_start);	//计时开始
	while (*max_element(delta.begin(), delta.end()) > episinon)
	{
		#pragma omp parallel
		{
			int tid;
			double temp = 0;
			tid = omp_get_thread_num();
			for (int i = 0; i < size; i++)
			{
				if (i != tid)
				{
					temp = temp + A[tid][i] * old_x[i];
				}
			}
			x[tid] = (b[tid] - temp) / A[tid][tid];
			delta[tid] = abs(x[tid] - old_x[tid]);
		}
		for (int k = 0; k < size; k++)
		{
			old_x[k] = x[k];
		}
	}
	QueryPerformanceCounter(&time_over);	//计时结束
	run_time = 1000000 * (time_over.QuadPart - time_start.QuadPart) / dqFreq;
	//乘以1000000把单位由秒化为微秒，精度为1000 000/（cpu主频）微秒
	printf("\nrun_time：%fus\n", run_time);
	for (int i = 0; i < size; i++)
	{
		cout << "x" << i << "=" << x[i] << endl;
	}
	system("pause");
	return 0;
}
