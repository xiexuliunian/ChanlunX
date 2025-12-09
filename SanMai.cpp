#include "SanMai.h"
#include <vector>
#include <algorithm>

// 判断是否形成三买信号
// pIn: 笔或段的端点数据(1为顶点，-1为底点)
// pHigh: 最高价数组
// pLow: 最低价数组
// pClose: 收盘价数组
std::vector<float> SanMai(int nCount, std::vector<float> pIn, std::vector<float> pHigh, std::vector<float> pLow, std::vector<float> pClose)
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
    
    // 至少需要3个底点和3个顶点才能形成三买
    if (botIndexes.size() < 3 || topIndexes.size() < 3)
    {
        return pOut;
    }
    
    // 从第3个底点开始检查是否形成三买
    for (size_t i = 2; i < botIndexes.size(); i++)
    {
        int bot1Idx = botIndexes[i - 2];  // 第一个底点
        int bot2Idx = botIndexes[i - 1];  // 第二个底点(二买位置)
        int bot3Idx = botIndexes[i];      // 第三个底点(三买位置)
        
        float bot1Price = botPrices[i - 2];
        float bot2Price = botPrices[i - 1];
        float bot3Price = botPrices[i];
        
        // 找到这些底点之间的顶点
        int top1Idx = -1, top2Idx = -1, top3Idx = -1;
        float top1Price = 0, top2Price = 0, top3Price = 0;
        
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
        
        // 找第三个顶点(在bot3之后)
        for (size_t j = 0; j < topIndexes.size(); j++)
        {
            if (topIndexes[j] > bot3Idx)
            {
                top3Idx = topIndexes[j];
                top3Price = topPrices[j];
                break;
            }
        }
        
        if (top1Idx == -1 || top2Idx == -1)
        {
            continue;
        }
        
        // 三买的条件:
        // 1. 形成了一个中枢
        // 2. bot3高于bot1和bot2(底部抬高，上升趋势)
        // 3. 突破了前期高点或者接近前期高点
        // 4. 回调不深，保持在中枢上方
        
        // 计算中枢
        float zhongShuHigh = std::min(top1Price, top2Price);
        float zhongShuLow = std::max(bot1Price, bot2Price);
        bool hasZhongShu = (zhongShuHigh > zhongShuLow);
        
        // 三买条件检查
        bool bottomRising = (bot3Price > bot1Price) && (bot3Price > bot2Price * 0.98);
        bool aboveZhongShu = (bot3Price >= zhongShuLow * 0.95); // 回调不跌破中枢下沿
        
        // 检查是否突破或接近前高
        bool nearPrevHigh = false;
        if (top3Idx != -1)
        {
            float maxPrevTop = std::max(top1Price, top2Price);
            nearPrevHigh = (top3Price >= maxPrevTop * 0.95);
        }
        
        // 如果满足三买条件
        if (hasZhongShu && bottomRising && aboveZhongShu)
        {
            // 在第三个底点标记三买信号
            // 需要确认价格继续向上
            for (int k = bot3Idx; k < nCount && k <= bot3Idx + 5; k++)
            {
                // 检查是否继续上涨
                if (pClose[k] > bot3Price * 1.02) // 上涨超过2%
                {
                    pOut[bot3Idx] = 1; // 在第三个底点标记三买
                    break;
                }
            }
        }
    }
    
    return pOut;
}

// 更严格的三买判断(包含中枢计算)
std::vector<float> SanMaiStrict(int nCount, std::vector<float> pIn, std::vector<float> pHigh, std::vector<float> pLow, std::vector<float> pClose)
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
    
    // 至少需要7个转折点才能形成完整的三买结构
    if (indexes.size() < 7)
    {
        return pOut;
    }
    
    // 检查每个可能的三买位置
    for (size_t i = 6; i < indexes.size(); i++)
    {
        // 标准的三买结构: 底-顶-底-顶-底-顶-底
        // 其中bot3(i-4), bot4(i-2), bot5(i)是三个关键底点
        if (i >= 6 && 
            types[i-6] == -1 && types[i-5] == 1 && 
            types[i-4] == -1 && types[i-3] == 1 && 
            types[i-2] == -1 && types[i-1] == 1 && 
            types[i] == -1)
        {
            float bot1 = prices[i-6];
            float top1 = prices[i-5];
            float bot2 = prices[i-4];
            float top2 = prices[i-3];
            float bot3 = prices[i-2];  // 潜在的二买点
            float top3 = prices[i-1];
            float bot4 = prices[i];    // 潜在的三买点
            
            // 计算中枢区间
            float zhongShuHigh = std::min(std::min(top1, top2), top3);
            float zhongShuLow = std::max(std::max(bot1, bot2), bot3);
            
            // 存在中枢
            if (zhongShuHigh > zhongShuLow)
            {
                // 三买条件:
                // 1. bot4高于中枢下沿(不跌破中枢)
                // 2. bot4高于bot3(底部抬高)
                // 3. 突破了之前的高点
                
                bool aboveZhongShu = (bot4 > zhongShuLow * 0.98);
                bool bottomRising = (bot4 > bot3 * 0.99);
                
                // 检查是否突破前高
                float maxTop = std::max(std::max(top1, top2), top3);
                bool breakPrevHigh = (top3 > maxTop * 0.98);
                
                if (aboveZhongShu && bottomRising && breakPrevHigh)
                {
                    int signalIdx = indexes[i];
                    // 确认继续上涨
                    for (int k = signalIdx; k < nCount && k <= signalIdx + 3; k++)
                    {
                        if (pClose[k] > bot4 * 1.015)
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
