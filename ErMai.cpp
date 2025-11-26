#include "ErMai.h"
#include <vector>
#include <algorithm>

// 判断是否形成二买信号
// pIn: 笔或段的端点数据(1为顶点，-1为底点)
// pHigh: 最高价数组
// pLow: 最低价数组
// pClose: 收盘价数组
std::vector<float> ErMai(int nCount, std::vector<float> pIn, std::vector<float> pHigh, std::vector<float> pLow, std::vector<float> pClose)
{
    std::vector<float> pOut(nCount, 0);
    
    // 记录顶点和底点的位置和价格
    std::vector<int> topIndexes;
    std::vector<float> topPrices;
    std::vector<int> botIndexes;
    std::vector<float> botPrices;
    
    // 收集所有顶点和底点
    for (int i = 0; i < nCount; i++)
    {
        if (pIn[i] == 1)
        {
            topIndexes.push_back(i);
            topPrices.push_back(pHigh[i]);
        }
        else if (pIn[i] == -1)
        {
            botIndexes.push_back(i);
            botPrices.push_back(pLow[i]);
        }
    }
    
    // 至少需要3个底点和2个顶点才能形成二买
    if (botIndexes.size() < 3 || topIndexes.size() < 2)
    {
        return pOut;
    }
    
    // 从第3个底点开始检查是否形成二买
    for (size_t i = 2; i < botIndexes.size(); i++)
    {
        int bot1Idx = botIndexes[i - 2];  // 第一个底点
        int bot2Idx = botIndexes[i - 1];  // 第二个底点(中枢回调后的底)
        int bot3Idx = botIndexes[i];      // 第三个底点(二买位置)
        
        float bot1Price = botPrices[i - 2];
        float bot2Price = botPrices[i - 1];
        float bot3Price = botPrices[i];
        
        // 找到这些底点之间的顶点
        int top1Idx = -1, top2Idx = -1;
        float top1Price = 0, top2Price = 0;
        
        // 找第一个顶点(在bot1和bot2之间)
        for (size_t j = 0; j < topIndexes.size(); j++)
        {
            if (topIndexes[j] > bot1Idx && topIndexes[j] < bot2Idx)
            {
                top1Idx = topIndexes[j];
                top1Price = topPrices[j];
                break;
            }
        }
        
        // 找第二个顶点(在bot2和bot3之间)
        for (size_t j = 0; j < topIndexes.size(); j++)
        {
            if (topIndexes[j] > bot2Idx && topIndexes[j] < bot3Idx)
            {
                top2Idx = topIndexes[j];
                top2Price = topPrices[j];
                break;
            }
        }
        
        if (top1Idx == -1 || top2Idx == -1)
        {
            continue;
        }
        
        // 二买的条件:
        // 1. 形成了一个中枢: bot1 < top1, bot2 < top1, bot2 < top2
        // 2. 第三个底点(二买点)不跌破第一个底点
        // 3. 第三个底点低于第二个底点(回调)
        // 4. 价格有背驰迹象(第三笔的力度小于第一笔)
        
        bool hasZhongShu = (bot1Price < top1Price) && (bot2Price < top1Price);
        bool notBreakBot1 = (bot3Price >= bot1Price * 0.98); // 允许小幅跌破
        bool isRetracement = (bot3Price <= bot2Price);
        
        // 计算力度背驰(简化版本: 比较价格波动幅度)
        float wave1 = top1Price - bot1Price;  // 第一笔的波动
        float wave3 = top2Price - bot3Price;  // 第三笔的波动
        bool hasBeiChi = (wave3 < wave1 * 1.1); // 第三笔力度不强于第一笔
        
        // 如果满足二买条件
        if (hasZhongShu && notBreakBot1 && isRetracement && hasBeiChi)
        {
            // 在第三个底点后的若干K线标记二买信号
            for (int k = bot3Idx; k < nCount && k <= bot3Idx + 5; k++)
            {
                // 检查是否突破第二个顶点，确认二买有效
                if (pClose[k] > bot2Price)
                {
                    pOut[bot3Idx] = 1; // 在第三个底点标记二买
                    break;
                }
            }
        }
    }
    
    return pOut;
}

// 更严格的二买判断(包含中枢计算)
std::vector<float> ErMaiStrict(int nCount, std::vector<float> pIn, std::vector<float> pHigh, std::vector<float> pLow, std::vector<float> pClose)
{
    std::vector<float> pOut(nCount, 0);
    
    std::vector<int> indexes;
    std::vector<float> prices;
    std::vector<int> types; // 1为顶，-1为底
    
    // 收集所有转折点
    for (int i = 0; i < nCount; i++)
    {
        if (pIn[i] == 1 || pIn[i] == -1)
        {
            indexes.push_back(i);
            prices.push_back(pIn[i] == 1 ? pHigh[i] : pLow[i]);
            types.push_back((int)pIn[i]);
        }
    }
    
    // 至少需要5个转折点
    if (indexes.size() < 5)
    {
        return pOut;
    }
    
    // 检查每个可能的二买位置
    for (size_t i = 4; i < indexes.size(); i++)
    {
        // 标准的二买结构: 底-顶-底-顶-底
        if (i >= 4 && 
            types[i-4] == -1 && types[i-3] == 1 && 
            types[i-2] == -1 && types[i-1] == 1 && 
            types[i] == -1)
        {
            float bot1 = prices[i-4];
            float top1 = prices[i-3];
            float bot2 = prices[i-2];
            float top2 = prices[i-1];
            float bot3 = prices[i];
            
            // 计算中枢区间
            float zhongShuHigh = std::min(top1, top2);
            float zhongShuLow = std::max(bot1, bot2);
            
            // 存在中枢
            if (zhongShuHigh > zhongShuLow)
            {
                // 二买条件:
                // 1. bot3不跌破bot1(或跌破很少)
                // 2. bot3在中枢下方
                // 3. 有背驰迹象
                
                bool notBreak = (bot3 >= bot1 * 0.97);
                bool belowZhongShu = (bot3 < zhongShuLow);
                
                // 背驰: 第二段下跌(top2到bot3)弱于第一段下跌(top1到bot2)
                float drop1 = top1 - bot2;
                float drop2 = top2 - bot3;
                bool beichi = (drop2 < drop1 * 0.9);
                
                if (notBreak && belowZhongShu && beichi)
                {
                    int signalIdx = indexes[i];
                    // 确认突破
                    for (int k = signalIdx; k < nCount && k <= signalIdx + 3; k++)
                    {
                        if (pClose[k] > zhongShuLow)
                        {
                            pOut[signalIdx] = 1;
                            break;
                        }
                    }
                }
            }
        }
    }
    
    return pOut;
}
