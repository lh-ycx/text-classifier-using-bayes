#include <iostream>
#include <fstream>
#include <cstdio>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <string>
#include <set>
#include <map>

using namespace std;
const int TypeNum = 4; //文档类别数
const int GroupNum = 10; //组数
const int Length = 50; //文档路径最长长度
char TypeName[TypeNum][30] = {"c1_atheism", "c2_sci.crypt", "c3_talk.politics.guns", "c4_comp.sys.mac.hardware"}; //文档类别名称
set<string> word; //单词表
int doc_num; //文档数
struct Result //贝叶斯学习结果
{
    map<string, int>  count; //count[word]表示该类中word出现的次数
    int len; //该类单词总数
    int doc; //该类文档总数
}res[TypeNum];
struct Prob //条件概率计算的结果
{
    double p[TypeNum];
};
map<string, Prob> prob; //prob[word].p[i]表示p(word|Ci)
int total, correct, wrong; //分别为测试集文档总数、分类正确文档数、分类错误文档数
double p[TypeNum]; //分类过程中为每类计算的临时变量

void init() //初始化变量
{
    word.clear();
    for (int i=0; i<TypeNum; i++)
    {
        res[i].count.clear();
        res[i].doc=0;
        res[i].len=0;
    }
    doc_num = 0;
    prob.clear();
    total = 0;
    correct = 0;
    wrong = 0;
}

bool valid(char ch) //判断一个字符是否属于一个单词
{
    if (ch=='\'' || ch=='-') return true;
    if (ch>='0' && ch<='9') return true;
    if (ch>='a' && ch<='z') return true;
    if (ch>='A' && ch<='Z') return true;
    return false;
}

void insert(string w, int type) //用一个单词更新统计结果
{
    word.insert(w);
    res[type].count[w]++;
    res[type].len++;
}

void study(ifstream &icin, int type) //学习一个类型为type的文档
{
    res[type].doc++;
    doc_num++;
    char ch;
    string now="";
    icin.unsetf(ios::skipws);
    while (icin>>ch)
    {
        if (valid(ch)) now+=ch;
        else
        {
            if (now!="") insert(now, type);
            now="";
        }
    }
    if (now!="") insert(now, type);
}

void calculate() //为每一个单词计算概率p(word|Ci)
{
    int examples = word.size();
    for (set<string>::iterator it = word.begin(); it != word.end(); it++)
    {
        string w = *it;
        Prob p_tmp;
        for (int i=0; i<TypeNum; i++)
        {
            p_tmp.p[i] = (double)(res[i].count[w] + 1) / (double)(res[i].len + examples);
        }
        prob[w] = p_tmp;
    }
}

void solve(string w) //用单词w更新测试过程中的计算结果
{
    if (word.find(w)!=word.end())
    {
        for (int i=0; i<TypeNum; i++)
            p[i] += log(prob[w].p[i]);
    }
}

void check(ifstream &icin, int type) //对一个类型为type的文档进行测试
{
    for (int i=0; i<TypeNum; i++)
        p[i] = log((double)res[i].doc / (double)doc_num);
    char ch;
    string now="";
    icin.unsetf(ios::skipws);
    while (icin>>ch)
    {
        if (valid(ch)) now+=ch;
        else
        {
            if (now!="") solve(now);
            now="";
        }
    }
    if (now!="") solve(now);
    int ans=0;
    for (int i=0; i<TypeNum; i++)
        if (p[i] > p[ans]) ans = i;
    total++;
    if (ans==type) correct++; else wrong++;
}

void output(int test_group) //输出测试结果
{
    printf("Test Group: %d\n", test_group);
    cout<<doc_num<<' '<<total<<' '<<correct<<' '<<wrong<<endl;
    printf("Correct Percent: %.4lf\n", (double)correct/(double)total);
}

int main()
{
    for (int test_group = 0; test_group < GroupNum; test_group++) //枚举测试组
    {
        init();
        for (int type=0; type < TypeNum; type++)
            for (int group = 0; group < GroupNum; group++)
                if (group != test_group)
                {
                    DIR *dir;
                    struct dirent *ptr;
                    char path[Length] = "data/", tmp[Length] = "/0/";
                    strcat(path, TypeName[type]);
                    tmp[1] = (char)('0'+group);
                    strcat(path, tmp);
                    dir = opendir(path);
                    while ((ptr = readdir(dir)) != NULL) //枚举需要学习的文档
                    {
                        if (ptr->d_name[0]=='.') continue;
                        ifstream icin;
                        char file_path[Length];
                        strcpy(file_path, path);
                        strcat(file_path, ptr->d_name);
                        icin.open(file_path);
                        study(icin, type);
                    }
                    closedir(dir);
                }
        calculate();
        for (int type=0; type < TypeNum; type++)
        {
            DIR *dir;
            struct dirent *ptr;
            char path[Length] = "data/", tmp[Length] = "/0/";
            strcat(path, TypeName[type]);
            tmp[1] = (char)('0'+test_group);
            strcat(path, tmp);
            dir = opendir(path);
            while ((ptr = readdir(dir)) != NULL) //枚举需要测试的文档
            {
                if (ptr->d_name[0]=='.') continue;
                ifstream icin;
                char file_path[Length];
                strcpy(file_path, path);
                strcat(file_path, ptr->d_name);
                icin.open(file_path);
                check(icin, type);
             }
             closedir(dir);
         }
         output(test_group); //输出分类结果
    }
    return 0;
}
