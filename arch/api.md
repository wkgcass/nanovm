# NanoVM API Design

# 面向软件使用者

```
cd $your_class_file_path
nanovm $your.clazz.name $arg0 $arg1
```

# 面向API使用者

使用时只需引入`nanovm.h`。

使用示例：

```c
int main(int argc, char** argv) {
  int err, bytecode_len;
  nanovm_config_t conf;
  nanovm_bytecode_t* bytecodes;

  // read bytecode and init conf
  // ...

  nanovm_t* vm = NanoVM_create(&conf);
  if (!vm) return -1;
  err = NanoVM_parse_code(vm, bytecode_len, bytecodes);
  if (err) return -1;
  err = NanoVM_start(vm, "demo.Sample");
  // will block the thread when success until executing finished
  NanoVM_release(vm);
  NanoVM_GLOBAL_free();
  if (err) return -1;
  return 0;
}
```

# 面向开发者

如果有不向模块外开放的API，统一在`${module}.internal.h`内定义。不同模块只能引用开放的API。

所有开放API均以`NanoVM_`开头。各模块API **不** 加自身独特前缀。模块内API可加模块前缀，例如`NVM_CODE_xxx|NVM_STACK_xxx`等。

所有类型均附带`nvm_`前缀。

* “操作”API，若不可能失败，则返回void，否则返回`int`,0=成功，-1=失败。
* “创建”API均返回创建对象的引用，NULL=失败。
* “释放”API均返回void。
* 失败时可以设置errno，需要在头文件注释中记录。
* 返回void的方法，在失败时直接core掉。
* 除了明确会接收NULL值的函数外，一律不检查NULL。
* 所有API不检查对象是否被初始化。创建API默认对象未初始化，其他API默认已初始化。
