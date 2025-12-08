#include "ZhongShu.h"
#include <limits>

// Rewritten ZhongShu detection to follow canonical Chanlun definition:
// - Build segments from consecutive pivots (pIn markers)
// - A ZhongShu is formed when three consecutive segments have a non-empty price intersection
// - The ZhongShu can be extended by following segments as long as intersection remains non-empty
// - Termination occurs when a following segment lies entirely above or below the current ZhongShu intersection

struct Seg
{
    int s;
    int e;
    float high;
    float low;
    int dir; // 1 up, -1 down
};

ZhongShu::ZhongShu()
{
    this->bValid = false;
    this->nTop1 = 0;
    this->nTop2 = 0;
    this->nTop3 = 0;
    this->nBot1 = 0;
    this->nBot2 = 0;
    this->nBot3 = 0;
    this->fTop1 = 0;
    this->fTop2 = 0;
    this->fTop3 = 0;
    this->fBot1 = 0;
    this->fBot2 = 0;
    this->fBot3 = 0;
    this->nLines = 0;
    this->nStart = 0;
    this->nEnd = 0;
    this->fHigh = 0;
    this->fLow = 0;
    this->nDirection = 0;
    this->nTerminate = 0;
}

ZhongShu::~ZhongShu()
{
}

void ZhongShu::Reset()
{
    this->bValid = false;
    this->nTop1 = 0;
    this->nTop2 = 0;
    this->nTop3 = 0;
    this->nBot1 = 0;
    this->nBot2 = 0;
    this->nBot3 = 0;
    this->fTop1 = 0;
    this->fTop2 = 0;
    this->fTop3 = 0;
    this->fBot1 = 0;
    this->fBot2 = 0;
    this->fBot3 = 0;
    this->nLines = 0;
    this->nStart = 0;
    this->nEnd = 0;
    this->fHigh = 0;
    this->fLow = 0;
    this->nDirection = 0;
    this->nTerminate = 0;
}

bool ZhongShu::PushHigh(int nIndex, float fValue)
{
    // Legacy: keep for compatibility but not used by new ZS implementation
    nTop3 = nTop2;
    fTop3 = fTop2;
    nTop2 = nTop1;
    fTop2 = fTop1;
    nTop1 = nIndex;
    fTop1 = fValue;
    if (bValid == true)
    {
        if (fTop1 < fLow) // 中枢终结
        {
            nTerminate = -1;
            if (nTop2 > nEnd)
            {
                nEnd = nTop2;
            }
            return true;
        }
        else
        {
            if (nBot1 > nEnd)
            {
                nEnd = nBot1; // 中枢终结点后移
            }
        }
    }
    else
    {
        if (nTop3 > 0 && nTop2 > 0 && nTop1 > 0 && nBot2 > 0 && nBot1 > 0) // 有两个高点和两个低点
        {
            float fTempHigh = (fTop1 < fTop2 ? fTop1 : fTop2);
            float fTempLow = (fBot1 > fBot2 ? fBot1 : fBot2);
            if (fTop3 > fTop2 && fTempHigh > fTempLow) // 有中枢
            {
                nDirection = -1; // 向下中枢
                nStart = nBot2;
                nEnd = nTop1;
                fHigh = fTempHigh;
                fLow = fTempLow;
                bValid = true;
            }
        }
    }
    return false;
}

bool ZhongShu::PushLow(int nIndex, float fValue)
{
    // Legacy: keep for compatibility but not used by new ZS implementation
    nBot3 = nBot2;
    fBot3 = fBot2;
    nBot2 = nBot1;
    fBot2 = fBot1;
    nBot1 = nIndex;
    fBot1 = fValue;
    if (bValid == true)
    {
        if (fBot1 > fHigh) // 中枢终结
        {
            nTerminate = 1;
            if (nBot2 > nEnd)
            {
                nEnd = nBot2;
            }
            return true;
        }
        else
        {
            if (nTop1 > nEnd)
            {
                nEnd = nTop1; // 中枢终结点后移
            }
        }
    }
    else
    {
        if (nTop2 > 0 && nTop1 > 0 && nBot3 > 0 && nBot2 > 0 && nBot1 > 0) // 有两个高点和两个低点
        {
            float fTempHigh = (fTop1 < fTop2 ? fTop1 : fTop2);
            float fTempLow = (fBot1 > fBot2 ? fBot1 : fBot2);
            if (fBot3 < fBot2 && fTempHigh > fTempLow) // 有中枢
            {
                nDirection = 1; // 向上中枢
                nStart = nTop2;
                nEnd = nBot1;
                fHigh = fTempHigh;
                fLow = fTempLow;
                bValid = true;
            }
        }
    }
    return false;
}

