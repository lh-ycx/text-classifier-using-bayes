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
const int TypeNum = 4; //�ĵ������
const int GroupNum = 10; //����
const int Length = 50; //�ĵ�·�������
char TypeName[TypeNum][30] = {"c1_atheism", "c2_sci.crypt", "c3_talk.politics.guns", "c4_comp.sys.mac.hardware"}; //�ĵ��������
set<string> word; //���ʱ�
int doc_num; //�ĵ���
struct Result //��Ҷ˹ѧϰ���
{
    map<string, int>  count; //count[word]��ʾ������word���ֵĴ���
    int len; //���൥������
    int doc; //�����ĵ�����
}res[TypeNum];
struct Prob //�������ʼ���Ľ��
{
    double p[TypeNum];
};
map<string, Prob> prob; //prob[word].p[i]��ʾp(word|Ci)
int total, correct, wrong; //�ֱ�Ϊ���Լ��ĵ�������������ȷ�ĵ�������������ĵ���
double p[TypeNum]; //���������Ϊÿ��������ʱ����

void init() //��ʼ������
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

bool valid(char ch) //�ж�һ���ַ��Ƿ�����һ������
{
    if (ch=='\'' || ch=='-') return true;
    if (ch>='0' && ch<='9') return true;
    if (ch>='a' && ch<='z') return true;
    if (ch>='A' && ch<='Z') return true;
    return false;
}

void insert(string w, int type) //��һ�����ʸ���ͳ�ƽ��
{
    word.insert(w);
    res[type].count[w]++;
    res[type].len++;
}

void study(ifstream &icin, int type) //ѧϰһ������Ϊtype���ĵ�
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

void calculate() //Ϊÿһ�����ʼ������p(word|Ci)
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

void solve(string w) //�õ���w���²��Թ����еļ�����
{
    if (word.find(w)!=word.end())
    {
        for (int i=0; i<TypeNum; i++)
            p[i] += log(prob[w].p[i]);
    }
}

void check(ifstream &icin, int type) //��һ������Ϊtype���ĵ����в���
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

void output(int test_group) //������Խ��
{
    printf("Test Group: %d\n", test_group);
    cout<<doc_num<<' '<<total<<' '<<correct<<' '<<wrong<<endl;
    printf("Correct Percent: %.4lf\n", (double)correct/(double)total);
}

int main()
{
    for (int test_group = 0; test_group < GroupNum; test_group++) //ö�ٲ�����
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
                    while ((ptr = readdir(dir)) != NULL) //ö����Ҫѧϰ���ĵ�
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
            while ((ptr = readdir(dir)) != NULL) //ö����Ҫ���Ե��ĵ�
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
         output(test_group); //���������
    }
    return 0;
}
