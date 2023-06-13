# novel_compiler
Compilation Principle Assignment

## 项目运行

拉取项目到本地

```
git clone git@github.com:Kingsdom005/novel_compiler.git
```

请使用`vs`新建项目，将源文件添加到资源列表即可成功运行。

## 项目修改和提交

项目修改和提交

```
# step1 获取仓库最新版本，防止冲突
git pull
# step2 添加目录下所有文件，需要忽略的文件请在.gitignore中添加
git add .
# step3 添加提交信息
git commit -m "此次提交需要说明的信息"
# step4 推送到远程仓库main分支
git push origin main
```

## 项目结构说明

项目结构说明如下：

```
- novel_compiler
# 头文件
-- cfg,h
-- grammar.h
-- lr.h
-- scanner.h
-- token.h
# 源文件
-- cfg.cpp
-- grammar.cpp
-- lr.cpp
-- scanner.cpp    # 用于读入文件
-- token.cpp
-- main.cpp       # 主函数入口
```

## 项目输入输出说明

项目输出说明如下：

```
# 说明：请把每个步骤的输入文件和中间结果和最终结果的输出文件补充完整

## 词法分析
输入文件：
中间文件：
输出文件：

## 语法分析
输入文件：
中间文件：
输出文件：

## 语义分析
输入文件：
中间文件：
输出文件：

## 中间代码生成
输入文件：
中间文件：
输出文件：

## 中间代码优化
输入文件：
中间文件：
输出文件：

```

