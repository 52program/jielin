
---
### 一、基础语法
```mermaid
sequenceDiagram
    participant O as 冈部伦太郎
    participant β as β世界线
    participant α as α世界线
    participant SG as SG线

    Note over O,β: 基础注释写法<br/>必须使用英文冒号
    O->>β: 同步消息(实线箭头)

    β-->>O: 异步返回(虚线箭头)
```


1. 循环结构
```mermaid
sequenceDiagram

loop 循环说明
    A->>B: 消息内容
end
```

2. 关键路径
```mermaid
sequenceDiagram

critical 关键操作
    A->>B: 不可逆操作
    B-->>C: 世界线跃迁
end
```

✨3. 分支结构
```mermaid
sequenceDiagram

alt 成功路径
    A->>B: 操作成功
else 失败路径
    A-xC: 操作失败
end
```

✨4. 并行结构
```mermaid
sequenceDiagram

par 并行事件
    A->>B: 事件1
    and
    C->>D: 事件2
end
```


---

### 二、可运行的高级功能示例
```mermaid
sequenceDiagram
    participant O as 观测者
    participant β as β线
    participant α as α线

    Note left of O: 左侧单边注释
    Note right of β: 右侧注释<br/>换行用<br/>实现

    loop 时间循环
        O->>β: 发送D-Mail
        activate β
        β-->>α: 世界线跃迁
        deactivate β
        α-->>O: 收束响应
    end

    critical 关键操作
        O->>α: 删除记录
        α-->>β: 返回β线
        O->>β: 伪造事件
    end

    alt 分支判断
        β-->>SG: 成功跃迁
    else
        β--x O: 失败
    end
```

---

### 三、严格语法规范表，易错点

| 元素类型  | 正确写法                      | 错误写法                |
| ----- | ------------------------- | ------------------- |
| 参与者定义 | `participant A as 名称`     | `participant A: 名称` |
| 注释换行  | `Note over A: 第一行<br>第二行` | 使用`\n`              |
| 激活区块  | `activate A` 必须配对使用       | 单独使用activate        |
| 箭头符号  | `->>` `-->>` `-x`         | 使用全角箭头→             |
| 颜色定义  | `participant A #FF0000`   | 使用color:red         |
1. 跨参与者注释
```mermaid
sequenceDiagram
    
Note over 起点,终点: 覆盖多个参与者的注释
%% Note over A,B,C: 错误的写法
```


---

### 四、已验证的完整案例
```mermaid
sequenceDiagram
    participant O as 冈部伦太郎
    participant β as β世界线
    participant α as α世界线
    participant SG as SG线

    Note over O,β: 初始观测：β世界线<br/>牧濑红莉栖死亡
    O->>β: 发送D-Mail
    activate β
    β-->>α: 世界线跃迁
    deactivate β

    loop 收束循环
        α-->>O: 真由理死亡事件
        O->>α: 时间跳跃
    end

    critical 关键操作
        O->>α: Operation Urd
        activate α
        α-->>β: 返回β线
        deactivate α
        O->>β: Operation Skuld
        activate β
        β-->>SG: 进入SG线
        deactivate β
    end

    Note over SG: 世界线变动率 1.048598
```

---
