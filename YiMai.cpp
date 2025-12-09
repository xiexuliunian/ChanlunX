#include "YiMai.h"
#include <vector>
#include <algorithm>

// 判断是否形成一买信号
// pIn: 笔或段的端点数据(1为顶点，-1为底点)
// pHigh: 最高价数组
// pLow: 最低价数组
// pClose: 收盘价数组
std::vector<float> YiMai(int nCount, std::vector<float> pIn, std::vector<float> pHigh, std::vector<float> pLow, std::vector<float> pClose)
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
    
    // 至少需要2个底点和1个顶点才能形成一买
    if (botIndexes.size() < 2 || topIndexes.size() < 1)
    {
        return pOut;
    }
    
    // 从第2个底点开始检查是否形成一买
    for (size_t i = 1; i < botIndexes.size(); i++)
    {
        int bot1Idx = botIndexes[i - 1];  // 第一个底点(中枢内的底)
        int bot2Idx = botIndexes[i];      // 第二个底点(一买位置，跌破中枢后的底)
        
        float bot1Price = botPrices[i - 1];
        float bot2Price = botPrices[i];
        
        // 找到这两个底点之间的顶点
        int topIdx = -1;
        float topPrice = 0;
        
        // 找顶点(在bot1之后且在bot2之前)
        for (size_t j = 0; j < topIndexes.size(); j++)
        {
            if (topIndexes[j] > bot1Idx && topIndexes[j] < bot2Idx)
            {
                topIdx = topIndexes[j];
                topPrice = topPrices[j];
                break;
            }
        }
        
        if (topIdx == -1)
        {
            continue;
        }
        
        // 一买的条件:
        // 1. 第二个底点明显跌破第一个底点(说明突破了中枢下沿)
        // 2. 下跌出现背驰(第二段下跌力度减弱)
        // 3. 价格开始反转向上
        
        bool breakBot1 = (bot2Price < bot1Price * 0.97); // 跌破3%以上
        
        // 计算力度背驰
        // 寻找bot1之前的顶点
        int prevTopIdx = -1;
        float prevTopPrice = 0;
        for (int j = (int)topIndexes.size() - 1; j >= 0; j--)
        {
            if (topIndexes[j] < bot1Idx)
            {
                prevTopIdx = topIndexes[j];
                prevTopPrice = topPrices[j];
                break;
            }
        }
        
        if (prevTopIdx != -1)
        {
            // 计算两段下跌的力度
            float drop1 = prevTopPrice - bot1Price;  // 第一段下跌
            float drop2 = topPrice - bot2Price;      // 第二段下跌
            bool hasBeiChi = (drop2 > drop1 * 0.8) && (drop2 < drop1 * 1.1); // 第二段下跌力度接近但不超过第一段
            
            // 如果满足一买条件
            if (breakBot1 && hasBeiChi)
            {
                // 在第二个底点后的若干K线标记一买信号
                for (int k = bot2Idx; k < nCount && k <= bot2Idx + 5; k++)
                {
                    // 检查是否开始反转向上
                    if (pClose[k] > bot2Price * 1.005) // 反转超过0.5%
                    {
                        pOut[bot2Idx] = 1; // 在第二个底点标记一买
                        break;
                    }
                }
            }
        }
        else
        {
            // 如果没有找到之前的顶点，使用简化判断
            // 只要跌破前底且有一定幅度
            if (breakBot1)
            {
                for (int k = bot2Idx; k < nCount && k <= bot2Idx + 5; k++)
                {
                    if (pClose[k] > bot2Price * 1.005)
                    {
                        pOut[bot2Idx] = 1;
                        break;
                    }
                }
            }
        }
    }
    
    return pOut;
}

// 更严格的一买判断(包含中枢计算)
std::vector<float> YiMaiStrict(int nCount, std::vector<float> pIn, std::vector<float> pHigh, std::vector<float> pLow, std::vector<float> pClose)
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
    
    // 至少需要4个转折点才能形成中枢并检测一买
    if (indexes.size() < 4)
    {
        return pOut;
    }
    
    // 检查每个可能的一买位置
    for (size_t i = 3; i < indexes.size(); i++)
    {
        // 一买结构: 顶-底-顶-底(最后一个底是一买点，跌破中枢)
        if (i >= 3 && 
            types[i-3] == 1 && types[i-2] == -1 && 
            types[i-1] == 1 && types[i] == -1)
        {
            float top1 = prices[i-3];
            float bot1 = prices[i-2];
            float top2 = prices[i-1];
            float bot2 = prices[i];
            
            // 计算中枢区间(简化: 使用前两笔的重叠区域)
            float zhongShuHigh = std::min(top1, top2);
            float zhongShuLow = std::max(bot1, bot1); // bot1作为中枢下沿参考
            
            // 一买条件:
            // 1. bot2跌破中枢下沿(bot1)
            // 2. 有背驰迹象
            
            bool breakZhongShu = (bot2 < bot1 * 0.98);
            
            // 背驰: 第二段下跌(top2到bot2)力度接近第一段(top1到bot1)但有衰减
            float drop1 = top1 - bot1;
            float drop2 = top2 - bot2;
            bool beichi = (drop2 > drop1 * 0.7) && (drop2 < drop1 * 1.15);
            
            if (breakZhongShu && beichi)
            {
                int signalIdx = indexes[i];
                // 确认反转
                for (int k = signalIdx; k < nCount && k <= signalIdx + 3; k++)
                {
                    if (pClose[k] > bot2 * 1.005)
                    {
                        pOut[signalIdx] = 1;
                        break;
                    }
                }
            }
        }
    }
    
    return pOut;
}
