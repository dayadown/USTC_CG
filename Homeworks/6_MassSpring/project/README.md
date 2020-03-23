# 作业完成说明

## 项目说明

- 读取 *.obj 格式面网格或 *.tet 格式的体网格，生成类 MassSpring 用于传输用于模拟的网格数据
- MassSpring 类中存储了模拟方法类 Simulate；
- 程序 UI 窗口最上方设置两个按钮分别为开始（或继续）模拟时间计时和停止时间计时
- attribute中新添了Simulation组件用于设置模拟的一些参数


## 作业完成步骤

- 本次作业主要完成类 Simulation 中 Init 和 SimulateOnce 接口函数，前者主要初始化一些模拟用到的矩阵和辅助变量，会在每次更改模拟参数（如劲度系数）时调用从而修改相应的一些辅助变量；后者则是更新下一帧的位置position和速度velocity；
- 模拟只需要顶点的位置和边的索引信息（边索引为点的序号的列表，每两个序号表示一条边由哪两个点连接），均已在Simulation中建立完成

## 具体实施步骤

- 根据20-80原则思考应该如何逐步完成？