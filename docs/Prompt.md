# 图数据格式提示词模板

将以下提示词附加到你的对话中，即可让大语言模型将任意图数据（图片、非结构化文本、不确定格式等）转换为 GraphViz 的标准输入格式。

---

## 提示词

````
Output graph data in this format only.

Names: any chars except space, -, <, >, ", #.
Quote names with these: "..." (escape \" \\).
Unquoted - in names is invalid: use "my-node".

Edge:  VERTEX [<] DASHES [WEIGHT] DASHES [>] VERTEX
  ≥1 dash required; with weight, ≥1 dash EACH side.
  < at start → reversed; > at end → forward. Each ≤1 time.
  Spaces allowed between parts.

  A---B     undirected         A-3--B    undirected, wt 3
  A-->B     directed A→B       A-2.5->B  directed A→B, wt 2.5
  A<-B      directed B→A       A<-3--B   directed B→A, wt 3
  A<-->B    bidirectional       A-"-2"->B negative wt

Isolated: name alone on a line, e.g. D or "孤立节点".
Same-label: N(suffix), suffix = non-negative integer, e.g. 2(1)---3.
Self-loop: A---A, A-->A. Parallel: repeat pair.
# comments and blank lines allowed.

Example:
```
A---B
B-3->C
C-->A
D
```
Output ONLY graph text, no explanations or code fences.
````

---

## 使用方式

1. 复制上方提示词块。
2. 在其后附上你的图数据来源（图片、非标准格式文本、自然语言描述等均可）。
3. LLM 输出的内容可直接粘贴到 GraphViz 编辑器中。
