#include <iostream>
#include <fstream>
#include <exception>
#include "BiChuLi.h"
#include "KxianChuLi.h"

using namespace std;

bool ifChengbi(vector<Kxian> &tempKxianList, int direction)
{
    if (tempKxianList.size() < 4)
    {
        // 如果连4根K线都没有的话肯定不是笔
        return false;
    }
    if (direction == -1)
    {
        // 是不是向下成笔
        // 先找有没有下降K线
        unsigned int i = 2;
        while (true)
        {
            for (; i < tempKxianList.size(); i++)
            {
                if (tempKxianList.at(i).di < tempKxianList.at(i - 1).di && tempKxianList.at(i - 1).di < tempKxianList.at(i - 2).di)
                {
                    // 找到下降K线
                    break;
                }
            }
            if (i >= tempKxianList.size())
            {
                // 下降K线都没找到
                return false;
            }
            // 找前面的最低价
            float zuiDiJia = tempKxianList.at(i).di;
            // 如果出现了更低价，这个笔就成立了
            for (unsigned int j = i + 1; j < tempKxianList.size(); j++)
            {
                if (tempKxianList.at(j).di < zuiDiJia)
                {
                    return true;
                }
            }
            i = i + 1;
        }
    }
    else if (direction == 1)
    {
        // 是不是向上成笔
        // 先找有没有上升K线
        unsigned int i = 2;
        while (true)
        {
            for (; i < tempKxianList.size(); i++)
            {
                if (tempKxianList.at(i).gao > tempKxianList.at(i - 1).gao && tempKxianList.at(i - 1).gao > tempKxianList.at(i - 2).gao)
                {
                    // 找到上升K线
                    break;
                }
            }
            if (i >= tempKxianList.size())
            {
                // 上升K线都没找到
                return false;
            }
            // 找前面的最高价
            float zuiGaoJia = tempKxianList.at(i).gao;
            // 如果出现了更高价，这个笔就成立了
            for (unsigned int j = i + 1; j < tempKxianList.size(); j++)
            {
                if (tempKxianList.at(j).gao > zuiGaoJia)
                {
                    return true;
                }
            }
            i = i + 1;
        }
    }
    return false;
}

