#include "pch.h"
#include <iostream>
#include <iomanip>
#include<fstream>
#include <string>
#include <vector>
using namespace std;
int c_Num=-1;
int s_Num;
vector<int> c_size;
vector<vector<int>> st;
vector<int> ScoreMin;
vector<int> ScoreMax;
vector<int> s_c_num;
vector<int> c_s_num;
int find_c(string cname,vector<string>& all_cname);
int main()
{
    vector<string> name;  //名字字符串的数组
    string s_name;
	string fname;   //文件名字
	string cname;   //课程名字
	//最长的名字长度,以防止因为名字长度不同而造成空格数量错误
	int maxsize = 7;	//因为average长度为7，所以至少设置为7
	vector<string> all_cname;//存放所有课程名字
	//分数最高值、最低值
	//初始化
	cout << "输入读取信息的文件名" << endl;
	cout << "例如 test.txt" << endl;
	cout << ":";
	cin >> fname;
	//以读模式打开文件
	ifstream infile;
	infile.open(fname);

	//采集同学名字、分数信息
	int i=0;
	int score;
	int num;//课程号
	while(infile >> s_name){
		//输入名字，更新名字最大长度
		if(s_name=="END")break;
        s_c_num.push_back(0);
		s_Num++;
        name.push_back(s_name);
		if (name[i].size() > maxsize)maxsize = name[i].size();
		//输入分数并对分数的最大最小值进行更新
		infile>>cname;
		//一位同学的信息以end为结束标志
		while(cname!="end"){
            vector<int> s_c;
			num=find_c(cname,all_cname);//寻找课程号
			infile>>score;
			s_c.push_back(num*100+score);
			//更新分数最大最小值
			if(score<ScoreMin[num])ScoreMin[num]=score;
			if(score>ScoreMax[num])ScoreMax[num]=score;
			

			infile>>cname;
            s_c.clear();
		}
		i++;
	}
	//关闭文件
	infile.close();
	const int s_num = s_Num;
	const int c_num = c_Num;
    short map[s_num][c_num+1];
    short sum[2][s_num]; //(0,x)代表某学生分数和，（x,0）代表某课程分数和
    for(int m=0;m!=s_num;m++){
        for(int n=0;n!=c_num+1;n++){
            map[m][n]=-1;
        }
    }
    for(int m=0;m!=s_num;m++){
        for(int n=0;n!=st[m].size();n++){
            num=st[m][n]/100;
            map[m][num]=st[m][n]%100;
            //更新几个参数，为后面的计算平均数提供便利
            sum[0][m]+=map[m][num];
            sum[1][num]+=map[m][num];
            s_c_num[m]+=1;
            c_s_num[num]+=1;
        }
    }
	for(int no=1;no!=s_num+1;no++){
		cout << left << setw(8) << no;
		cout << left << setw(maxsize + 1) << name[no-1];
		//根据课程名字长度输出分数
		for(int i=0;i<=c_num;i++){
			if(map[no-1][i]==0)cout<<left<<setw(c_size[i]+1)<<" ";
			else cout<<left<<setw(c_size[i]+1)<<map[no-1][i];
		}
		//如果某位同学平均分是整数，则直接输出
		//若是小数，则保留6位有效数字
		if ( sum[0][no-1]% s_c_num[no-1] == 0)
		{
			cout << left << setw(7) << sum[0][no-1]/ s_c_num[no-1] << endl;
			continue;
		}
		cout << left << setw(7) << 1.0*sum[0][no-1]/ s_c_num[no-1] << endl;
		continue;
	}
}
int find_c(string cname,vector<string>& all_cname)
{
	int i=0;
	//寻找现有表中的课程，找到则返回课程号
	while(i<=c_Num)
	{
		if(cname==all_cname[i])return i;
		i++;
	}
	//找不到，新创建课程
	c_num++;
    c_s_num.push_back(0);
    ScoreMax.push_back(0);
    ScoreMin.push_back(100);
    c_size.push_back(cname.size()+1);
	all_cname.push_back(cname);
	return i;
}

