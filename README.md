# 课程信息管理系统

最新版本: 2020/4/1

Update:

* 增加了安排考试功能
* 增加了对用户密码的SHA256加密
* 优化了登出或右上角时存档速度
* 修复了注册界面切换到管理员身份时的bug

提供一份652课程，998个学生的测试样本

**PS**: 课程数据爬取自http://elite.nju.edu.cn/jiaowu/student/teachinginfo/allCourseList.do?method=getCourseList&curTerm=20192&curGrade=2019

**PS2**:学生选课数据随机产生

**PS3**: SHA256程序来自 http://www.zedwood.com/article/cpp-sha256-function



## 文件依赖

注: 系统暂时仅支持`UTF-8`编码的文件，使用课程提供的`GB2312`编码的文件可能出现乱码

`./data`目录下有如下文件

* *student.txt*
* *currentcourse.txt*
* *assistant.txt*

可能有以下文件

* *defaultcourse.txt*
* *exam.txt*
* `./data/students/`*[student_name$].txt*

由于检查文件内容是否匹配的功能尚待开发，**第一次使用请确保*assistant.txt*, *currentcourse.txt*的信息匹配**，或**将上述文件清空**

### student.txt

用于存储学生ID和密码

格式为`studentName,password[SHA256]\n`

![image-20200401224922424](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200401224922424.png)

### currentcourse.txt/defaultcourse.txt

系统打开时自动从*currentcourse.txt*中读取课程数据

在管理员窗口可以选择从文件中读入课程，选择文件*defaultcourse.txt*

内容格式为

`课程号\t课程名\t教师名\t限额\t已选\t（非）专业课\n`

![image-20200401230142369](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200401230142369.png)

**注意**：已选人数不会直接读入，而是程序根据`data/student/`内的学生选课信息计算出，以保证信息准确。因此**新读入的课程选课人数必定会初始化为0**

### assistant.txt

管理课程助教信息，格式为`课程号\t助教1,助教2,助教i\n`

![image-20200401230713103](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200401230713103.png)

### [student_name$].txt

记录学生选择的课程和该课程选择的助教（如无则为Null）

格式为`课程号\t助教名\n`

![image-20200401230843701](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200401230843701.png)

### exam.txt

在管理员界面中由管理员操作生成

格式为`课程号\t考试场次\n`

（左边课程号补充前置0，如1改写成001，不影响使用）

![image-20200401231037484](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200401231037484.png)

## 登陆界面

![image-20200401225015399](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200401225015399.png)

* 默认为学生身份、登录模式
* 按需要切换为管理员或注册
* 目前仅支持唯一管理员Admin

![image-20200401225131845](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200401225131845.png)

* 注册界面 需要确认密码

最新版修复了从注册界面点击切换到管理员时不同时切换到登录模式的bug



## 管理员界面

用户名：Admin 密码：admin

![image-20200401225426559](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200401225426559.png)

初始界面如上，

### 从文件读入

点击从文件读入，即可弹出文件选择框。选择合适的*defaultcourse.txt*文件（文件名不一定是defaultcourse）

**注**：如果新加入的课程与原有课程的课程号冲突，则前者将被忽略

![image-20200401225544752](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200401225544752.png)

### 课程

可以查看全部课程，并提供如下功能

* 按缩写搜索课程
* 添加/删除课程
* 修改课程
* 查看某课程助教及其所指导小组的成员名单

![image-20200401231546124](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200401231546124.png)

#### 按缩写搜索课程

**定义A是B的缩写**为：A中的所有字符都在B中，且出现先后顺序相同

e.g. **程序设计基础实验 ** 可能的缩写：程设、程设实验、程计验、ect.

特别的，空串是所有字符串的缩写。即**搜索空串将展示所有课程**

![image-20200401232024170](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200401232024170.png)

#### 添加/删除课程

**添加课程**可点击【新课程】，然后在弹窗中输入课程信息

暂时添加课程后需要刷新才能看到

![image-20200401232125357](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200401232125357.png)

**删除课程**直接点击课表中的删除即可

其中选课人数若不为$0$，则不可删除课程

> **关于删除中表格更新的实现**
>
> 点击对应某个课程的删除按钮后需要删除表格中该行
>
> 由于经过若干次删除操作后课程对应的行数会发生改变，需要维护每个课程所在的表格行数
>
> 利用列表展示中课程序号为严格增序的特点，
>
> 如果某个课程$i$存在（未被删除），则记为$a[i]=1$，否则$a[i]=0$
>
> 则某个课程$i$所在的行数为$a[i]$的前缀和
>
> 因此使用**树状数组**来维护每个课程所在表格行数，增删课程复杂度$O(\log n)$

#### 修改课程

仅开放**教师名**，**限额**，**课程类型**三项的修改，其中限额不得低于现有已选课的人数

修改方式：双击修改项，在弹窗中输入新信息

![image-20200401233131136](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200401233131136.png)

#### 查看助教及小组内成员名单

单击课程所在行的任意一项，将会在窗口右侧显示当前课程的助教及其小组成员名单

![image-20200401233657248](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200401233657248.png)



### 安排考试

直接单击安排考试即可。系统将采用贪心算法进行考试安排，保证所有学生在一个场次中至多有一场考试需要参加。最坏复杂度$O(n^2)$,最大考试数为$\max(\mbox{degree}_i+1)$，$n$为课程数，$\mbox{degree}_i$为不能与课程$i$安排在同一场的课程数

![image-20200401234236131](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200401234236131.png)



### 存档

右上角关闭或者登出时执行存档。



## 学生界面

提供的测试样本用户名密码为

用户名: student$i$， 密码:pass$i$

如（student10,pass10)

![image-20200401234657720](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200401234657720.png)

### 我的课程

可查看所有已选课程，点开时对选课信息进行判断，可能弹出以下窗口

* 若选课超过10门：提示选课过多
* 若专业课少于4门，非专业课少于2门：提示选课不足
* 若课程助教退课：提示某课程某助教已退课（并自动重设该课程助教为Null）

我的课程界面除查看已选的课程外提供以下功能：

* 选择助教
* 报名成为助教（最多两门）
* 退课/重选

#### 选择助教

点开ComboBox可选，若自己也为该课程的助教，没有选自己的选项

![image-20200401235521435](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200401235521435.png)

#### 选择成为助教

点击选择成为助教即可。其中如果已选成为助教的课程达到两门，在取消成为某课程的助教前，隐藏其他课程的成为助教按钮

![image-20200401235637876](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200401235637876.png)

#### 退课

点击退课即可，退课后**有后悔机会**：该行不会消失，退课按钮变为选课按钮，

![image-20200401235738875](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200401235738875.png)

### 所有课程

除查看所有课程外，还提供根据缩写搜索课程的选项和选课功能。（缩写规则参考管理员窗口中的缩写搜索课程）

![image-20200401235922530](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200401235922530.png)

### 考试安排

点击若无考试安排（找不到exam.txt文件或文件为空），则弹窗显示无考试

若有考试，显示考试安排

![image-20200402000022914](https://github.com/yegcjs/CourseManageSystem/raw/master/img/image-20200402000022914.png)

### 存档

右上角关闭和登出时存档

