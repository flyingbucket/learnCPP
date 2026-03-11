# 设置变量
BUILD_DIR := "build"

# 默认行为：列出帮助信息
default:
    @just --list

# 1. 编译整个项目
build:
    cmake -B {{BUILD_DIR}}
    cmake --build {{BUILD_DIR}}

# 2. 交互式选择并运行 (核心功能)
# 此命令会列出 build/bin 下所有的可执行文件供你选择
run:
    @target=$(ls {{BUILD_DIR}}/bin/* 2>/dev/null | xargs -n 1 basename | fzf --prompt="请选择要运行的算法程序 > ") ; \
    if [ -n "$target" ]; then \
        echo "正在运行: $target" ; \
        {{BUILD_DIR}}/bin/$target ; \
    else \
        echo "未选择任何目标。" ; \
    fi

# 3. 快速重编译并运行单个目标 (如果知道名字)
# 用法: just run-single mainSLinkList
run-single target:
    cmake --build {{BUILD_DIR}} --target {{target}}
    {{BUILD_DIR}}/bin/{{target}}

# 4. 清理构建目录
clean:
    rm -rf {{BUILD_DIR}}

# 5. 生成编译数据库 (用于 LSP 代码跳转)
setup:
    cmake -B {{BUILD_DIR}} -DCMAKE_EXPORT_COMPILE_COMMANDS=1
    ln -sf {{BUILD_DIR}}/compile_commands.json compile_commands.json
