# 流程

## 主流程

1. 初始化全局数据 `NanoVM_GLOBAL_init`
1. 这时会依次初始化各模块全局数据
1. `NanoVM_GLOBAL_init_code`会opcode_meta
1. `NanoVM_GLOBAL_init_stack`会初始化insn_handle
1. `NanoVM_GLOBAL_init_native`会初始化native_handle
1. 全局初始化完成
1. 创建vm `NanoVM_create`
1. 这时会依次创建如下模块的manager
1. `NanoVM_create_mem_mgr` momory模块
1. `NanoVM_create_code_mgr` code模块，初始化primitive类型
1. `NanoVM_create_stack_mgr` stack模块
1. `NanoVM_create_thread_mgr` thread模块
1. 但是这时并不会创建native_mgr
1. 创建vm完成
1. 解析字节码 `NanoVM_parse_code`
1. 使用cfr解析为Class对象
1. 遍历所有Class对象名称，生成type_t对象
1. 遍历所有Class对象，向type_t对象中填入字段和方法，此时方法CODE段对应insn只填入opcode
1. 遍历所有CODE段，填入opcode内容
1. 在解析后，创建native模块 `NanoVM_create_native_mgr`
1. 解析字节码完成
1. 这时native模块从code模块获取所需的类型数据，以便构建`native_meth_t`和`native_handle_t`
1. 此时所有模块manager初始化完成
1. 启动vm `NanoVM_start`
1. 依次进行如下步骤
1. 创建线程 `NanoVM_create_thread` 并立即启动
1. 创建stack并绑定到该线程上 `NanoVM_create_stack`
1. 寻找main method `NanoVM_get_meth`
1. 从操作系统传进来的字符串作为局部变量
1. 走“方法调用流程”
1. 从返回结果中拿到异常时，将异常抛给外部，否则获取返回值并转换为int
1. 按照和（创建）相反的顺序释放各manager，然后释放vm
1. 按照和（初始化）相反的顺序释放各全局数据
1. 将返回值返回给操作系统

## 方法调用流程

需要有一个stack、一个method、一些局部变量表的对象

1. 在stack上根据method和局部变量表对象创建frame `NanoVM_create_frame`
1. 如果这个method是native的，则在native模块寻找对应native_meth `NanoVM_get_native_meth`
1. 从native_meth中获取native_handle
1. 执行native_handle
1. 如果这个method不是native的，则执行 `NanoVM_frame_start`
1. 设置当前insn为第一个insn
1. loop: 这是一个循环，每个循环都执行一条insn
1. 对于每个insn，获取其对应的handle，然后进行调用
1. 当其opcode为return系列或athrow时，执行该insn然后跳出循环
1. 如果执行了一条insn后，frame上的ex被赋值，也需要跳出循环
1. 循环结束
1. 如果ex非空，则寻找对应的exception_table
1. 如果找到了，则将当前insn设置为对应的insn，回到loop
1. 如果没找到，则将该ex放入上一个frame，然后pop当前frame。如果没有上一frame则异常，然后退出当前线程
1. 如果ex为空，则检查方法返回是否为void
1. 如果为void，则直接pop该frame
1. 如果不为void，则将返回值push入上一frame的opstack中。如果没有上一frame，则退出当前线程，返回到操作系统

## 其他

### 新建线程

1. stack-manager调用native方法`nanovm.os.thread`
1. stack-manager向thread-manager传入`nanovm.os.Runnable`的对象
1. stack-manager创建对应线程调用栈
1. stack-manager创建栈帧
1. thread-manager创建新线程，绑定刚才创建的栈帧，并将该对象`run`方法压入栈
1. stack-manager调用native方法`thread#start`
1. thread-manager启动新线程

### 新建对象

1. memory-manager规划一块内存并返回，该内存包括header和对象需要占用的内存

### 引用计数增删

1. 局部变量表清空/覆盖时，原先的非空对象计数-1
1. 操作数栈pop/清空时，对应非空对象计数-1
1. 一个对象被释放时，其内部所指向所有对象计数各-1
1. 栈帧pop时，局部变量表和操作数栈清空，引起引用计数减少
1. 插入局部变量表时，插入对象计数+1
1. push操作数栈时，该非空对象计数+1
1. 对象被赋给字段时，该对象计数+1

* 注：所有申请与释放内存的地方都会向memory-manager发起调用，具体来说，就是申请某个类型的内存，或者归还某个类型的内存。归还内存时，里面未归还的对象将被一并归还
