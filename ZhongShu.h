#ifndef __ZHONGSHU_H__
#define __ZHONGSHU_H__

#include <vector>
#include <algorithm>

#pragma pack(push, 1)

struct ZhongShu
{
    bool bValid;
    int nTop1, nTop2, nTop3, nBot1, nBot2, nBot3;
    float fTop1, fTop2, fTop3, fBot1, fBot2, fBot3;
    int nLines, nStart, nEnd;
    float fHigh, fLow;
    int nDirection;
    int nTerminate;

    ZhongShu();
    ~ZhongShu();

    void Reset();
    bool PushHigh(int nIndex, float fValue);
    bool PushLow(int nIndex, float fValue);
};

struct Pivot
{
    int s;
    int e;
    float zg;    // 中枢上轨
    float zd;    // 中枢下轨
    float gg;    // 区间内最高
    float dd;    // 区间内最低
    float direction;
    bool affirm;
    int terminate; // 0:not terminated, 1:terminated upward, -1:terminated downward
};

std::vector<Pivot> ZS(int nCount, std::vector<float> pIn, std::vector<float> pHigh, std::vector<float> pLow);

#pragma pack(pop)

#endif