void BiChuLi::handle(vector<Kxian> &kxianList)
{
    vector<Kxian> tempKxianList; // 临时未成笔K线的保存
    for (vector<Kxian>::iterator iter = kxianList.begin(); iter != kxianList.end(); iter++)
    {
        if (this->biList.empty())
        {
            // 不再默认生成第一笔为向上，而是先把K线放入临时队列，等待成笔条件
            tempKxianList.push_back(*iter);

            // 尝试根据现有临时K线确定第一笔方向（如果已具备最小成笔条件）
            if (tempKxianList.size() >= 4)
            {
                if (ifChengbi(tempKxianList, 1))
                {
                    // 形成向上第一笔
                    Bi bi;
                    bi.fangXiang = 1;
                    bi.kaiShi = tempKxianList.front().kaiShi;
                    bi.jieShu = tempKxianList.back().jieShu;
                    // 以临时K线中最靠后的最高价作为笔顶
                    bi.gao = tempKxianList.back().gao;
                    // 笔底使用临时K线中最早的最低价（可根据需要改进）
                    bi.di = tempKxianList.front().di;
                    for (vector<Kxian>::iterator it = tempKxianList.begin(); it != tempKxianList.end(); it++)
                    {
                        bi.kxianList.push_back(*it);
                    }
                    tempKxianList.clear();
                    this->biList.push_back(bi);
                }
                else if (ifChengbi(tempKxianList, -1))
                {
                    // 形成向下第一笔
                    Bi bi;
                    bi.fangXiang = -1;
                    bi.kaiShi = tempKxianList.front().kaiShi;
                    bi.jieShu = tempKxianList.back().jieShu;
                    // 以临时K线中最靠后的最低价作为笔底
                    bi.di = tempKxianList.back().di;
                    // 笔顶使用临时K线中最早的最高价（可根据需要改进）
                    bi.gao = tempKxianList.front().gao;
                    for (vector<Kxian>::iterator it = tempKxianList.begin(); it != tempKxianList.end(); it++)
                    {
                        bi.kxianList.push_back(*it);
                    }
                    tempKxianList.clear();
                    this->biList.push_back(bi);
                }
            }
        }
        else
        {
            if (this->biList.back().fangXiang == 1)
            {
                // 上一笔是向上笔
                if ((*iter).gao >= this->biList.back().gao)
                {
                    // 向上笔继续延续
                    this->biList.back().jieShu = (*iter).jieShu;
                    this->biList.back().gao = (*iter).gao;
                    if (tempKxianList.size() > 0)
                    {
                        for (vector<Kxian>::iterator it = tempKxianList.begin(); it != tempKxianList.end(); it++)
                        {
                            this->biList.back().kxianList.push_back(*it);
                        }
                        tempKxianList.clear();
                    }
                    this->biList.back().kxianList.push_back(*iter);
                }
                else
                {
                    tempKxianList.push_back(*iter);
                    // 有没有成新的向下笔
                    if (ifChengbi(tempKxianList, -1))
                    {
                        Bi bi;
                        bi.fangXiang = -1;
                        bi.kaiShi = this->biList.back().jieShu;
                        bi.jieShu = tempKxianList.back().jieShu;
                        bi.di = tempKxianList.back().di;
                        bi.gao = this->biList.back().gao;
                        for (vector<Kxian>::iterator it = tempKxianList.begin(); it != tempKxianList.end(); it++)
                        {
                            bi.kxianList.push_back(*it);
                        }
                        tempKxianList.clear();
                        this->biList.push_back(bi);
                    }
                }
            }
            else if (this->biList.back().fangXiang == -1)
            {
                // 上一笔是向下笔
                if ((*iter).di <= this->biList.back().di)
                {
                    // 向下笔继续延续
                    this->biList.back().jieShu = (*iter).jieShu;
                    this->biList.back().di = (*iter).di;
                    if (tempKxianList.size() > 0)
                    {
                        for (vector<Kxian>::iterator it = tempKxianList.begin(); it != tempKxianList.end(); it++)
                        {
                            this->biList.back().kxianList.push_back(*it);
                        }
                        tempKxianList.clear();
                    }
                    this->biList.back().kxianList.push_back(*iter);
                }
                else
                {
                    tempKxianList.push_back(*iter);
                    // 有没有成新的向上笔
                    if (ifChengbi(tempKxianList, 1))
                    {
                        Bi bi;
                        bi.fangXiang = 1;
                        bi.kaiShi = this->biList.back().jieShu;
                        bi.jieShu = tempKxianList.back().jieShu;
                        bi.gao = tempKxianList.back().gao;
                        bi.di = this->biList.back().di;
                        for (vector<Kxian>::iterator it = tempKxianList.begin(); it != tempKxianList.end(); it++)
                        {
                            bi.kxianList.push_back(*it);
                        }
                        tempKxianList.clear();
                        this->biList.push_back(bi);
                    }
                }
            }
        }
    }
    if (tempKxianList.size() >= 4)
    {
        if (this->biList.back().fangXiang == 1)
        {
            if (ifChengbi(tempKxianList, -1))
            {
                Bi bi;
                bi.fangXiang = -1;
                bi.kaiShi = this->biList.back().jieShu;
                bi.jieShu = tempKxianList.back().jieShu;
                bi.di = tempKxianList.back().di;
                bi.gao = this->biList.back().gao;
                for (vector<Kxian>::iterator it = tempKxianList.begin(); it != tempKxianList.end(); it++)
                {
                    bi.kxianList.push_back(*it);
                }
                tempKxianList.clear();
                this->biList.push_back(bi);
            }
        }
        else if (this->biList.back().fangXiang == -1)
        {
            if (ifChengbi(tempKxianList, 1))
            {
                Bi bi;
                bi.fangXiang = 1;
                bi.kaiShi = this->biList.back().jieShu;
                bi.jieShu = tempKxianList.back().jieShu;
                bi.gao = tempKxianList.back().gao;
                bi.di = this->biList.back().di;
                for (vector<Kxian>::iterator it = tempKxianList.begin(); it != tempKxianList.end(); it++)
                {
                    bi.kxianList.push_back(*it);
                }
                tempKxianList.clear();
                this->biList.push_back(bi);
            }
        }
    }
}
