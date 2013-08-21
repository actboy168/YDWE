// Edit By Warft_TigerCN

define {
    
    while = whilenot not

    // 根据条件a的值设置两个数据b与c使用的优先权，相当于C语言里的三目运算符“ ?:”。
    SetPriority(a, b, c) = { if a { return b; } else { return c; } }

    // 控制类循环语句，其中var作为控制变量，监测各个常量const，如符合则执行相应的cont
    // ，都不符合则返回默认的def值。类似于C语言里的switch循环语句。以下根据重载扩展至
    // 最多拥有10个常量和一个默认值，但至少得拥有3个常量。

    SwitchMany(var, const1, cont1, const2, cont2, const3, cont3, def) = {
        if(var == const1) {
            cont1;
            return;
        }
        elseif(var == const2) {
            cont2;
            return;
        }
        elseif(var == const3) {
            cont3;
            return;
        }
        else {
            def;
            return;
        }
    }

}