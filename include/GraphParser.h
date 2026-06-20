#ifndef GRAPHPARSER_H
#define GRAPHPARSER_H

#include "Graph.h"

#include <string>
#include <vector>

/// 图数据文本解析器 — 严格边语法
///
/// 边格式: A x [W] y B  (x, y = 操作符两端, [W] = 可选权重)
///   '-' 为 A/W/B 的分隔符:
///     W 缺失时: x+y 合计 ≥ 1 个 '-'
///     W 存在时: x、y 各自 ≥ 1 个 '-'
///   '<' 只能在 x 的最左端，'>' 只能在 y 的最右端，各最多一次
///   W 须可解析为浮点数 (负数须用引号如 "-2")
///
/// 示例:
///   A---B      无向无权边
///   A-->B      有向边 A→B
///   A<---B     有向边 B→A
///   A-3--B     无向带权边 (weight=3)
///   A-2.5->B   有向带权边 A→B (weight=2.5)
///   A<-3--B    有向带权边 B→A (weight=3)
///   A-"-2"->B  有向带权边 A→B (weight=-2)
///
/// 顶点名: 不含 ' ', '-', '<', '>', '#', '"' 时可省略引号;
///         否则须用 "..." 包裹 (内部 \" \\ 转义)
/// 支持 # 注释、空行、孤立点、同名后缀 (name(N))
class GraphParser {
public:
    /// 从多行文本解析，构建并返回 Graph
    /// @param text  输入文本（支持 \\n 或 \\r\\n）
    /// @param errors  解析错误信息（输出参数）
    /// @return 解析后的图（即使有部分错误也会返回已解析的部分）
    static Graph parse(const std::string& text, std::vector<std::string>& errors);

    /// 将 Graph 序列化为文本格式
    static std::string serialize(const Graph& graph);

    /// 解析单行
    /// @return true 解析成功
    static bool parseLine(const std::string& line, Graph& graph,
                          std::string& error);
};

#endif // GRAPHPARSER_H
