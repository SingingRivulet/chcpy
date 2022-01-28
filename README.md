# 基于hmm的自动和弦生成工具：  
执行`./test/chordGen`可查看效果  
实际效果：
输入
```
[0,0,0,0,45,45,45,45,48,48,48,48,48,48,41,41,43,43,43,43,43,43,43,43,0,0,0,0,0,0,0,0,0,0,0,0,43,43,43,43,45,45,45,45,45,45,43,43,41,41,41,41,41,41,41,41,0,0,0,0,0,0,0,0,45,45,45,45,45,45,45,45,0,0,0,0,0,0,0,0,0,0,0,0,45,45,45,45,50,50,50,50,50,50,50,50,50,50,46,46,46,46,46,46,0,0,0,0,0,0,0,0,0,0,0,0,43,43,43,43,48,48,48,48,48,48,50,50,48,48,48,48,48,48,48,48,0,0,0,0,0,0,0,0,0,0,48,48,48,48,48,48,48,48,48,48,50,50,45,45,45,45,45,45,45,45,45,45,0,0,0,0,0,0,0,0,45,45,45,45,45,45,45,45,0,0,0,0,0,0,0,0,0,0,45,45,45,45,45,45,50,50,50,50,50,50,50,50,50,50,46,46,46,46,46,46,0,0,0,0,0,0,0,0,0,0,0,0,43,43,43,43,48,48,48,48,48,48,50,48,48,48,48,48,48,48,48,48,0,0,0,0,0,0,0,0,0,0,0,0,48,48,48,48,48,48,48,48,48,48,48,50,50,50,50,50,50,50,50,50,0,0,0,0,0,0,0,0,0,0,0,0,50,50,50,50,53,53,53,53,53,53,50,50,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,53,0,0,0,0,50,50,50,50,53,53,53,53,53,53,50,50,52,52,52,52,52,52,52,52,0,0,0,0,0,0,0,0,0,0,0,0,52,52,52,52,52,52,52,52,52,52,52,52,53,53,53,53,53,53,50,50,50,50,50,50,48,48,46,46,46,46,46,46,46,46,46,46,53,53,46,46,46,46,43,43,45,45,45,45,45,45,45,45,0,0,0,0,0,0,0,0]
```
输出  
```
41_M3(w=0.000000) len(seq)=4
旋律		模型输出	实际结果
[0 0 0 0 ]	[0 4 8 ]	(29 33 36 )
[45 45 45 45 ]	[0 4 8 ]	(29 33 36 )
[48 48 48 48 ]	[0 4 8 ]	(29 33 36 )
[48 48 41 41 ]	[0 4 8 ]	(29 33 36 )
43_-5(w=2.000000) len(seq)=6
旋律		模型输出	实际结果
[43 43 43 43 ]	[0 4 8 ]	(31 35 38 )
[43 43 43 43 ]	[0 4 8 ]	(31 35 38 )
[0 0 0 0 ]	[0 4 8 ]	(31 35 38 )
[0 0 0 0 ]	[0 4 8 ]	(31 35 38 )
[0 0 0 0 ]	[0 4 8 ]	(31 35 38 )
[43 43 43 43 ]	[0 4 8 ]	(31 35 38 )
41_-5(w=3.500000) len(seq)=12
旋律		模型输出	实际结果
[45 45 45 45 ]	[0 4 8 ]	(29 33 36 )
[45 45 43 43 ]	[0 4 8 ]	(29 33 36 )
[41 41 41 41 ]	[0 4 8 ]	(29 33 36 )
[41 41 41 41 ]	[0 4 8 ]	(29 33 36 )
[0 0 0 0 ]	[0 4 8 ]	(29 33 36 )
[0 0 0 0 ]	[0 4 8 ]	(29 33 36 )
[45 45 45 45 ]	[0 4 8 ]	(29 33 36 )
[45 45 45 45 ]	[0 4 8 ]	(29 33 36 )
[0 0 0 0 ]	[0 4 8 ]	(29 33 36 )
[0 0 0 0 ]	[0 4 8 ]	(29 33 36 )
[0 0 0 0 ]	[0 4 8 ]	(29 33 36 )
[45 45 45 45 ]	[0 4 8 ]	(29 33 36 )
43_m3(w=0.000000) len(seq)=8
旋律		模型输出	实际结果
[50 50 50 50 ]	[0 3 8 ]	(31 34 38 )
[50 50 50 50 ]	[0 3 8 ]	(31 34 38 )
[50 50 46 46 ]	[0 3 8 ]	(31 34 38 )
[46 46 46 46 ]	[0 3 8 ]	(31 34 38 )
[0 0 0 0 ]	[0 3 8 ]	(31 34 38 )
[0 0 0 0 ]	[0 3 8 ]	(31 34 38 )
[0 0 0 0 ]	[0 3 8 ]	(31 34 38 )
[43 43 43 43 ]	[0 3 8 ]	(31 34 38 )
45_m3(w=4.500000) len(seq)=20
旋律		模型输出	实际结果
[48 48 48 48 ]	[4 9 12 ]	(36 40 43 )
[48 48 50 50 ]	[4 9 12 ]	(36 40 43 )
[48 48 48 48 ]	[4 9 12 ]	(36 40 43 )
[48 48 48 48 ]	[4 9 12 ]	(36 40 43 )
[0 0 0 0 ]	[4 9 12 ]	(36 40 43 )
[0 0 0 0 ]	[4 9 12 ]	(36 40 43 )
[0 0 48 48 ]	[4 9 12 ]	(36 40 43 )
[48 48 48 48 ]	[4 9 12 ]	(36 40 43 )
[48 48 48 48 ]	[4 9 12 ]	(36 40 43 )
[50 50 45 45 ]	[4 9 12 ]	(36 40 43 )
[45 45 45 45 ]	[6 11 12 0 ]	(26 30 31 33 )
[45 45 45 45 ]	[6 11 12 0 ]	(26 30 31 33 )
[0 0 0 0 ]	[6 11 12 0 ]	(26 30 31 33 )
[0 0 0 0 ]	[6 11 12 0 ]	(26 30 31 33 )
[45 45 45 45 ]	[6 11 12 0 ]	(26 30 31 33 )
[45 45 45 45 ]	[6 11 12 0 ]	(26 30 31 33 )
[0 0 0 0 ]	[6 11 12 0 ]	(26 30 31 33 )
[0 0 0 0 ]	[6 11 12 0 ]	(26 30 31 33 )
[0 0 45 45 ]	[6 11 12 0 ]	(26 30 31 33 )
[45 45 45 45 ]	[6 11 12 0 ]	(26 30 31 33 )
43_m3(w=0.000000) len(seq)=8
旋律		模型输出	实际结果
[50 50 50 50 ]	[0 3 8 ]	(31 34 38 )
[50 50 50 50 ]	[0 3 8 ]	(31 34 38 )
[50 50 46 46 ]	[0 3 8 ]	(31 34 38 )
[46 46 46 46 ]	[0 3 8 ]	(31 34 38 )
[0 0 0 0 ]	[0 3 8 ]	(31 34 38 )
[0 0 0 0 ]	[0 3 8 ]	(31 34 38 )
[0 0 0 0 ]	[0 3 8 ]	(31 34 38 )
[43 43 43 43 ]	[0 3 8 ]	(31 34 38 )
48_-5(w=3.500000) len(seq)=10
旋律		模型输出	实际结果
[48 48 48 48 ]	[0 4 8 ]	(36 40 43 )
[48 48 50 48 ]	[0 4 8 ]	(36 40 43 )
[48 48 48 48 ]	[0 4 8 ]	(36 40 43 )
[48 48 48 48 ]	[0 4 8 ]	(36 40 43 )
[0 0 0 0 ]	[0 4 8 ]	(36 40 43 )
[0 0 0 0 ]	[0 4 8 ]	(36 40 43 )
[0 0 0 0 ]	[0 4 8 ]	(36 40 43 )
[48 48 48 48 ]	[0 4 8 ]	(36 40 43 )
[48 48 48 48 ]	[0 4 8 ]	(36 40 43 )
[48 48 48 50 ]	[0 4 8 ]	(36 40 43 )
50_m3(w=1.000000) len(seq)=16
旋律		模型输出	实际结果
[50 50 50 50 ]	[0 4 8 ]	(38 41 45 )
[50 50 50 50 ]	[0 4 8 ]	(38 41 45 )
[0 0 0 0 ]	[0 4 8 ]	(38 41 45 )
[0 0 0 0 ]	[0 4 8 ]	(38 41 45 )
[0 0 0 0 ]	[0 4 8 ]	(38 41 45 )
[50 50 50 50 ]	[10 0 4 ]	(35 38 41 )
[53 53 53 53 ]	[10 0 4 ]	(35 38 41 )
[53 53 50 50 ]	[10 0 4 ]	(35 38 41 )
[53 53 53 53 ]	[10 0 4 ]	(35 38 41 )
[53 53 53 53 ]	[10 0 4 ]	(35 38 41 )
[53 53 53 53 ]	[10 0 4 ]	(35 38 41 )
[53 53 53 53 ]	[10 0 4 ]	(35 38 41 )
[0 0 0 0 ]	[10 0 4 ]	(35 38 41 )
[50 50 50 50 ]	[10 0 4 ]	(35 38 41 )
[53 53 53 53 ]	[10 0 4 ]	(35 38 41 )
[53 53 50 50 ]	[10 0 4 ]	(35 38 41 )
52_-5(w=2.000000) len(seq)=8
旋律		模型输出	实际结果
[52 52 52 52 ]	[0 4 8 ]	(40 43 47 )
[52 52 52 52 ]	[0 4 8 ]	(40 43 47 )
[0 0 0 0 ]	[0 4 8 ]	(40 43 47 )
[0 0 0 0 ]	[0 4 8 ]	(40 43 47 )
[0 0 0 0 ]	[0 4 8 ]	(40 43 47 )
[52 52 52 52 ]	[0 4 8 ]	(40 43 47 )
[52 52 52 52 ]	[0 4 8 ]	(40 43 47 )
[52 52 52 52 ]	[0 4 8 ]	(40 43 47 )
43_m7(w=2.500000) len(seq)=8
旋律		模型输出	实际结果
[53 53 53 53 ]	[3 8 12 ]	(34 38 41 )
[53 53 50 50 ]	[3 8 12 ]	(34 38 41 )
[50 50 50 50 ]	[3 8 12 ]	(34 38 41 )
[48 48 46 46 ]	[3 8 12 ]	(34 38 41 )
[46 46 46 46 ]	[3 8 12 ]	(34 38 41 )
[46 46 46 46 ]	[3 8 12 ]	(34 38 41 )
[53 53 46 46 ]	[3 8 12 ]	(34 38 41 )
[46 46 43 43 ]	[3 8 12 ]	(34 38 41 )
45_-5(w=2.000000) len(seq)=4
旋律		模型输出	实际结果
[45 45 45 45 ]	[0 4 8 ]	(33 36 40 )
[45 45 45 45 ]	[0 4 8 ]	(33 36 40 )
[0 0 0 0 ]	[0 4 8 ]	(33 36 40 )
[0 0 0 0 ]	[0 4 8 ]	(33 36 40 )
=========================================================
```