std::vector<Pivot> ZS(int nCount, std::vector<float> pIn, std::vector<float> pHigh, std::vector<float> pLow)
{
    std::vector<Pivot> ZhongShuList;

    // collect pivot indices
    std::vector<int> pivots;
    pivots.reserve(nCount);
    for (int i = 0; i < nCount; ++i)
    {
        if (pIn[i] != 0)
            pivots.push_back(i);
    }

    if (pivots.size() < 4)
        return ZhongShuList; // need at least 3 segments (4 pivots)

    // build segments between consecutive pivots
    std::vector<Seg> segs;
    segs.reserve(pivots.size() - 1);
    for (size_t k = 1; k < pivots.size(); ++k)
    {
        int a = pivots[k - 1];
        int b = pivots[k];
        Seg s;
        s.s = a;
        s.e = b;
        s.high = std::max(pHigh[a], pHigh[b]);
        s.low = std::min(pLow[a], pLow[b]);
        // direction: if starting pivot is low (-1) then segment is up, else down
        s.dir = (pIn[a] == -1) ? 1 : -1;
        segs.push_back(s);
    }

    // scan triples to find ZhongShu
    for (size_t i = 0; i + 2 < segs.size(); ++i)
    {
        float overlap_low = std::max(std::max(segs[i].low, segs[i + 1].low), segs[i + 2].low);
        float overlap_high = std::min(std::min(segs[i].high, segs[i + 1].high), segs[i + 2].high);
        if (overlap_low <= overlap_high)
        {
            Pivot p;
            p.s = segs[i].s;
            p.e = segs[i + 2].e;
            p.zg = overlap_high;
            p.zd = overlap_low;
            p.direction = (float)segs[i + 1].dir; // middle segment direction
            p.affirm = true;
            p.terminate = 0;
            // extend with subsequent segments while intersection remains non-empty
            size_t j = i + 3;
            float cur_low = p.zd;
            float cur_high = p.zg;
            while (j < segs.size())
            {
                float new_low = std::max(cur_low, segs[j].low);
                float new_high = std::min(cur_high, segs[j].high);
                if (new_low <= new_high)
                {
                    // extend
                    cur_low = new_low;
                    cur_high = new_high;
                    p.e = segs[j].e;
                    p.zd = cur_low;
                    p.zg = cur_high;
                    ++j;
                }
                else
                {
                    // no overlap -> termination check
                    if (segs[j].high < cur_low)
                    {
                        p.terminate = -1; // terminated downward (next seg entirely below)
                    }
                    else if (segs[j].low > cur_high)
                    {
                        p.terminate = 1; // terminated upward (next seg entirely above)
                    }
                    break;
                }
            }
            // determine gg/dd (max/min inside open interval (s+1, e-1))
            if (p.e > p.s + 1)
            {
                auto hi_it_begin = pHigh.begin() + p.s + 1;
                auto hi_it_end = pHigh.begin() + p.e;
                auto lo_it_begin = pLow.begin() + p.s + 1;
                auto lo_it_end = pLow.begin() + p.e;
                p.gg = *std::max_element(hi_it_begin, hi_it_end);
                p.dd = *std::min_element(lo_it_begin, lo_it_end);
            }
            else
            {
                // no interior points
                p.gg = p.zg;
                p.dd = p.zd;
            }
            ZhongShuList.push_back(p);
            // continue scan after the last segment used (j - 1). set i = j - 1 (outer loop will i++)
            if (j > i + 2)
                i = j - 1;
        }
    }

    return ZhongShuList;
}
