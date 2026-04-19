#ifndef __YIMAI_H__
#define __YIMAI_H__

#include <vector>

#pragma pack(push, 1)

// 一买判断(基础版本)
// 参数:
//   nCount: K线数量
//   pIn: 笔或段的端点数据(1为顶点，-1为底点)
//   pHigh: 最高价数组
//   pLow: 最低价数组
//   pClose: 收盘价数组
// 返回: 一买信号数组(1表示一买点)
std::vector<float> YiMai(int nCount, std::vector<float> pIn, std::vector<float> pHigh, std::vector<float> pLow, std::vector<float> pClose);

// 一买判断(严格版本，包含中枢计算)
std::vector<float> YiMaiStrict(int nCount, std::vector<float> pIn, std::vector<float> pHigh, std::vector<float> pLow, std::vector<float> pClose);

#pragma pack(pop)

#endif
