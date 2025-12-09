# ChanlunX

## 如何编译

### Visual Studio 2019

通达信插件需要编译成32位，下面以Visual Studio 2019举例，作者用的是Visual Studio 2019社区版。

```cmd
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A Win32 ..
cmake --build . --config Release
```

### Visual Studio 2015 或者 Visual Studio 2017

在项目根目录下依次执行以下命令

```cmd
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## 主图代码

把编译好的DLL放到通达信的T0002\dlls目录，绑定为2号函数，下面的代码做成通达信主图公式。

```text
FRAC:=TDXDLL2(2,H,L,0);{标准笔}
NOTEXT画上升笔2:DRAWLINE(FRAC=-1,L,FRAC=+1,H,0), DOTLINE,COLORYELLOW;
NOTEXT画下降笔2:DRAWLINE(FRAC=+1,H,FRAC=-1,L,0), DOTLINE, COLORYELLOW;

BIZG:=TDXDLL2(5,FRAC,H,L);{输出BI中枢高}
BIZD:=TDXDLL2(6,FRAC,H,L);{输出BI中枢低}
BISE:=TDXDLL2(7,FRAC,H,L);{输出BI中枢开始和结束}

NOTEXT_BIZG:IF(BIZG,BIZG,DRAWNULL),COLORYELLOW;{画BI中枢高}
NOTEXT_BIZD:IF(BIZD,BIZD,DRAWNULL),COLORYELLOW;{画BI中枢低}
NOTEXT_BISE:STICKLINE(BISE,BIZD,BIZG,0,0),COLORYELLOW;{画BI中枢起始结束};

DUAN1:=TDXDLL2(3,FRAC,H,L);{计算段的端点,3改成4是1+1终结画法}
NOTEXT画上升段1:DRAWLINE(DUAN1=-1,L,DUAN1=+1,H,0), COLORFF8000;
NOTEXT画下降段1:DRAWLINE(DUAN1=+1,H,DUAN1=-1,L,0), COLORFF8000;

DUANZG1:=TDXDLL2(5,DUAN1,H,L);{输出段中枢高}
DUANZD1:=TDXDLL2(6,DUAN1,H,L);{输出段中枢低}
DUANSE1:=TDXDLL2(7,DUAN1,H,L);{输出段中枢开始和结束}

NOTEXT_DDUANZG1:IF(DUANZG1,DUANZG1,DRAWNULL),COLORFF8000;{画段中枢高}
NOTEXT_DDUANZD1:IF(DUANZD1,DUANZD1,DRAWNULL),COLORFF8000;{画段中枢低}
NOTEXT_DDUANSE1:STICKLINE(DUANSE1,DUANZD1,DUANZG1,0,0),COLORFF8000;{画段中枢起始结束};
```

## 买卖点信号

插件新增了一买、二买、三买信号检测功能，每个信号都提供了基础版本和严格版本。

### 函数编号说明

- **函数10号**: 二买信号(基础版本)
- **函数11号**: 二买信号(严格版本)
- **函数12号**: 一买信号(基础版本)
- **函数13号**: 一买信号(严格版本)
- **函数14号**: 三买信号(基础版本)
- **函数15号**: 三买信号(严格版本)

### 使用示例

在通达信公式中使用买点信号：

```text
{一买信号}
YIMAI:=TDXDLL2(12,FRAC,H,L);{一买信号-基础版}
DRAWICON(YIMAI,L*0.98,1);{在一买点画图标}

{二买信号}
ERMAI:=TDXDLL2(10,FRAC,H,L);{二买信号-基础版}
DRAWICON(ERMAI,L*0.98,2);{在二买点画图标}

{三买信号}
SANMAI:=TDXDLL2(14,FRAC,H,L);{三买信号-基础版}
DRAWICON(SANMAI,L*0.98,3);{在三买点画图标}
```

### 买点信号说明

- **一买(第一类买点)**: 价格跌破中枢后出现背驰，是最激进的买点，风险较高但潜在收益最大
- **二买(第二类买点)**: 价格回抽中枢后不破前低，是较为平衡的买点
- **三买(第三类买点)**: 价格突破中枢向上后的回调买点，是最稳健但潜在收益相对较小的买点

建议根据个人风险偏好选择合适的买点：
- 激进投资者可关注一买
- 稳健投资者建议关注二买和三买
- 严格版本的信号判断更为严格，假信号更少但可能错过部分机会

## 交流

- 缠论X星球: https://t.zsxq.com/0aDUuhQC5
- WeChat: kldcty
- QQ: 1106628276
- 微信公众号: mywildquant

缠论X星球和QQ群福利: 有新版本下载。
